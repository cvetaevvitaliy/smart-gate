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
#ifndef DFU_APP_H__
#define DFU_APP_H__

#include <stdint.h>
#include "dfu_packet.h"
#include "dfu_types_mesh.h"
#include "bl_if.h"


/**
 * Initialize the dfu functionality. Called by the framework as part of mesh
 * initialization.
 *
 * @return NRF_SUCCESS The bootloader was successfully initialized.
 * @return NRF_ERROR_NOT_AVAILABLE There's no DFU service available on the chip.
 */
uint32_t dfu_init(void);

/**
* Manually trigger the bootloader. The device will be reset immediately, and
* may not be available for regular operation for several minutes. If
* successful, this function does not return.
*
* @return NRF_ERROR_BUSY The application rejected the bootloader start request.
* @return NRF_ERROR_FORBIDDEN The NRF_UICR->BOOTLOADERADDR persistent register
*   has not been set, and the bootloader could not start.
*/
uint32_t dfu_jump_to_bootloader(void);

/**
* Pass a dfu packet to the dfu module.
*
* @param[in] p_packet A pointer to a DFU packet.
* @param[in] length The length of the DFU packet.
*
* @return NRF_SUCCESS The packet was successfully handled by the DFU module.
* @return NRF_ERROR_BUSY The dfu module can't accept the request at the moment.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
*/
uint32_t dfu_rx(dfu_packet_t* p_packet, uint32_t length);

/**
* Request a DFU transfer.
*
* The DFU transfer will run alongside the application, and store the firmware
* in the given bank.
*
* Generates events:
*   @RBC_MESH_EVT_DFU_BANK_AVAILABLE: The DFU transfer is finished, and is
*   available for flashing.
*   @RBC_MESH_EVT_DFU_START: The dfu module got a response to the DFU
*   request, and started receiving the transfer.
*   @RBC_MESH_EVT_DFU_END: The dfu module finished its transfer.
*
* @param[in] type DFU type to request.
* @param[in] p_fwid Firmware ID to request.
*
* @return NRF_SUCCESS The dfu module has started requesting the given transfer.
* @return NRF_ERROR_NULL The FWID pointer provided was NULL.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_INVALID_PARAM The given dfu type is not available.
* @return NRF_ERROR_INVALID_STATE The dfu module is currently running another
* dfu operation. Stop it with @ref dfu_abort() or wait for an end-event
* before requesting a new transfer.
*/
uint32_t dfu_request(dfu_type_t type,
        fwid_union_t* p_fwid,
        uint32_t* p_bank_addr);

/**
* Relay an ongoing transfer. Should only be used as a response to an
* @ref RBC_MESH_EVENT_DFU_RELAY_REQ.
*
* Generates events:
*   @RBC_MESH_EVT_DFU_START: The transfer has started, and the device is
*   actively relaying data packets.
*   @RBC_MESH_EVT_DFU_END: The dfu module finished its transfer.
*
* @param[in] type DFU type to request.
* @param[in] p_fwid Firmware ID to request.
*
* @return NRF_SUCCESS The dfu module has started advertising its intention to
* relay the given transfer.
* @return NRF_ERROR_NULL The FWID pointer provided was NULL.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_INVALID_PARAM The given dfu type is not available.
* @return NRF_ERROR_INVALID_STATE The dfu module is currently running another
* dfu operation. Stop it with @ref dfu_abort() or wait for an end-event
* before requesting a new transfer.
*/
uint32_t dfu_relay(dfu_type_t type,
        fwid_union_t* p_fwid);

/**
* Abort the ongoing dfu operation.
*
* @return NRF_SUCCES The ongoing dfu operation was successfully stopped, and
* the DFU module went back to the idle state.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_INVALID_STATE The dfu module was not doing any dfu
* operations.
*/
uint32_t dfu_abort(void);

/**
* Get info on the bank of the given type.
*
* @params[in] type Type of the bank to get info on.
* @params[out] p_bank_info Pointer to a structure which the function will put
* information on the bank in.
*
* @return NRF_SUCCESS The bank was found, and the @p_bank parameter was filled
* with the correct paramters.
* @return NRF_ERROR_NULL The bank info pointer provided was NULL.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_NOT_FOUND No bank of the given type was found.
*/
uint32_t dfu_bank_info_get(dfu_type_t type, dfu_bank_info_t* p_bank_info);

/**
* Flash the bank of the given type.
*
* @warning This will trigger a restart of the chip. All non-volatile memory
* will be lost during this call. If successful, this never returns.
*
* @param[in] bank_type The dfu type of the bank to be flashed. There can only
* be one bank of each dfu type.
*
* @return NRF_ERROR_NOT_FOUND No bank of the given type is available.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
*/
uint32_t dfu_bank_flash(dfu_type_t bank_type);

/**
* Get the current state of the DFU module.
*
* @param[out] p_dfu_transfer_state A pointer to a dfu transfer state variable,
* which the framework will fill with the current state/progress of an ongoing
* transfer, if any.
*
* @return NRF_SUCCESS The dfu state was successfully retrieved.
* @return NRF_ERROR_NULL The transfer state pointer provided was NULL.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_INVALID_STATE No dfu transfer currently in progress.
*/
uint32_t dfu_state_get(dfu_transfer_state_t* p_dfu_transfer_state);

/**
 * Get the current firmware ID of the given type.
 *
 * @param[in] type Firmware ID type to get.
 * @param[out] p_fwid Firmware ID buffer where the current firmware ID of the
 * given type is copied to.
 *
 * @retval NRF_SUCCESS The firmware ID of the given type was successfully
 * copied to the given firmware ID buffer.
 * @retval NRF_ERROR_NULL The given firmware ID buffer was NULL.
 * @retval NRF_ERROR_INVALID_STATE The dfu module has not been successfully initialized.
 * @retval NRF_ERROR_INVALID_DATA The given type is unknown to the DFU module.
 */
uint32_t dfu_current_fwid_get(dfu_type_t type, fwid_union_t* p_fwid);

/**
 * Check whether the given fwid is an upgrade of the current firmware of the
 * same type.
 *
 * @note For Softdevice transfers, this always returns true if the transfer is
 * different from the current. If the current firmware ID is unknown, this
 * function returns false. This should only happen if the dfu module is
 * uninitialized, or if the bootloader isn't found.
 *
 * @param[in] type Type of firmware to check.
 * @param[in] p_fwid Firmware ID to check.
 *
 * @return Whether the new dfu type is an upgrade of the old, or false if the
 * current firmware version is unknown.
 */
bool dfu_transfer_is_upgrade(dfu_type_t type, const fwid_union_t* p_fwid);




/**
* Send a raw command to the dfu module.
*
* @warning The application should not be using this function directly, unless
* absolutely needed. It is intended for internal use.
*
* @param[in] p_cmd A pointer to a command structure.
*
* @return NRF_SUCCESS The command was successfully sent and executed.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_* The given command did not succeed. The meaning of each
* error code depends on the command.
*/
uint32_t dfu_cmd_send(bl_cmd_t* p_cmd);

/**
* Handler function for DFU events coming from the DFU section.
*
* @warning Not intended for regular use, but can be called to emulate bootloader
* behavior.
*
* @param[in] p_evt Pointer to an event structure.
*
* @return NRF_SUCCESS The event was successfully sent and executed.
* @return NRF_ERROR_NOT_AVAILABLE The dfu functionality is not available.
* @return NRF_ERROR_* The given command did not succeed. The meaning of each
* error code depends on the command.
*/
uint32_t dfu_evt_handler(bl_evt_t* p_evt);

#endif /* DFU_APP_H__ */
