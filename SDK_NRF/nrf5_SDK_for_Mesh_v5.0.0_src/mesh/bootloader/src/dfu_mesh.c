/* Copyright (c) 2010 - 2020, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include "dfu_mesh.h"
#include "sha256.h"
#include "dfu_transfer_mesh.h"
#include "dfu_types_mesh.h"
#include "uECC.h"
#include "bootloader_info.h"
#include "bootloader_app_bridge.h"
#include "nrf_mbr.h"
#include "rtt_log.h"
#include "dfu_util.h"
#include "dfu_bank.h"
#include "boards.h"
#include "app_error.h"

/*****************************************************************************
* Local defines
*****************************************************************************/
#define TX_REPEATS_DEFAULT              (3)
#define TX_REPEATS_FWID                 (TX_REPEATS_DEFAULT * 5)
#define TX_REPEATS_READY                (TX_REPEATS_DEFAULT)
#define TX_REPEATS_READY_FWD            (1)
#define TX_REPEATS_START                (TX_REPEATS_DEFAULT * 2)
#define TX_REPEATS_DATA                 (TX_REPEATS_DEFAULT)
#define TX_REPEATS_RSP                  (TX_REPEATS_DEFAULT)
#define TX_REPEATS_REQ                  (TX_REPEATS_INF)

#define TX_INTERVAL_TYPE_FWID           (BL_RADIO_INTERVAL_TYPE_REGULAR_SLOW)
#define TX_INTERVAL_TYPE_DFU_REQ        (BL_RADIO_INTERVAL_TYPE_REGULAR)
#define TX_INTERVAL_TYPE_DFU_REQ_FWD    (BL_RADIO_INTERVAL_TYPE_REGULAR_SLOW)
#define TX_INTERVAL_TYPE_READY          (BL_RADIO_INTERVAL_TYPE_REGULAR)
#define TX_INTERVAL_TYPE_DATA           (BL_RADIO_INTERVAL_TYPE_EXPONENTIAL)
#define TX_INTERVAL_TYPE_RSP            (BL_RADIO_INTERVAL_TYPE_EXPONENTIAL)
#define TX_INTERVAL_TYPE_REQ            (BL_RADIO_INTERVAL_TYPE_REGULAR_SLOW)

#define STATE_TIMEOUT_RAMPDOWN          (30000000)

#define TX_SLOT_BEACON                  (0)
#define TX_SLOT_READY_FORWARDING        (0) /**< Slot to use for ready-packet forwarding. */

#define START_ADDRESS_UNKNOWN           (0xFFFFFFFF)

#define REQ_CACHE_SIZE                  (4)
#define REQ_RX_COUNT_RETRY              (8)

#define DATA_REQ_SEGMENT_NONE           (0)

#define LOST_START_EDGE                 (10)

/*****************************************************************************
* Local typedefs
*****************************************************************************/
typedef enum
{
    BEACON_TYPE_FWID,
    BEACON_TYPE_READY_APP,
    BEACON_TYPE_READY_SD,
    BEACON_TYPE_READY_BL
} beacon_type_t;

typedef struct
{
    uint32_t        transaction_id;
    uint8_t         authority;
    dfu_type_t      type;
    uint32_t*       p_start_addr;
    uint32_t*       p_bank_addr;
    uint32_t*       p_indicated_start_addr;
    uint32_t*       p_last_requested_entry;
    uint32_t        length;
    uint32_t        signature_length;
    uint8_t         signature[DFU_SIGNATURE_LEN];
    uint8_t         signature_bitmap;
    uint16_t        segments_remaining;
    uint16_t        segment_count;
    fwid_union_t    target_fwid_union;
    bool            segment_is_valid_after_transfer;
    bool            flood;
} transaction_t;

typedef struct
{
    bl_info_version_t*      p_fwid;
    bl_info_segment_t*      p_segment_sd;
    bl_info_segment_t*      p_segment_bl;
    bl_info_segment_t*      p_segment_app;
    bl_info_flags_t*        p_flags;
    uint8_t*                p_ecdsa_public_key;
    uint8_t*                p_signature_sd;
    uint8_t*                p_signature_bl;
    uint8_t*                p_signature_app;
    uint8_t*                p_signature_bl_info;
} bl_info_pointers_t;

typedef struct
{
    uint16_t segment;
    uint16_t rx_count;
} req_cache_entry_t;
/*****************************************************************************
* Static globals
*****************************************************************************/
static transaction_t            m_transaction;
static dfu_state_t              m_state = DFU_STATE_FIND_FWID;
static bl_info_pointers_t       m_bl_info_pointers;
static req_cache_entry_t        m_req_cache[REQ_CACHE_SIZE];
static uint8_t                  m_req_index;
static uint8_t                  m_tx_slots;
static uint16_t                 m_data_req_segment;
static uint8_t                  m_lost_start_edge;

#ifdef RTT_LOG
static const char*              m_state_strs[] =
{
    "INITIALIZED",
    "FIND FWID",
    "DFU REQ",
    "DFU READY",
    "DFU TARGET",
    "VALIDATE",
    "STABILIZE",
    "RELAY CANDIDATE",
    "RELAY"
};
static const char*              m_beacon_type_strs[] =
{
    "FWID",
    "READY_APP",
    "READY_SD",
    "READY_BL"
};
static const char*              m_dfu_type_strs[] =
{
    "NONE",
    "SD",
    "BL",
    "?",
    "APP"
};
#endif

/*****************************************************************************
* Static Functions
*****************************************************************************/
/** Fetch all shortcut info page pointers. */
static void get_info_pointers(void)
{
    /* fetch persistent entries */
    m_bl_info_pointers.p_flags              = &bootloader_info_entry_get(BL_INFO_TYPE_FLAGS)->flags;                    /*lint !e613 Possible use of null pointer 'unknown-name' in left argument to operator '->'. */
    m_bl_info_pointers.p_fwid               = &bootloader_info_entry_get(BL_INFO_TYPE_VERSION)->version;                /*lint !e613 Possible use of null pointer 'unknown-name' in left argument to operator '->'. */
    m_bl_info_pointers.p_segment_app        = &bootloader_info_entry_get(BL_INFO_TYPE_SEGMENT_APP)->segment;            /*lint !e613 Possible use of null pointer 'unknown-name' in left argument to operator '->'. */
    m_bl_info_pointers.p_segment_bl         = &bootloader_info_entry_get(BL_INFO_TYPE_SEGMENT_BL)->segment;             /*lint !e613 Possible use of null pointer 'unknown-name' in left argument to operator '->'. */
    m_bl_info_pointers.p_segment_sd         = &bootloader_info_entry_get(BL_INFO_TYPE_SEGMENT_SD)->segment;             /*lint !e613 Possible use of null pointer 'unknown-name' in left argument to operator '->'. */
    m_bl_info_pointers.p_ecdsa_public_key   = &bootloader_info_entry_get(BL_INFO_TYPE_ECDSA_PUBLIC_KEY)->public_key[0]; /*lint !e613 Possible use of null pointer 'unknown-name' in left argument to operator '->'. */

    if (
        ((uint32_t) m_bl_info_pointers.p_flags              < BOOTLOADER_INFO_ADDRESS) ||
        ((uint32_t) m_bl_info_pointers.p_fwid               < BOOTLOADER_INFO_ADDRESS) ||
        ((uint32_t) m_bl_info_pointers.p_segment_app        < BOOTLOADER_INFO_ADDRESS) ||
        ((uint32_t) m_bl_info_pointers.p_segment_sd         < BOOTLOADER_INFO_ADDRESS) ||
        ((uint32_t) m_bl_info_pointers.p_segment_bl         < BOOTLOADER_INFO_ADDRESS)
       )
    {
#ifdef RTT_LOG
        __LOG("Missing a critical info pointer\n");
        if ((uint32_t) m_bl_info_pointers.p_flags              < BOOTLOADER_INFO_ADDRESS)
        {
            __LOG(RTT_CTRL_TEXT_RED "MISSING FLAGS\n");
        }
        if ((uint32_t) m_bl_info_pointers.p_fwid               < BOOTLOADER_INFO_ADDRESS)
        {
            __LOG(RTT_CTRL_TEXT_RED "MISSING FWID\n");
        }
        if ((uint32_t) m_bl_info_pointers.p_segment_app        < BOOTLOADER_INFO_ADDRESS)
        {
            __LOG(RTT_CTRL_TEXT_RED "MISSING SEGMENT APP\n");
        }
        if ((uint32_t) m_bl_info_pointers.p_segment_sd         < BOOTLOADER_INFO_ADDRESS)
        {
            __LOG(RTT_CTRL_TEXT_RED "MISSING SEGMENT SD\n");
        }
        if ((uint32_t) m_bl_info_pointers.p_segment_bl         < BOOTLOADER_INFO_ADDRESS)
        {
            __LOG(RTT_CTRL_TEXT_RED "MISSING SEGMENT BL\n");
        }
#endif
        send_end_evt(DFU_END_ERROR_INVALID_PERSISTENT_STORAGE);
    }
}

