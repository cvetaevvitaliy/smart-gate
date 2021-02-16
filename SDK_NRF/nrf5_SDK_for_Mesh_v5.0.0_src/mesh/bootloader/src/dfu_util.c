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

#include <stdbool.h>
#include <string.h>
#include "dfu_util.h"
#include "dfu_types_mesh.h"

#ifdef BOOTLOADER
#include "bootloader_info.h"
#endif

/*****************************************************************************
* Local defines
*****************************************************************************/
#define TRANSACTION_ID_CACHE_SIZE   (8)
#define PACKET_CACHE_SIZE           (16)


#define IRQ_ENABLED                 (0x01)  /**< Field that identifies if an interrupt is enabled. */
#define MAX_NUMBER_INTERRUPTS       (32)    /**< Maximum number of interrupts available. */

/*****************************************************************************
* Local typedefs
*****************************************************************************/

typedef struct
{
    dfu_packet_type_t   type;
    uint16_t            segment;
} packet_cache_entry_t;


/*****************************************************************************
* Static globals
*****************************************************************************/
static uint32_t                 m_tid_cache[TRANSACTION_ID_CACHE_SIZE];
static uint8_t                  m_tid_index;
static packet_cache_entry_t     m_packet_cache[PACKET_CACHE_SIZE];
static uint32_t                 m_packet_cache_index;

/*****************************************************************************
* Static functions
*****************************************************************************/

/*****************************************************************************
* Interface functions
*****************************************************************************/
void fwid_union_cpy(fwid_union_t* p_dst, fwid_union_t* p_src, dfu_type_t dfu_type)
{
    switch (dfu_type)
    {
        case DFU_TYPE_APP:
            memcpy(&p_dst->app, &p_src->app, sizeof(app_id_t));
            break;
        case DFU_TYPE_SD:
            p_dst->sd = p_src->sd;
            break;
        case DFU_TYPE_BOOTLOADER:
            memcpy(&p_dst->bootloader, &p_src->bootloader, sizeof(bl_id_t));
            break;
        default: break;
    }
}

bool fwid_union_cmp(fwid_union_t* p_a, fwid_union_t* p_b, dfu_type_t dfu_type)
{
    switch (dfu_type)
    {
        case DFU_TYPE_APP:
            return memcmp(&p_a->app,
                          &p_b->app,
                          sizeof(app_id_t)) == 0;
        case DFU_TYPE_SD:
            return (p_a->sd == p_b->sd);
        case DFU_TYPE_BOOTLOADER:
            return memcmp(&p_a->bootloader,
                          &p_b->bootloader,
                          sizeof(bl_id_t)) == 0;
        default: return false;
    }
}

bool fwid_union_id_cmp(fwid_union_t* p_a, fwid_union_t* p_b, dfu_type_t dfu_type)
{
    switch (dfu_type)
    {
        case DFU_TYPE_APP:
            return (p_a->app.app_id == p_b->app.app_id &&
                    p_a->app.company_id == p_b->app.company_id);
        case DFU_TYPE_SD:
            return (p_a->sd == p_b->sd);
        case DFU_TYPE_BOOTLOADER:
            return (p_a->bootloader.id == p_b->bootloader.id);
        default: return false;
    }
}

int fwid_union_version_cmp(fwid_union_t* p_a, fwid_union_t* p_b, dfu_type_t dfu_type)
{
    switch (dfu_type)
    {
        case DFU_TYPE_APP:
            return (p_a->app.app_version - p_b->app.app_version);
        case DFU_TYPE_SD:
            return (p_a->sd - p_b->sd);
        case DFU_TYPE_BOOTLOADER:
            return (p_a->bootloader.id - p_b->bootloader.id);
        default: return 0;
    }
}

bool ready_packet_matches_our_req(dfu_packet_t* p_packet, dfu_type_t dfu_type_req, fwid_union_t* p_fwid_req)
{
    if (dfu_type_req != p_packet->payload.state.dfu_type &&
        dfu_type_req != DFU_TYPE_NONE)
    {
        return false;
    }

    if (tid_cache_has_entry(p_packet->payload.state.transaction_id))
    {
        return false;
    }
    return (fwid_union_id_cmp(&p_packet->payload.state.fwid,
                p_fwid_req,
                dfu_type_req)
            &&
            (fwid_union_version_cmp(&p_packet->payload.state.fwid,
                p_fwid_req,
                dfu_type_req) >= 0));
}

uint32_t* addr_from_seg(uint16_t segment, uint32_t* p_start_addr)
{
    if (segment == 1)
    {
        return p_start_addr;
    }
    else
    {
        return (uint32_t*) (((segment - 1) << 4) + ((uint32_t) p_start_addr & 0xFFFFFFF0));
    }
}

