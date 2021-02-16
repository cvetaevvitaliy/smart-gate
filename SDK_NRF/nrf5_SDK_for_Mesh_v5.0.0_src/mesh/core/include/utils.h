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
#ifndef MESH_UTILS_H__
#define MESH_UTILS_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "hal.h"

#if !defined _lint && !defined HOST
#   include "app_util.h"
#else
#   include "nordic_common.h"
#endif

/**
 * @defgroup UTILS Utility functions
 * @ingroup MESH_CORE
 * This library provides implementations of common utility functions.
 *
 * @{
 */

#if defined(__CC_ARM) && !defined(_lint)
#   include <nrf.h>
#   define BE2LE16(n) __REV16(n) /**< Converts a 16-bit big-endian number to little-endian. */
#   define LE2BE16(n) __REV16(n) /**< Converts a 16-bit little-endian number to big endian. */
#   define BE2LE32(n) __REV(n)   /**< Converts a 32-bit big-endian number to little-endian. */
#   define LE2BE32(n) __REV(n)   /**< Converts a 32-bit little-endian number to big-endian. */
#else
#   define BE2LE16(n) ((((n) << 8) & 0xff00) | (((n) >> 8) & 0x00ff)) /**< Converts a 16-bit big-endian number to little-endian. */
#   define LE2BE16(n) BE2LE16(n)                                      /**< Converts a 16-bit little-endian number to big-endian. */
#   define BE2LE32(n) LE2BE32(n)    /**< Converts a 32-bit big-endian number to little-endian. */
#   define LE2BE32(n) (((uint32_t) (n) << 24) | (((uint32_t) (n) << 8) & 0xff0000) \
        | (((n) >> 8) & 0xff00) | ((n) >> 24))  /**< Converts a 32-bit little-endian number to big-endian. */
#endif

/** Converts a 24-bit little-endian number to big-endian. */
#define LE2BE24(n) ((((n) & 0x00FF0000) >> 16) |        \
                    ( (n) & 0x0000FF00)        |        \
                    (((n) & 0x000000FF) << 16))

/** Converts a 24-bit big-endian number to little-endian. */
#define BE2LE24(n) LE2BE24(n)

/** Size of a word on the current hardware platform. */
#define WORD_SIZE (sizeof(int))

#if defined(_lint)
    /** Entry macro for debugging atomic functions. */
    #define DEBUG_ATOMIC_FUNCTION_ENTER(lock)
    #define DEBUG_ATOMIC_FUNCTION_EXIT(lock)
#elif (NRF51 || NRF52_SERIES)
/** Entry macro for debugging atomic functions. */
#define DEBUG_ATOMIC_FUNCTION_ENTER(lock) do                                            \
                                          {                                             \
                                              uint32_t masked_irqs;                     \
                                              _DISABLE_IRQS(masked_irqs);               \
                                              /* Are we interrupting an existing call?*/\
                                              NRF_MESH_ASSERT(lock == 0xFFFFFFFF);      \
                                              _GET_LR(lock);                            \
                                              _ENABLE_IRQS(masked_irqs);                \
                                          }while(0)

/** Exit macro for debuging atomic functions. */
#define DEBUG_ATOMIC_FUNCTION_EXIT(lock) (lock = 0xFFFFFFFF)
#else
#define DEBUG_ATOMIC_FUNCTION_ENTER(lock)   {}
#define DEBUG_ATOMIC_FUNCTION_EXIT(lock)    {}
#endif

#if HOST
#define IS_VALID_RAM_ADDR(ADDR) (true)
#else
/** Check if an address is a valid RAM address. */
#define IS_VALID_RAM_ADDR(ADDR)     (                                                              \
        (((uint32_t)(ADDR) > DATA_RAM_START) && ((uint32_t)(ADDR) < DEVICE_DATA_RAM_END_GET())) || \
        (((uint32_t)(ADDR) > CODE_RAM_START) && ((uint32_t)(ADDR) < DEVICE_CODE_RAM_END_GET()))    \
                                    )
#endif