static uint32_t segment_count_from_start_packet(dfu_packet_t* p_packet)
{
    uint32_t start_address = p_packet->payload.start.start_address;
    if (start_address == 0xFFFFFFFF)
    {
        start_address = 0; /* It'll be aligned. */
    }
    uint32_t segment_count = ((p_packet->payload.start.length * 4) + (start_address & 0x0F) - 1) / 16 + 1;

    if (p_packet->payload.start.signature_length != 0)
    {
        segment_count += p_packet->payload.start.signature_length / SEGMENT_LENGTH;
    }
    if (segment_count > 0xFFFF)
    {
        /* can't have more than 65536 segments in a transmission */
        segment_count = 0xFFFF;
    }

    return segment_count;
}

#define SET_STATE(s) set_state(s, __LINE__)
static void set_state(dfu_state_t state, uint16_t line)
{
    m_state = state;
    __LOG(RTT_CTRL_TEXT_GREEN "NEW STATE: %s (L%u)\n", m_state_strs[(uint32_t) state], line);
}

static beacon_type_t state_beacon_type(dfu_type_t transfer_type)
{
    switch (transfer_type)
    {
        case DFU_TYPE_APP:
            return BEACON_TYPE_READY_APP;
        case DFU_TYPE_BOOTLOADER:
            return BEACON_TYPE_READY_BL;
        case DFU_TYPE_SD:
            return BEACON_TYPE_READY_SD;
        default:
            return BEACON_TYPE_FWID;
    }
}

static void send_progress_event(uint16_t segment, uint16_t total_segments)
{
    bl_evt_t segment_rx;
    segment_rx.type = BL_EVT_TYPE_DFU_DATA_SEGMENT_RX;
    segment_rx.params.dfu.data_segment.received_segment = segment;
    segment_rx.params.dfu.data_segment.total_segments = total_segments;
    (void) bootloader_evt_send(&segment_rx);
}

static uint32_t packet_tx_dynamic(dfu_packet_t* p_packet,
    uint32_t length,
    bl_radio_interval_type_t interval_type,
    uint8_t repeats)
{
    static uint8_t tx_slot = 1;
    if (tx_slot >= m_tx_slots || tx_slot == 0)
    {
        tx_slot = 1;
    }
    bl_evt_t tx_evt;
    tx_evt.type = BL_EVT_TYPE_TX_RADIO;
    tx_evt.params.tx.radio.p_dfu_packet = p_packet;
    tx_evt.params.tx.radio.length = length;
    tx_evt.params.tx.radio.interval_type = interval_type;
    tx_evt.params.tx.radio.tx_count = repeats;
    tx_evt.params.tx.radio.tx_slot = tx_slot;
    uint32_t status = bootloader_evt_send(&tx_evt);
    if (status == NRF_SUCCESS)
    {
        tx_slot++;
    }
    return status;
}

static bool signature_check(void)
{
    __LOG("Verifying signature... ");
    /* if we don't have a public key we will accept all firmware upgrades. */
    if (m_bl_info_pointers.p_ecdsa_public_key == NULL)
    {
        __LOG("No key (THAT'S OKAY THOUGH)\n");
        return true;
    }

    /* if we have a key, but the transfer isn't signed, we will fail */
    if (m_transaction.signature_length == 0)
    {
        __LOG("No signature (FAILURE)\n");
        return false;
    }

    uint8_t hash[uECC_BYTES];
    sha256_context_t hash_context;
    (void) sha256_init(&hash_context);
    (void) sha256_update(&hash_context, (uint8_t*) &m_transaction.type, 1);
    (void) sha256_update(&hash_context, (uint8_t*) &m_transaction.p_indicated_start_addr, 4);
    (void) sha256_update(&hash_context, (uint8_t*) &m_transaction.length, 4);
    uint8_t padding = 0;
    (void) sha256_update(&hash_context, &padding, 1);

    switch (m_transaction.type)
    {
        case DFU_TYPE_APP:
            (void) sha256_update(&hash_context, (uint8_t*) &m_transaction.target_fwid_union, DFU_FWID_LEN_APP);
            break;
        case DFU_TYPE_SD:
            (void) sha256_update(&hash_context, (uint8_t*) &m_transaction.target_fwid_union, DFU_FWID_LEN_SD);
            break;
        case DFU_TYPE_BOOTLOADER:
            (void) sha256_update(&hash_context, (uint8_t*) &m_transaction.target_fwid_union, DFU_FWID_LEN_BL);
            break;
        default:
            break;
    }

    (void) dfu_transfer_sha256(&hash_context);
#if NORDIC_SDK_VERSION >= 11
    (void) sha256_final(&hash_context, hash, false);
#else
    (void) sha256_final(&hash_context, hash);
#endif
    bool success = (bool) (uECC_verify(m_bl_info_pointers.p_ecdsa_public_key, hash, m_transaction.signature));

    if (success)
    {
        __LOG("OK\n");
    }
    else
    {
        __LOG("FAIL\n");
    }
    return success;
}