#ifdef BOOTLOADER
bool is_upgrade(fwid_union_t* p_fwid, dfu_type_t dfu_type)
{
    /* Consider any version of the firmware (with the same ID) an upgrade if ours is broken */
    bl_info_entry_t* p_fwid_entry = bootloader_info_entry_get(BL_INFO_TYPE_VERSION);
    bl_info_entry_t* p_flag_entry = bootloader_info_entry_get(BL_INFO_TYPE_FLAGS);
    switch (dfu_type)
    {
        case DFU_TYPE_APP:
            return ( p_fwid_entry != NULL &&
                     p_fwid->app.app_id     == p_fwid_entry->version.app.app_id &&
                     p_fwid->app.company_id == p_fwid_entry->version.app.company_id &&
                     (  p_fwid->app.app_version > p_fwid_entry->version.app.app_version || 
                        (p_flag_entry && !p_flag_entry->flags.app_intact)
                     ) );

        case DFU_TYPE_BOOTLOADER:
            return ( p_fwid_entry != NULL &&
                     p_fwid->bootloader.id == p_fwid_entry->version.bootloader.id &&
                     (  p_fwid->bootloader.ver > p_fwid_entry->version.bootloader.ver || 
                        (p_flag_entry && !p_flag_entry->flags.bl_intact)
                     ) );

        case DFU_TYPE_SD:
            /* Only accept the Softdevice if ours is broken. Otherwise, we would be DFUing the same
               SD over and over. */
            return ( p_fwid_entry != NULL &&
                     p_fwid->sd == p_fwid_entry->version.sd && 
                     (p_flag_entry && !p_flag_entry->flags.sd_intact) );
        default:
            return false;
    }
}

bool app_is_newer(app_id_t* p_app_id)
{
    bl_info_entry_t* p_fwid_entry = bootloader_info_entry_get(BL_INFO_TYPE_VERSION);
    if (p_fwid_entry != NULL)
    {
        return (p_app_id->app_id     == p_fwid_entry->version.app.app_id &&
                p_app_id->company_id == p_fwid_entry->version.app.company_id &&
                p_app_id->app_version > p_fwid_entry->version.app.app_version);
    }

    return true;
}

bool bootloader_is_newer(bl_id_t bl_id)
{
    bl_info_entry_t* p_fwid_entry = bootloader_info_entry_get(BL_INFO_TYPE_VERSION);
    if (p_fwid_entry != NULL)
    {
        return (bl_id.id == p_fwid_entry->version.bootloader.id &&
                bl_id.ver > p_fwid_entry->version.bootloader.ver);
    }

    return true;
}

bool fw_is_verified(void)
{
    bl_info_entry_t* p_flag_entry = bootloader_info_entry_get(BL_INFO_TYPE_FLAGS);
    if (p_flag_entry)
    {
        return (p_flag_entry->flags.sd_intact &&
                p_flag_entry->flags.app_intact &&
                p_flag_entry->flags.bl_intact);
    }

    return true;
}
#endif

void tid_cache_entry_put(uint32_t tid)
{
    m_tid_cache[(m_tid_index++) & (TRANSACTION_ID_CACHE_SIZE - 1)] = tid;
}

bool tid_cache_has_entry(uint32_t tid)
{
    for (uint32_t i = 0; i < TRANSACTION_ID_CACHE_SIZE; ++i)
    {
        if (m_tid_cache[i] == tid)
        {
            return true;
        }
    }
    return false;
}

bool packet_in_cache(dfu_packet_t* p_packet)
{
    for (uint32_t i = 0; i < PACKET_CACHE_SIZE; ++i)
    {
        if (m_packet_cache[i].type    == p_packet->packet_type &&
            m_packet_cache[i].segment == p_packet->payload.data.segment)
        {
            return true;
        }
    }
    return false;
}

void packet_cache_put(dfu_packet_t* p_packet)
{
    m_packet_cache[(m_packet_cache_index) & (PACKET_CACHE_SIZE - 1)].type = (dfu_packet_type_t) p_packet->packet_type;
    m_packet_cache[(m_packet_cache_index) & (PACKET_CACHE_SIZE - 1)].segment = p_packet->payload.data.segment;
    m_packet_cache_index++;
}

void packet_cache_flush(void)
{
    memset(m_packet_cache, 0, sizeof(m_packet_cache));
    m_packet_cache_index = 0;
}

bool section_overlap(uint32_t section_a_start, uint32_t section_a_length,
                     uint32_t section_b_start, uint32_t section_b_length)
{
    return (
        (
         section_a_start >= section_b_start &&
         section_a_start <  section_b_start + section_b_length
        )
        ||
        (
         section_b_start >= section_a_start &&
         section_b_start <= section_a_start + section_a_length
        )
     );
 }

void interrupts_disable(void)
{
    uint32_t interrupt_setting_mask;
    uint32_t irq;

    /* Fetch the current interrupt settings. */
    interrupt_setting_mask = NVIC->ISER[0];

    /* Loop from interrupt 0 for disabling of all interrupts. */
    for (irq = 0; irq < MAX_NUMBER_INTERRUPTS; irq++)
    {
        if (interrupt_setting_mask & (IRQ_ENABLED << irq))
        {
            /* The interrupt was enabled, hence disable it. */
            NVIC_DisableIRQ((IRQn_Type)irq);
        }
    }
}