/** Check whether the given pointer is page aligned. */
#define IS_PAGE_ALIGNED(p) (((uint32_t)(p) & (PAGE_SIZE - 1)) == 0)
/** Check whether the given pointer is word aligned. */
#define IS_WORD_ALIGNED(p) (((uint32_t)(p) & (WORD_SIZE - 1)) == 0)
/** Returns an equivalent for a number (X) aligned to given word size (A). Value of A must be 2^n. */
#define ALIGN_VAL(X, A)          (((X)+((A)-1))&~((A)-1))
/** Checks whether the given value is power of two. */
#define IS_POWER_OF_2(VALUE) ((VALUE) && (((VALUE) & ((VALUE) - 1)) == 0))

/**@brief Macro for performing rounded integer division (as opposed to truncating the result).
 *
 * @param[in]   A   Numerator.
 * @param[in]   B   Denominator.
 *
 * @return      Rounded (integer) result of dividing A by B.
 */
#ifndef ROUNDED_DIV
#define ROUNDED_DIV(A, B) (((A) + ((B) / 2)) / (B))
#endif

/**
 * Converts hours to seconds.
 * @param t The number of hours.
 * @return  The number of seconds corresponding to the specified number of hours.
 */
#define HOURS_TO_SECONDS(t) ((t) * 60 * 60)

/**
 * Converts minutes to milliseconds.
 * @param t The number of minutes.
 * @return  The number of milliseconds corresponding to the specified number of minutes.
 */
#define MIN_TO_MS(t) ((t) * 60000ul)

/**
 * Converts seconds to microseconds.
 * @param t The number of seconds.
 * @return  The number of microseconds corresponding to the specified number of seconds.
 */
#define SEC_TO_US(t) ((t) * 1000000ul)

/**
 * Converts seconds to milliseconds.
 * @param t The number of seconds.
 * @return  The number of milliseconds corresponding to the specified number of seconds.
 */
#define SEC_TO_MS(t) ((t) * 1000)

/**
 * Converts milliseconds to microseconds.
 * @param t The number of milliseconds.
 * @return  The number of microseconds corresponding to the specified number of milliseconds.
 */
#define MS_TO_US(t) ((t) * 1000)

/**
 * Converts milliseconds to seconds.
 * @param t The number of milliseconds.
 * @return  The number of seconds corresponding to the specified number of milliseconds.
 */
#define MS_TO_SEC(t) (ROUNDED_DIV(t, 1000))

/**
 * Converts milliseconds to minutes.
 * @param t The number of milliseconds.
 * @return  The number of minutes corresponding to the specified number of milliseconds.
 */
#define MS_TO_MIN(t) ((t) / 60000ul)

/**
 * Converts microseconds to milliseconds.
 * @param t The number of microseconds.
 * @return  The number of milliseconds corresponding to the specified number of microseconds.
 */
#define US_TO_MS(t) (ROUNDED_DIV(t, 1000))

/**
 * Converts microseconds to seconds.
 * @param t The number of microseconds.
 * @return  The number of seconds corresponding to the specified number of microseconds.
 */
#define US_TO_SEC(t) (ROUNDED_DIV(t, 1000000ul))

/**
 * Macro for checking if min <= val <= max.
 *
 * @param[in] val Value to check.
 * @param[in] min Range minimum.
 * @param[in] max Range maximum.
 *
 * @retval true  If the value is within the range.
 * @retval false Otherwise.
 */
#define IS_IN_RANGE(val, min, max) (((min) <= (val) && (val) <= (max)))

/**
 * Get pointer to the start of a structure given a pointer to one of the structure's fields.
 *
 * @param[in] STRUCT_TYPE   Type of structure.
 * @param[in] FIELD_NAME    Name of field.
 * @param[in] FIELD_POINTER Pointer to field inside the structure.
 *
 * @return Pointer to start of structure.
 */
#define PARENT_BY_FIELD_GET(STRUCT_TYPE, FIELD_NAME, FIELD_POINTER) \
    ((STRUCT_TYPE *) (((uint8_t *)FIELD_POINTER) - offsetof(STRUCT_TYPE, FIELD_NAME)))

/**@brief Macro for performing integer division, making sure the result is rounded up.
 *
 * @details One typical use for this is to compute the number of objects with size B is needed to
 *          hold A number of bytes.
 *
 * @param[in]   A   Numerator.
 * @param[in]   B   Denominator.
 *
 * @return      Integer result of dividing A by B, rounded up.
 */
#ifndef CEIL_DIV
#define CEIL_DIV(A, B)   (((A) + (B) - 1) / (B))
#endif