static void beacon_set(beacon_type_t type)
{
    dfu_packet_t dfu_packet;
    memset(&dfu_packet, 0, sizeof(dfu_packet_t));

    uint8_t repeats = TX_REPEATS_FWID;
    bl_radio_interval_type_t interval_type = TX_INTERVAL_TYPE_FWID;
    uint8_t length = 0;

    if (type != BEACON_TYPE_FWID)
    {
        dfu_packet.packet_type = DFU_PACKET_TYPE_STATE;
        dfu_packet.payload.state.authority = m_transaction.authority;
        dfu_packet.payload.state.flood = m_transaction.flood;
        dfu_packet.payload.state.transaction_id = m_transaction.transaction_id;
        dfu_packet.payload.state.relay_node = (m_state == DFU_STATE_RELAY_CANDIDATE);
        dfu_packet.payload.state.fwid = m_transaction.target_fwid_union;
        repeats = TX_REPEATS_READY;
        if (m_transaction.authority == 0)
        {
            interval_type = TX_INTERVAL_TYPE_DFU_REQ;
        }
        else
        {
            interval_type = TX_INTERVAL_TYPE_READY;
        }
    }

    switch (type)
    {
        case BEACON_TYPE_FWID:
            length = DFU_PACKET_LEN_FWID;
            dfu_packet.packet_type = DFU_PACKET_TYPE_FWID;
            dfu_packet.payload.fwid = *m_bl_info_pointers.p_fwid;
            break;

        case BEACON_TYPE_READY_APP:
            length = DFU_PACKET_LEN_STATE_APP;
            dfu_packet.payload.state.dfu_type = DFU_TYPE_APP;
            break;

        case BEACON_TYPE_READY_SD:
            length = DFU_PACKET_LEN_STATE_SD;
            dfu_packet.payload.state.dfu_type = DFU_TYPE_SD;
            break;

        case BEACON_TYPE_READY_BL:
            length = DFU_PACKET_LEN_STATE_BL;
            dfu_packet.payload.state.dfu_type = DFU_TYPE_BOOTLOADER;
            break;
    }

    bl_evt_t tx_evt;
    tx_evt.type = BL_EVT_TYPE_TX_RADIO;
    tx_evt.params.tx.radio.p_dfu_packet = &dfu_packet;
    tx_evt.params.tx.radio.length = length;
    tx_evt.params.tx.radio.interval_type = interval_type;
    tx_evt.params.tx.radio.tx_count = repeats;
    tx_evt.params.tx.radio.tx_slot = TX_SLOT_BEACON;

    (void) bootloader_evt_send(&tx_evt);

#ifdef RBC_MESH_SERIAL
    tx_evt.type = BL_EVT_TYPE_TX_SERIAL;
    tx_evt.params.tx.serial.p_dfu_packet = &dfu_packet;
    tx_evt.params.tx.serial.length = length;
    (void) bootloader_evt_send(&tx_evt);
#endif
    __LOG("BEACON SET: TYPE %s\n", m_beacon_type_strs[(uint32_t) type]);
    if (type != BEACON_TYPE_FWID)
    {
        __LOG("\t(DFU TYPE: %s)\n", m_dfu_type_strs[(uint32_t) dfu_packet.payload.state.dfu_type]);
    }
}

static uint32_t relay_packet(dfu_packet_t* p_packet, uint16_t length)
{
    uint32_t repeats = TX_REPEATS_DATA;
    if (p_packet->packet_type == DFU_PACKET_TYPE_DATA && p_packet->payload.data.segment == 0)
    {
        repeats = TX_REPEATS_START;
    }

    uint32_t status = packet_tx_dynamic(p_packet, length, TX_INTERVAL_TYPE_DATA, repeats);
    if (status == NRF_SUCCESS)
    {
        packet_cache_put(p_packet);
    }
    return status;
}

static void send_bank_notifications(void)
{
    const bl_info_type_t bank_types[] =
    {
        BL_INFO_TYPE_BANK_BL,
        BL_INFO_TYPE_BANK_SD,
        BL_INFO_TYPE_BANK_APP
    };
    const dfu_type_t dfu_types[] = {DFU_TYPE_BOOTLOADER, DFU_TYPE_SD, DFU_TYPE_APP};
    const void* p_curr_fwid[] =
    {
        &m_bl_info_pointers.p_fwid->bootloader,
        (void*) &m_bl_info_pointers.p_fwid->sd,
        &m_bl_info_pointers.p_fwid->app
    };

    /* check for available banks */
    for (uint32_t i = 0; i < 3; ++i)
    {
        bl_info_entry_t* p_bank_entry = bootloader_info_entry_get(bank_types[i]);
        if (p_bank_entry)
        {
            bl_evt_t bank_evt;
            bank_evt.type = BL_EVT_TYPE_BANK_AVAILABLE;
            bank_evt.params.bank_available.bank_dfu_type = dfu_types[i];
            fwid_union_cpy(
                    &bank_evt.params.bank_available.bank_fwid,
                    &p_bank_entry->bank.fwid, dfu_types[i]);
            fwid_union_cpy(
                    &bank_evt.params.bank_available.current_fwid,
                    (fwid_union_t*) p_curr_fwid[i],
                    dfu_types[i]);
            bank_evt.params.bank_available.p_bank_addr = p_bank_entry->bank.p_bank_addr;
            bank_evt.params.bank_available.bank_length = p_bank_entry->bank.length;
            bank_evt.params.bank_available.is_signed = p_bank_entry->bank.has_signature;
            (void) bootloader_evt_send(&bank_evt);
        }
    }
}
/********** STATE MACHINE ENTRY POINTS ***********/
static void start_find_fwid(void)
{
    beacon_set(BEACON_TYPE_FWID);
    SET_STATE(DFU_STATE_FIND_FWID);
    memset(&m_transaction, 0, sizeof(transaction_t));
}

static void start_req(dfu_type_t type, fwid_union_t* p_fwid)
{
    m_transaction.authority = 0;
    m_transaction.length = 0;
    m_transaction.p_start_addr = NULL;
    m_transaction.segments_remaining = 0xFFFF;
    m_transaction.segment_count = 0;
    m_transaction.segment_is_valid_after_transfer = false;
    m_transaction.signature_length = 0;
    m_transaction.transaction_id = 0;
    m_transaction.type = type;
    fwid_union_cpy(&m_transaction.target_fwid_union, p_fwid, type);
    SET_STATE(DFU_STATE_DFU_REQ);

    beacon_set(state_beacon_type(type));
}

static void start_ready(dfu_packet_t* p_ready_packet)
{
    if (p_ready_packet->packet_type != DFU_PACKET_TYPE_STATE ||
        p_ready_packet->payload.state.authority == 0 ||
        p_ready_packet->payload.state.dfu_type != m_transaction.type)
    {
        APP_ERROR_CHECK(NRF_ERROR_INVALID_PARAM);
    }
    m_transaction.transaction_id = p_ready_packet->payload.state.transaction_id;
    m_transaction.authority = p_ready_packet->payload.state.authority;
    m_transaction.flood = p_ready_packet->payload.state.flood;
    SET_STATE(DFU_STATE_READY);

    beacon_set(state_beacon_type(m_transaction.type));
}

