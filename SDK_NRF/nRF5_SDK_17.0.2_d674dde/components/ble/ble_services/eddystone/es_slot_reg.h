/**
 * Copyright (c) 2016 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
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
 *
 */
#ifndef ES_SLOT_REG_H__
#define ES_SLOT_REG_H__

#include <stdint.h>
#include "es_slot.h"

/**
 * @file
 * @addtogroup eddystone_slot
 * @{
 */

/** @brief Function for checking if an eTLM frame is required.
 *
 * @param[in]   p_reg   Pointer to the slot registry.
 *
 * @retval      true    If an eTLM frame is required.
 * @retval      false   Otherwise.
 */
bool es_slot_reg_etlm_required(const es_slot_reg_t * p_reg);

/** @brief Function for clearing a slot.
 *
 * @param[in]   p_reg       Pointer to the slot registry.
 * @param[in]   slot_no     The slot number to clear.
 *
 * @retval      true        If an EID slot was cleared.
 */
bool es_slot_reg_clear_slot(es_slot_reg_t * p_reg, uint8_t slot_no);

/** @brief Function for updating the state of the slot registry after adding a slot.
 *
 * @param[in]   p_reg       Pointer to the slot registry.
 * @param[in]   slot_no     The slot number that was added.
 * @param[in]   frame_type  The frame type that was added.
 * @param[in]   init        Information if the data is loaded during initialization. Set this
 * parameter to false if the call is a result of a write to the Eddystone Configuration Service.
 */
void es_slot_reg_update_slot_list_info_on_add(es_slot_reg_t * p_reg, uint8_t slot_no, es_frame_type_t frame_type, bool init);

/** @brief Function for initializing the slot registry.
 *
 * @param[in]   p_reg       Pointer to the slot registry to initialize.
 */
void es_slot_reg_init(es_slot_reg_t * p_reg);

/**
 * @}
 */

#endif // ES_SLOT_REG_H__