/**
 * Check if a value is power of two without evaluating the value multiple times.
 *
 * See https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2 for details.
 *
 * @param[in] value Value to check for.
 *
 * @returns Whether value is power of two.
 */
static inline bool is_power_of_two(uint32_t value)
{
    return IS_POWER_OF_2(value);
}

/**
 * Reverse memcpy.
 *
 * Writes size bytes from p_src to p_dst in reverse order.
 *
 * @param p_dst Destination address.
 * @param p_src Source address.
 * @param size  Number of bytes to write.
 */
static inline void utils_reverse_memcpy(uint8_t * p_dst, const uint8_t * p_src, uint16_t size)
{
    p_src += size;
    while (size--)
    {
        *((uint8_t *) p_dst++) = *((uint8_t *) --p_src);
    }
}

/**
 * Reverses an array in-place.
 *
 * @param p_array The array to reverse.
 * @param size    The length of the array to reverse.
 */
static inline void utils_reverse_array(uint8_t * p_array, uint16_t size)
{
    for(uint16_t i = 0; i < size / 2; ++i)
    {
        uint8_t temp = p_array[i];
        p_array[i] = p_array[size - i - 1];
        p_array[size - i - 1] = temp;
    }
}

/**
 * Bytewise XOR for an array.
 *
 * XORs size amount of data from p_src1 and p_src2 and stores it in p_dst.
 *
 * @note p_dst may be equal to one or more of the sources.
 *
 * @param p_dst  Destination address.
 * @param p_src1 First source address.
 * @param p_src2 Secound source address.
 * @param size   Number of bytes to XOR.
 */
static inline void utils_xor(uint8_t * p_dst, const uint8_t * p_src1, const uint8_t * p_src2, uint16_t size)
{
    while (0 != size)
    {
        size--;
        p_dst[size] = p_src1[size] ^ p_src2[size];
    }
}

/**
 * Left shift an array of bytes one bit. p_dst and p_src may be the same.
 *
 * @param p_dst Destination address.
 * @param p_src Source address.
 * @param size  Size of p_dst and p_src.
 */
static inline void utils_lshift(uint8_t * p_dst, const uint8_t * p_src, uint16_t size)
{
    for (uint16_t i = 0; i < size - 1; ++i)
    {
        p_dst[i] = (p_src[i] << 1);
        p_dst[i] |= !!(p_src[i + 1] & 0x80);
    }
    p_dst[size - 1] = (p_src[size - 1] << 1);
}

/**
 * Pads an array according to AES-CMAC RFC 4493.
 *
 * For an input string x of r-octets, where 0 <= r < 16, the padding
 * function, padding(x), is defined as follows:
 *
 * @verbatim padding(x) = x || 10^i      where i is 128-8*r-1 @endverbatim
 *
 * That is, padding(x) is the concatenation of x and a single '1',
 * followed by the minimum number of '0's, so that the total length is
 * equal to 128 bits.
 *
 * @param p_dst Destination address.
 * @param p_src Source address.
 * @param size  Size of p_dst and p_src arrays.
 */
static inline void utils_pad(uint8_t * p_dst, const uint8_t * p_src, uint16_t size)
{
    memcpy(p_dst, p_src, size);
    p_dst[size] = 0x80;

    for (int i = size+1; i < 16; ++i)
        p_dst[i] = 0x00;
}

/**
 * Gets the integer value of the binary logarithm of a number.
 *
 * @param[in] value Input value for the calculation.
 *
 * @returns Returns the binary logarithm of the input number.
 */
static inline uint8_t log2_get(uint32_t value)
{
    uint8_t log_val = 0;
    while ((value >>= 1) != 0)
    {
        log_val++;
    }

    return log_val;
}

/**
 * Gets the integer square root of a natural number.
 *
 * @param[in] value Input value for the calculation.
 *
 * @returns Returns the square root of the input number.
 */
static inline uint16_t uint_sqrt(uint32_t value)
{
    uint16_t c = 0x8000;
    uint16_t g = 0x8000;

    for (;;)
    {
        if ((uint32_t)g * g > value)
        {
            g ^= c;
        }

        c >>= 1;

        if (c == 0)
        {
            return g;
        }
        g |= c;
    }
}


/**
 * @}
 */
#endif