static void start_target(void)
{
    SET_STATE(DFU_STATE_TARGET);

    bl_info_entry_t flags_entry;
    memset(&flags_entry, 0xFF, (BL_INFO_LEN_FLAGS + 3) & ~0x03UL);

    switch (m_transaction.type)
    {
        case DFU_TYPE_SD:
            flags_entry.flags.sd_intact = false;
            break;
        case DFU_TYPE_APP:
            flags_entry.flags.app_intact = false;
            break;
        case DFU_TYPE_BOOTLOADER:
            flags_entry.flags.bl_intact = false;
            break;
        default: APP_ERROR_CHECK(NRF_ERROR_INVALID_PARAM);
    }

    if (m_transaction.p_bank_addr != m_transaction.p_start_addr)
    {
        /* check whether we're destroying some bank: */
        bl_info_type_t bl_type[] = {
            BL_INFO_TYPE_BANK_SD,
            BL_INFO_TYPE_BANK_BL,
            BL_INFO_TYPE_BANK_APP,
        };

        uint32_t* p_first_bank = (uint32_t*) 0xFFFFFFFF;
        for (uint32_t i = 0; i < ARRAY_SIZE(bl_type); ++i)
        {
            /* if a bank is overlapping, we should erase info pointing to it. */
            const bl_info_entry_t* p_bank = bootloader_info_entry_get(bl_type[i]);
            if (p_bank)
            {
                if (section_overlap(
                    (uint32_t) p_bank->bank.p_bank_addr,
                    p_bank->bank.length,
                    (uint32_t) m_transaction.p_bank_addr,
                    m_transaction.length))
                {
                    __LOG("Invalidate bank type %s (%d)\n", m_dfu_type_strs[(1 << i)], i);
                    APP_ERROR_CHECK(bootloader_info_entry_invalidate((bl_info_type_t) (BL_INFO_TYPE_BANK_BASE + (1 << i))));
                }
                if (p_bank->bank.p_bank_addr < p_first_bank)
                {
                    p_first_bank = p_bank->bank.p_bank_addr;
                }
            }
        }

        /* Look for any section data inside the new bank-section. If found, we
           are about to invalidate the application, and should mark it. Don't
           look through any known banks. */
        for (uint32_t* p_addr = m_transaction.p_bank_addr;
                p_addr < (uint32_t*) ((uint32_t) m_transaction.p_bank_addr + m_transaction.length) &&
                p_addr < p_first_bank;
                p_addr++)
        {
            if (*p_addr != 0xFFFFFFFF)
            {
                if (m_bl_info_pointers.p_segment_sd->start < (uint32_t) p_addr &&
                        m_bl_info_pointers.p_segment_sd->start +
                        m_bl_info_pointers.p_segment_sd->length > (uint32_t) p_addr)
                {
                    flags_entry.flags.sd_intact = false;
                }
                if (m_bl_info_pointers.p_segment_bl->start < (uint32_t) p_addr &&
                        m_bl_info_pointers.p_segment_bl->start +
                        m_bl_info_pointers.p_segment_bl->length > (uint32_t) p_addr)
                {
                    flags_entry.flags.bl_intact = false;
                }
                if (m_bl_info_pointers.p_segment_app->start < (uint32_t) p_addr &&
                        m_bl_info_pointers.p_segment_app->start +
                        m_bl_info_pointers.p_segment_app->length > (uint32_t) p_addr)
                {
                    flags_entry.flags.app_intact = false;
                }
            }
        }
    }

    /* Tag the transfer as incomplete in device page if we're about to overwrite it. */
    if (m_transaction.p_start_addr == m_transaction.p_bank_addr)
    {
        if (m_bl_info_pointers.p_flags == NULL)
        {
            m_bl_info_pointers.p_flags = &bootloader_info_entry_put(BL_INFO_TYPE_FLAGS, &flags_entry, BL_INFO_LEN_FLAGS)->flags;
        }
        else if (bootloader_info_entry_overwrite(BL_INFO_TYPE_FLAGS, &flags_entry) != NRF_SUCCESS)
        {
            start_req(m_transaction.type, &m_transaction.target_fwid_union);
        }
    }

    __LOG("Transferring...\n");
    if (dfu_transfer_start(
                m_transaction.p_start_addr,
                m_transaction.p_bank_addr,
                m_transaction.length,
                m_transaction.segment_is_valid_after_transfer) == NRF_SUCCESS)
    {
        bl_evt_t abort_evt;
        abort_evt.type = BL_EVT_TYPE_TX_ABORT;
        abort_evt.params.tx.abort.tx_slot = TX_SLOT_BEACON;
        (void) bootloader_evt_send(&abort_evt);

        bl_evt_t target_start_evt;
        target_start_evt.type = BL_EVT_TYPE_DFU_START;
        target_start_evt.params.dfu.start.role = DFU_ROLE_TARGET;
        target_start_evt.params.dfu.start.dfu_type = m_transaction.type;
        target_start_evt.params.dfu.start.fwid = m_transaction.target_fwid_union;
        (void) bootloader_evt_send(&target_start_evt);
    }
    else
    {
        start_req(m_transaction.type, &m_transaction.target_fwid_union);
    }
}

static void start_rampdown(void)
{
    bl_evt_t timer_evt;
    timer_evt.type = BL_EVT_TYPE_TIMER_SET;
    timer_evt.params.timer.set.delay_us = STATE_TIMEOUT_RAMPDOWN;
    timer_evt.params.timer.set.index = 0;
    (void) bootloader_evt_send(&timer_evt);

    SET_STATE(DFU_STATE_VALIDATE);
}

static uint32_t notify_state_packet(dfu_packet_t* p_packet)
{
    bl_evt_t fw_evt;
    fw_evt.type = BL_EVT_TYPE_DFU_NEW_FW;
    fw_evt.params.dfu.new_fw.fw_type = (dfu_type_t) p_packet->payload.state.dfu_type;
    fw_evt.params.dfu.new_fw.state = m_state;
    fwid_union_cpy(
            &fw_evt.params.dfu.new_fw.fwid,
            &p_packet->payload.state.fwid,
            (dfu_type_t) p_packet->payload.state.dfu_type);
    return bootloader_evt_send(&fw_evt);
}

/* check whether we've lost any entries, and request them */
static void request_missing_data(uint16_t prev_segment)
{
    uint32_t* p_req_entry = NULL;
    uint32_t req_entry_len = 0;

    if (dfu_transfer_get_oldest_missing_entry(
                m_transaction.p_last_requested_entry,
                &p_req_entry,
                &req_entry_len) &&
            (
             /* don't request the previous packet yet */
             (ADDR_SEGMENT(p_req_entry, m_transaction.p_start_addr) < (uint32_t)(prev_segment - 1)) ||
             (m_transaction.segment_count == prev_segment)
            )
       )
    {
        dfu_packet_t req_packet;
        req_packet.packet_type = DFU_PACKET_TYPE_DATA_REQ;
        req_packet.payload.req_data.segment = ADDR_SEGMENT(p_req_entry, m_transaction.p_start_addr);
        req_packet.payload.req_data.transaction_id = m_transaction.transaction_id;

        /* Use beacon slot */
        bl_evt_t tx_evt;
        tx_evt.type = BL_EVT_TYPE_TX_RADIO;
        tx_evt.params.tx.radio.p_dfu_packet = &req_packet;
        tx_evt.params.tx.radio.length = DFU_PACKET_LEN_DATA_REQ;
        tx_evt.params.tx.radio.interval_type = TX_INTERVAL_TYPE_REQ;
        tx_evt.params.tx.radio.tx_count = TX_REPEATS_REQ;
        tx_evt.params.tx.radio.tx_slot = TX_SLOT_BEACON;
        uint32_t status = bootloader_evt_send(&tx_evt);
        if (status == NRF_SUCCESS)
        {
            m_transaction.p_last_requested_entry = (uint32_t*) p_req_entry;
            m_data_req_segment = req_packet.payload.req_data.segment;
            __LOG("TX REQ FOR 0x%x\n", m_data_req_segment);
        }
    }
}
/*************** Packet handlers ******************/
static uint32_t target_rx_start(dfu_packet_t* p_packet, bool* p_do_relay)
{
    bl_info_segment_t* p_segment = NULL;
    switch (m_transaction.type)
    {
        case DFU_TYPE_APP:
            p_segment = m_bl_info_pointers.p_segment_app;
            break;
        case DFU_TYPE_SD:
            p_segment = m_bl_info_pointers.p_segment_sd;
            break;
        case DFU_TYPE_BOOTLOADER:
            p_segment = m_bl_info_pointers.p_segment_bl;
            break;
        default:
            APP_ERROR_CHECK(NRF_ERROR_NOT_SUPPORTED);
            return NRF_ERROR_NOT_SUPPORTED;
    }

    m_transaction.p_indicated_start_addr = (uint32_t*) p_packet->payload.start.start_address;
    uint32_t start_address = p_packet->payload.start.start_address;
    /* if the host doesn't know the start address, we use start of segment: */
    if (start_address == START_ADDRESS_UNKNOWN)
    {
        start_address = p_segment->start;
    }
    uint32_t segment_count = segment_count_from_start_packet(p_packet);

    m_transaction.segments_remaining                = segment_count;
    m_transaction.segment_count                     = segment_count;
    m_transaction.p_start_addr                      = (uint32_t*) start_address;
    m_transaction.length                            = p_packet->payload.start.length * 4;
    m_transaction.signature_length                  = p_packet->payload.start.signature_length;
    m_transaction.segment_is_valid_after_transfer   = p_packet->payload.start.last;
    m_transaction.p_last_requested_entry            = NULL;
    m_transaction.signature_bitmap                  = 0;

    /* Reset all transfer specific caches. */
    memset(m_req_cache, 0, REQ_CACHE_SIZE * sizeof(m_req_cache[0]));
    packet_cache_flush();

    /* If no bank was specified, we either have to do it single-banked or find a bank */
    if (m_transaction.p_bank_addr == (uint32_t*) 0xFFFFFFFF)
    {
        if (m_transaction.type == DFU_TYPE_BOOTLOADER)
        {
            __LOG("Placing BL bank at end of application segment.\n");
            uint32_t upper_limit =
                (m_bl_info_pointers.p_segment_app->start) +
                (m_bl_info_pointers.p_segment_app->length);
            if (upper_limit > BOOTLOADERADDR())
            {
                upper_limit = BOOTLOADERADDR();
            }
            /* Place bootloader bank at end of app section */
            m_transaction.p_bank_addr = (uint32_t*) (upper_limit -
                    (m_transaction.length & ((uint32_t) ~(PAGE_SIZE - 1))) -
                    (PAGE_SIZE));
        }
        else
        {
            /* others will be inline. */
            __LOG("Bank: 0x%x\n", m_transaction.p_start_addr);
            m_transaction.p_bank_addr = m_transaction.p_start_addr;
        }
    }
    else
    {
        if ((uint32_t) m_transaction.p_bank_addr + m_transaction.length > BOOTLOADERADDR())
        {
            send_end_evt(DFU_END_ERROR_BANK_IN_BOOTLOADER_AREA);
            start_find_fwid();
            /* Return error to not add packet to cache. */
            return NRF_ERROR_INVALID_LENGTH;
        }
    }

    if (m_transaction.p_start_addr != m_transaction.p_bank_addr &&
        section_overlap((uint32_t) m_transaction.p_start_addr, m_transaction.length,
                        (uint32_t) m_transaction.p_bank_addr, m_transaction.length))
    {
        send_end_evt(DFU_END_ERROR_BANK_AND_DESTINATION_OVERLAP);
        start_find_fwid();
        /* Return error to not add packet to cache. */
        return NRF_ERROR_INVALID_LENGTH;
    }

    __LOG("Set transaction parameters:\n");
    __LOG("\ttype:       %s\n", m_dfu_type_strs[(uint32_t) m_transaction.type]);
    __LOG("\tsegments:   %u\n", segment_count);
    __LOG("\tstart addr: 0x%x\n", start_address);
    __LOG("\tbank addr:  0x%x\n", m_transaction.p_bank_addr);
    __LOG("\tlength:     %u\n", m_transaction.length);
    __LOG("\tsigned:     %s\n", m_transaction.signature_length > 0 ? "YES" : "NO");

    if ((uint32_t) m_transaction.p_start_addr >= p_segment->start &&
        (uint32_t) m_transaction.p_start_addr + m_transaction.length <= p_segment->start + p_segment->length)
    {
        start_target();
        *p_do_relay = true;
    }
    else
    {
        __LOG(RTT_CTRL_TEXT_RED "ERROR: Couldn't make transfer fit inside section.\n");
        tid_cache_entry_put(p_packet->payload.start.transaction_id);
        start_req(m_transaction.type, &m_transaction.target_fwid_union);
    }

    return NRF_SUCCESS;
}

static uint32_t target_rx_data(dfu_packet_t* p_packet, uint16_t length, bool* p_do_relay)
{
    uint32_t* p_addr = NULL;
    uint32_t error_code = NRF_ERROR_NULL;

    if (m_data_req_segment == p_packet->payload.data.segment)
    {
        /* Got missing packet, stop requesting. */
        m_data_req_segment = DATA_REQ_SEGMENT_NONE;
        bl_evt_t tx_abort_evt;
        tx_abort_evt.type = BL_EVT_TYPE_TX_ABORT;
        tx_abort_evt.params.tx.abort.tx_slot = TX_SLOT_BEACON;
        (void) bootloader_evt_send(&tx_abort_evt);
    }

    if (p_packet->payload.data.segment <=
        m_transaction.segment_count - m_transaction.signature_length / SEGMENT_LENGTH)
    {
        p_addr = addr_from_seg(p_packet->payload.data.segment, m_transaction.p_start_addr);

        if (length < (DFU_PACKET_LEN_DATA - SEGMENT_LENGTH))
        {
            error_code = NRF_ERROR_INVALID_LENGTH;
        }
        else
        {
            error_code = dfu_transfer_data((uint32_t) p_addr,
                    p_packet->payload.data.data,
                    length - (DFU_PACKET_LEN_DATA - SEGMENT_LENGTH));
        }
    }
    else /* treat signature packets at the end */
    {
        uint32_t index = p_packet->payload.data.segment -
            (m_transaction.segment_count - m_transaction.signature_length / SEGMENT_LENGTH) - 1;
        if (index >= m_transaction.signature_length / SEGMENT_LENGTH ||
                m_transaction.signature_bitmap & (1 << index))
        {
            error_code = NRF_ERROR_INVALID_STATE;
        }
        else
        {
            if (length < (DFU_PACKET_LEN_DATA - SEGMENT_LENGTH) ||
                length > DFU_PACKET_LEN_DATA)
            {
                error_code = NRF_ERROR_INVALID_LENGTH;
            }
            else
            {
                memcpy(&m_transaction.signature[index * SEGMENT_LENGTH],
                        p_packet->payload.data.data,
                        length - (DFU_PACKET_LEN_DATA - SEGMENT_LENGTH));

                __LOG("Signature packet #%u\n", index);
                m_transaction.signature_bitmap |= (1 << index);
                error_code = NRF_SUCCESS;
            }
        }
    }

    if (error_code != NRF_SUCCESS)
    {
        return error_code;
    }
    send_progress_event(p_packet->payload.data.segment, m_transaction.segment_count);
    m_transaction.segments_remaining--;
    *p_do_relay = true;
    if (m_data_req_segment == DATA_REQ_SEGMENT_NONE)
    {
        request_missing_data(p_packet->payload.data.segment);
    }
    return error_code;
}

static uint32_t handle_data_packet(dfu_packet_t* p_packet, uint16_t length)
{
    if (p_packet->payload.data.transaction_id != m_transaction.transaction_id)
    {
        return NRF_ERROR_INVALID_DATA;
    }
    if (packet_in_cache(p_packet))
    {
        return NRF_SUCCESS;
    }

    uint32_t status;
    bool do_relay = false;

    switch (m_state)
    {
        case DFU_STATE_READY:
            if (p_packet->payload.start.segment == 0)
            {
                m_lost_start_edge = 0;
                status = target_rx_start(p_packet, &do_relay);
            }
            else
            {
                m_lost_start_edge++;
                if (m_lost_start_edge < LOST_START_EDGE)  // reach some edge to be sure that we really missed start
                {
                    return NRF_SUCCESS;
                }

                __LOG("ERROR: Received non-0 segment.\n");
                m_lost_start_edge = 0;
                tid_cache_entry_put(m_transaction.transaction_id);
                start_req(m_transaction.type, &m_transaction.target_fwid_union); /* go back to req, we've missed packet 0 */
                status = NRF_ERROR_INVALID_STATE;
            }
            break;

        case DFU_STATE_TARGET:
            if (p_packet->payload.data.segment > 0 &&
                p_packet->payload.data.segment <= m_transaction.segment_count)
            {
                (void) target_rx_data(p_packet, length, &do_relay);
                status = NRF_SUCCESS;
            }
            else
            {
                status = NRF_ERROR_INVALID_DATA;
            }

            /* ending the DFU */
            if (m_transaction.segments_remaining == 0)
            {
                start_rampdown();
            }
            break;

        case DFU_STATE_RELAY_CANDIDATE:
        {
            if (p_packet->payload.data.segment == 0)
            {
                m_transaction.segment_count = segment_count_from_start_packet(p_packet);
            }
            SET_STATE(DFU_STATE_RELAY);
            (void) tx_abort(TX_SLOT_BEACON);

            bl_evt_t relay_evt;
            relay_evt.type = BL_EVT_TYPE_DFU_START;
            relay_evt.params.dfu.start.role = DFU_ROLE_RELAY;
            relay_evt.params.dfu.start.fwid = m_transaction.target_fwid_union;
            relay_evt.params.dfu.start.dfu_type = m_transaction.type;
            status = bootloader_evt_send(&relay_evt);

            do_relay = true;
            break;
        }
        case DFU_STATE_RELAY:
            if (p_packet->payload.data.segment == 0)
            {
                m_transaction.segment_count = segment_count_from_start_packet(p_packet);
            }
            send_progress_event(p_packet->payload.data.segment, m_transaction.segment_count);
            do_relay = true;
            status = NRF_SUCCESS;
            break;
        default:
            status = NRF_ERROR_INVALID_STATE;
            break;
    }

    bool is_cached = false;
    if (do_relay)
    {
        is_cached = NRF_SUCCESS == relay_packet(p_packet, length);
    }

    if (status == NRF_SUCCESS && !is_cached)
    {
        packet_cache_put(p_packet);
    }

    return status;
}

static uint32_t handle_state_packet(dfu_packet_t* p_packet)
{
    uint32_t status;

    switch (m_state)
    {
        case DFU_STATE_FIND_FWID:
            status = notify_state_packet(p_packet);
            break;
        case DFU_STATE_DFU_REQ:
        case DFU_STATE_READY:
            if (ready_packet_matches_our_req(p_packet, m_transaction.type, &m_transaction.target_fwid_union))
            {
                if (p_packet->payload.state.authority > m_transaction.authority ||
                        (
                         p_packet->payload.state.authority == m_transaction.authority &&
                         p_packet->payload.state.transaction_id > m_transaction.transaction_id
                        )
                   )
                {
                    __LOG("\t->Upgradable\n");
                    /* assume that the other device knows what to upgrade */
                    m_transaction.type = (dfu_type_t) p_packet->payload.state.dfu_type;
                    m_transaction.target_fwid_union = p_packet->payload.state.fwid;

                    start_ready(p_packet);
                    status = NRF_SUCCESS;
                }
                else
                {
                    /* Source is worse than the current, ignore */
                    status = NRF_SUCCESS;
                }
            }
            /* Notify about other transfer if the transfer is different from the current, and
			   it's a new transfer, not a request */
            else if ((m_transaction.type != p_packet->payload.state.dfu_type ||
                    !fwid_union_id_cmp(
                        &p_packet->payload.state.fwid,
                        &m_transaction.target_fwid_union,
                        m_transaction.type)) && p_packet->payload.state.authority != 0)
            {
                status = notify_state_packet(p_packet);
            }
            else
            {
                status = NRF_SUCCESS;
            }
            break;
        case DFU_STATE_RELAY_CANDIDATE:
            if (m_transaction.type == p_packet->payload.state.dfu_type &&
                    fwid_union_cmp(&m_transaction.target_fwid_union, &p_packet->payload.state.fwid, m_transaction.type))
            {
                if (m_transaction.authority < p_packet->payload.state.authority ||
                        (m_transaction.authority      == p_packet->payload.state.authority &&
                         m_transaction.transaction_id <  p_packet->payload.state.transaction_id))
                {
                    __LOG("\tHigher authority (%u)\n", p_packet->payload.state.authority);
                    m_transaction.transaction_id = p_packet->payload.state.transaction_id;
                    m_transaction.authority      = p_packet->payload.state.authority;
                    beacon_set(state_beacon_type(m_transaction.type));
                }
            }
            status = NRF_SUCCESS;
            break;
        default:
            status = NRF_ERROR_INVALID_STATE;
    }
    return status;
}

static uint32_t handle_fwid_packet(dfu_packet_t* p_packet)
{
    uint32_t status;

    if (m_state == DFU_STATE_FIND_FWID)
    {
        /* always upgrade bootloader first */
        if (bootloader_is_newer(p_packet->payload.fwid.bootloader))
        {
            __LOG("\tBootloader upgrade possible\n");
            bl_evt_t fwid_evt;
            fwid_evt.type = BL_EVT_TYPE_DFU_NEW_FW;
            fwid_evt.params.dfu.new_fw.fw_type = DFU_TYPE_BOOTLOADER;
            fwid_evt.params.dfu.new_fw.fwid.bootloader.id  = p_packet->payload.fwid.bootloader.id;
            fwid_evt.params.dfu.new_fw.fwid.bootloader.ver = p_packet->payload.fwid.bootloader.ver;
            fwid_evt.params.dfu.new_fw.state = m_state;
            status = bootloader_evt_send(&fwid_evt);
        }
        else if (app_is_newer(&p_packet->payload.fwid.app))
        {
            /* SD shall only be upgraded if a newer version of our app requires a different SD */
            if (p_packet->payload.fwid.sd != 0xFFFE && p_packet->payload.fwid.sd != m_bl_info_pointers.p_fwid->sd)
            {
                __LOG("\tSD upgrade possible\n");
                bl_evt_t fwid_evt;
                fwid_evt.type = BL_EVT_TYPE_DFU_NEW_FW;
                fwid_evt.params.dfu.new_fw.fw_type = DFU_TYPE_SD;
                fwid_evt.params.dfu.new_fw.fwid.sd = p_packet->payload.fwid.sd;
                fwid_evt.params.dfu.new_fw.state = m_state;
                status = bootloader_evt_send(&fwid_evt);
            }
            else
            {
                __LOG("\tApp upgrade possible\n");
                bl_evt_t fwid_evt;
                fwid_evt.type = BL_EVT_TYPE_DFU_NEW_FW;
                fwid_evt.params.dfu.new_fw.fw_type = DFU_TYPE_APP;
                fwid_evt.params.dfu.new_fw.state = m_state;
                memcpy(&fwid_evt.params.dfu.new_fw.fwid.app, &p_packet->payload.fwid.app, sizeof(app_id_t));
                status = bootloader_evt_send(&fwid_evt);
            }
        }
        else
        {
            status = NRF_ERROR_INVALID_DATA;
        }
    }
    else
    {
        status = NRF_ERROR_INVALID_STATE;
    }
    return status;
}

static uint32_t handle_data_req_packet(dfu_packet_t* p_packet)
{
    uint32_t status;
    if (p_packet->payload.data.transaction_id == m_transaction.transaction_id)
    {
        __LOG("RX data REQ #%u\n", p_packet->payload.data.segment);
        if (m_state == DFU_STATE_RELAY)
        {
            /* only relay new packets, look for it in cache */
            if (packet_in_cache(p_packet))
            {
                status = NRF_SUCCESS;
            }
            else
            {
                status = relay_packet(p_packet, DFU_PACKET_LEN_DATA_REQ);

#if RBC_MESH_SERIAL
                bl_evt_t tx_evt;
                tx_evt.type = BL_EVT_TYPE_TX_SERIAL;
                tx_evt.params.tx.serial.p_dfu_packet = p_packet;
                tx_evt.params.tx.serial.length = DFU_PACKET_LEN_DATA_REQ;
                (void) bootloader_evt_send(&tx_evt);
#endif
            }
        }
        else /* In transfer */
        {
            req_cache_entry_t* p_req_entry = NULL;
            /* check that we haven't served this request recently. */
            for (uint32_t i = 0; i < REQ_CACHE_SIZE; ++i)
            {
                if (m_req_cache[i].segment == p_packet->payload.req_data.segment)
                {
                    if (m_req_cache[i].rx_count++ < REQ_RX_COUNT_RETRY)
                    {
                        return NRF_SUCCESS;
                    }
                    p_req_entry = &m_req_cache[i];
                    break;
                }
            }
            /* serve request */
            dfu_packet_t dfu_rsp;
            if (
                dfu_transfer_has_entry(
                    (uint32_t*) SEGMENT_ADDR(p_packet->payload.req_data.segment, m_transaction.p_start_addr),
                    dfu_rsp.payload.rsp_data.data, SEGMENT_LENGTH)
               )
            {
                dfu_rsp.packet_type = DFU_PACKET_TYPE_DATA_RSP;
                dfu_rsp.payload.rsp_data.segment = p_packet->payload.req_data.segment;
                dfu_rsp.payload.rsp_data.transaction_id = p_packet->payload.req_data.transaction_id;

                status = packet_tx_dynamic(&dfu_rsp, DFU_PACKET_LEN_DATA_RSP, TX_INTERVAL_TYPE_RSP, TX_REPEATS_RSP);
            }
            else
            {
                status = NRF_ERROR_NOT_FOUND;
            }

            /* log our attempt at responding */
            if ((status == NRF_SUCCESS) && (p_req_entry == NULL))
            {
                p_req_entry = &m_req_cache[(m_req_index++) & (REQ_CACHE_SIZE - 1)];
                p_req_entry->segment = p_packet->payload.req_data.segment;
            }

            /* Check that p_req_enry is not NULL, if status is not NRF_SUCCESS. If status is 
               NRF_SUCCESS, p_req_entry will always be set. */
            if (p_req_entry != NULL)
            {
                p_req_entry->rx_count = 0;
            }
        }
    }
    else
    {
        status = NRF_ERROR_INVALID_DATA;
    }
    return status;
}

static uint32_t handle_data_rsp_packet(dfu_packet_t* p_packet, uint16_t length)
{
    uint32_t status;
    if (m_state == DFU_STATE_RELAY)
    {
        /* only relay new packets, look for it in cache */
        if (packet_in_cache(p_packet))
        {
            status = NRF_SUCCESS;
        }
        else
        {
            send_progress_event(m_transaction.segment_count - m_transaction.segments_remaining + 1,
                    m_transaction.segment_count);
            status = relay_packet(p_packet, length);
        }
    }
    else
    {
        status = handle_data_packet(p_packet, length);
    }
    return status;
}

/*****************************************************************************
* Interface Functions
*****************************************************************************/

void dfu_mesh_init(uint8_t tx_slots)
{
    SET_STATE(DFU_STATE_INITIALIZED);
    memset(&m_transaction, 0, sizeof(transaction_t));
    memset(m_req_cache, 0, REQ_CACHE_SIZE * sizeof(m_req_cache[0]));
    m_req_index = 0;
    m_tx_slots = tx_slots;

    get_info_pointers();
}

void dfu_mesh_start(void)
{
    memset(&m_transaction, 0, sizeof(transaction_t));
    memset(m_req_cache, 0, sizeof(req_cache_entry_t) * REQ_CACHE_SIZE);
    m_req_index = 0;
    m_data_req_segment = false;
    get_info_pointers();
    send_bank_notifications();
    packet_cache_flush();

    if (!dfu_bank_transfer_in_progress())
    {
        __LOG("DFU mesh start: \n");
        __LOG("\tAPP: %08x.%04x:%08x\n",
                (uint32_t) m_bl_info_pointers.p_fwid->app.company_id,
                (uint32_t) m_bl_info_pointers.p_fwid->app.app_id,
                (uint32_t) m_bl_info_pointers.p_fwid->app.app_version);
        __LOG("\tSD:  %04x\n",
                (uint32_t) m_bl_info_pointers.p_fwid->sd);
        __LOG("\tBL:  %02x:%02x\n",
                (uint32_t) m_bl_info_pointers.p_fwid->bootloader.id,
                (uint32_t) m_bl_info_pointers.p_fwid->bootloader.ver);

        start_find_fwid();
    }
    else
    {
        __LOG("Bank Transfer in progress!\n");
        SET_STATE(DFU_STATE_INITIALIZED);
    }
}

uint32_t dfu_mesh_req(dfu_type_t type, fwid_union_t* p_fwid, uint32_t* p_bank_addr)
{
    if (m_state != DFU_STATE_FIND_FWID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if ((uint32_t) p_bank_addr >= BOOTLOADERADDR() &&
        (uint32_t) p_bank_addr != 0xFFFFFFFF)
    {
        __LOG("Attempting to overwrite bootloader\n");
        return NRF_ERROR_INVALID_ADDR;
    }

    __LOG("REQUESTING TRANSFER OF %s\n", m_dfu_type_strs[(uint32_t) type]);
    m_transaction.p_bank_addr = p_bank_addr;
    start_req(type, p_fwid);

    return NRF_SUCCESS;
}

uint32_t dfu_mesh_relay(dfu_type_t type, fwid_union_t* p_fwid, uint32_t transaction_id)
{
    if (m_state != DFU_STATE_FIND_FWID &&
        m_state != DFU_STATE_DFU_REQ)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    __LOG("Relay transfer of %s (TID=0x%x)\n", m_dfu_type_strs[type], transaction_id);

    SET_STATE(DFU_STATE_RELAY_CANDIDATE);
    m_transaction.type = type;
    m_transaction.transaction_id = transaction_id;
    fwid_union_cpy(
            &m_transaction.target_fwid_union,
            p_fwid,
            type);
    beacon_set(state_beacon_type(m_transaction.type));
    return NRF_SUCCESS;
}

uint32_t dfu_mesh_rx(dfu_packet_t* p_packet, uint16_t length, bool from_serial)
{
#ifdef DEBUG_LEDS
    static bool led = false;
    if (led)
    {
        NRF_GPIO->OUTCLR = LED_2;
    }
    else
    {
        NRF_GPIO->OUTSET = LED_2;
    }
    led = !led;
#endif
    uint32_t status;

    switch (p_packet->packet_type)
    {
        case DFU_PACKET_TYPE_FWID:
            status = handle_fwid_packet(p_packet);
            break;

        case DFU_PACKET_TYPE_STATE:
            status = handle_state_packet(p_packet);
            break;

        case DFU_PACKET_TYPE_DATA:
            status = handle_data_packet(p_packet, length);
            break;

        case DFU_PACKET_TYPE_DATA_REQ:
            status = handle_data_req_packet(p_packet);
            break;

        case DFU_PACKET_TYPE_DATA_RSP:
            status = handle_data_rsp_packet(p_packet, length);
            break;

        default:
            /* don't care */
            status = NRF_ERROR_INVALID_DATA;
            break;
    }
    return status;
}

void dfu_mesh_timeout(void)
{
    __LOG("TIMEOUT\n");
    switch (m_state)
    {
        case DFU_STATE_FIND_FWID:
            send_end_evt(DFU_END_FWID_VALID);
            break;

        case DFU_STATE_DFU_REQ:
        case DFU_STATE_READY:
            send_end_evt(DFU_END_ERROR_NO_START);
            break;

        case DFU_STATE_TARGET:
            start_req(m_transaction.type, &m_transaction.target_fwid_union);
            break;

        case DFU_STATE_VALIDATE:
            if (signature_check())
            {
                (void) dfu_mesh_finalize();
            }
            else
            {
                /* someone gave us unauthorized firmware */
                send_end_evt(DFU_END_ERROR_UNAUTHORIZED);
            }
            break;
        case DFU_STATE_RELAY:
        case DFU_STATE_RELAY_CANDIDATE:
            send_end_evt(DFU_END_SUCCESS);
            break;
        default:
            break;
    }
}

dfu_type_t dfu_mesh_missing_type_get(void)
{
    bl_info_entry_t* p_fwid_entry = bootloader_info_entry_get(BL_INFO_TYPE_VERSION);
    bl_info_entry_t* p_flag_entry = bootloader_info_entry_get(BL_INFO_TYPE_FLAGS);
    bl_info_entry_t* p_sd_seg_entry = bootloader_info_entry_get(BL_INFO_TYPE_SEGMENT_SD);
    bl_info_entry_t* p_app_seg_entry = bootloader_info_entry_get(BL_INFO_TYPE_SEGMENT_APP);
    if ((p_fwid_entry != NULL) && (p_fwid_entry->version.sd != SD_VERSION_INVALID)) /* only check for SD if we need it. */
    {
        if ( ((p_flag_entry != NULL) && (!p_flag_entry->flags.sd_intact)) || 
             ((p_sd_seg_entry != NULL) && (*((uint32_t*) p_sd_seg_entry->segment.start) == 0xFFFFFFFF)) )
        {
            return DFU_TYPE_SD;
        }
    }

    if ( ((p_flag_entry != NULL) && (!p_flag_entry->flags.app_intact)) || 
         ((p_app_seg_entry != NULL) && (*((uint32_t*) p_app_seg_entry->segment.start) == 0xFFFFFFFF)) )
    {
        return DFU_TYPE_APP;
    }

    return DFU_TYPE_NONE;
}

bool dfu_mesh_app_is_valid(void)
{
    bl_info_entry_t* p_fwid_entry    = bootloader_info_entry_get(BL_INFO_TYPE_VERSION);
    bl_info_entry_t* p_app_seg_entry = bootloader_info_entry_get(BL_INFO_TYPE_SEGMENT_APP);

    return (p_fwid_entry != NULL && p_app_seg_entry != NULL &&
            (uint32_t*) p_app_seg_entry->segment.start != (uint32_t*) 0xFFFFFFFF &&
            *((uint32_t*) p_app_seg_entry->segment.start) != 0xFFFFFFFF &&
            p_fwid_entry->version.app.app_version != APP_VERSION_INVALID &&
            fw_is_verified());
}

uint32_t dfu_mesh_finalize(void)
{
    __LOG("Finalize\n");
    /* write new version in bl info: */
    bl_info_entry_t new_version_entry;
    memcpy(&new_version_entry.version, m_bl_info_pointers.p_fwid, sizeof(fwid_t));
    bl_info_type_t sign_info_type = BL_INFO_TYPE_INVALID;

    if (m_transaction.p_bank_addr != m_transaction.p_start_addr) /* Dual bank! */
    {
        bl_info_entry_t bank_entry;
        memset(&bank_entry, 0xFF, sizeof(bl_info_bank_t));
        fwid_union_cpy(&bank_entry.bank.fwid,
                       &m_transaction.target_fwid_union,
                       m_transaction.type);
        bank_entry.bank.length = m_transaction.length;
        bank_entry.bank.p_bank_addr = m_transaction.p_bank_addr;
        bank_entry.bank.state = BL_INFO_BANK_STATE_IDLE;
        bank_entry.bank.has_signature = false;

        uint32_t entry_length = BL_INFO_LEN_BANK;

        if (m_transaction.signature_length > 0)
        {
            memcpy(bank_entry.bank.signature, m_transaction.signature, BL_INFO_LEN_SIGNATURE);
            bank_entry.bank.has_signature = true;
            entry_length = BL_INFO_LEN_BANK_SIGNED;
        }

        switch (m_transaction.type)
        {
            case DFU_TYPE_APP:
                (void) bootloader_info_entry_put(BL_INFO_TYPE_BANK_APP, &bank_entry, entry_length);
                break;
            case DFU_TYPE_BOOTLOADER:
                (void) bootloader_info_entry_put(BL_INFO_TYPE_BANK_BL, &bank_entry, entry_length);
                break;
            case DFU_TYPE_SD:
                (void) bootloader_info_entry_put(BL_INFO_TYPE_BANK_SD, &bank_entry, entry_length);
                break;
            default:
                return NRF_ERROR_INVALID_DATA;
        }
        __LOG("Bank info stored.\n");
    }
    else
    {
        if (m_bl_info_pointers.p_flags == NULL)
        {
            APP_ERROR_CHECK(NRF_ERROR_NULL);
        }

        /* copy flags, then mark the type we just verified as intact before reflashing it. */
        bl_info_entry_t flags_entry;
        memcpy(&flags_entry, m_bl_info_pointers.p_flags, ((BL_INFO_LEN_FLAGS + 3) & ~0x03UL));

        switch (m_transaction.type)
        {
            case DFU_TYPE_APP:
                memcpy((void*) &new_version_entry.version.app,
                       (void*) &m_transaction.target_fwid_union.app,
                       DFU_FWID_LEN_APP);
                sign_info_type = BL_INFO_TYPE_SIGNATURE_APP;
                flags_entry.flags.app_intact = true;
                break;
            case DFU_TYPE_SD:
                memcpy((void*) &new_version_entry.version.sd,
                       (void*) &m_transaction.target_fwid_union.sd,
                       DFU_FWID_LEN_SD);
                sign_info_type = BL_INFO_TYPE_SIGNATURE_SD;
                flags_entry.flags.sd_intact = true;
                break;
            case DFU_TYPE_BOOTLOADER:
                memcpy((void*) &new_version_entry.version.bootloader,
                       (void*) &m_transaction.target_fwid_union.bootloader,
                       DFU_FWID_LEN_BL);
                sign_info_type = BL_INFO_TYPE_SIGNATURE_BL;
                flags_entry.flags.bl_intact = true;
                break;
            default:
                break;
        }
        /* add signature to bl info, if applicable: */
        if (m_transaction.signature_length != 0)
        {
            (void) bootloader_info_entry_put(sign_info_type, (bl_info_entry_t*) m_transaction.signature, DFU_SIGNATURE_LEN);
        }

        __LOG("VERSION: ");
        for (uint32_t i = 0; i < BL_INFO_LEN_FWID; ++i)
        {
            __LOG("%02x", ((uint8_t*) &new_version_entry)[i]);
        }
        __LOG("\n");
        m_bl_info_pointers.p_fwid  = &bootloader_info_entry_put(BL_INFO_TYPE_VERSION,
                &new_version_entry, BL_INFO_LEN_FWID)->version;
        m_bl_info_pointers.p_flags = &bootloader_info_entry_put(BL_INFO_TYPE_FLAGS,
                &flags_entry, BL_INFO_LEN_FLAGS)->flags;
        __LOG("Single bank meta stored.\n");
    }

    __LOG(RTT_CTRL_TEXT_GREEN RTT_CTRL_BG_RED "Transfer complete!" RTT_CTRL_RESET "\n");
    /* Reset the bootloader info page to make room for the next transfer. */
    SET_STATE(DFU_STATE_STABILIZE);
    APP_ERROR_CHECK(bootloader_info_reset());
    return NRF_SUCCESS;
}

void dfu_mesh_restart(void)
{
    __LOG("Restart\n");
    start_find_fwid();
}

void dfu_mesh_on_flash_idle(void)
{
    /* check whether the blinfo module is done with all its stuff. */
    if (m_state == DFU_STATE_STABILIZE)
    {
        __LOG("DFU MESH: IDLE\n");
        if (bootloader_info_stable())
        {
            bl_evt_t end_evt;
            end_evt.type = BL_EVT_TYPE_DFU_END;
            end_evt.params.dfu.end.role = DFU_ROLE_TARGET;
            end_evt.params.dfu.end.dfu_type = m_transaction.type;
            fwid_union_cpy(&end_evt.params.dfu.end.fwid, &m_transaction.target_fwid_union, m_transaction.type);
            (void) bootloader_evt_send(&end_evt);
            dfu_transfer_end();
            get_info_pointers();
            dfu_mesh_start();
        }
    }
}
