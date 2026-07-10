/* elib_ft_mem.h - FunctionTool Memory Operations */

#ifndef ELIB_FT_MEM_H
#define ELIB_FT_MEM_H

#include <stddef.h>
#include <stdint.h>
#include "elib_ft_err.h"
#include "elib_ft_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Memory copy / fill                                                  */
/* ------------------------------------------------------------------ */

/**
 * @brief Copy memory with max length limit
 * @param dst Destination buffer
 * @param src Source buffer
 * @param len Requested copy length in bytes
 * @param max_len Maximum allowed length in bytes; truncates if len exceeds
 * @return Actual number of bytes copied (0 on NULL or zero length)
 */
uint32_t elib_ft_memcpy(void *dst, const void *src, uint32_t len, uint32_t max_len);

/**
 * @brief Fill memory with a byte value, with max length limit
 * @param dst Destination buffer
 * @param val Byte value to fill
 * @param len Requested fill length in bytes
 * @param max_len Maximum allowed length in bytes; truncates if len exceeds
 * @return Actual number of bytes set (0 on NULL or zero length)
 */
uint32_t elib_ft_memset(void *dst, uint8_t val, uint32_t len, uint32_t max_len);

/* ------------------------------------------------------------------ */
/*  Bitmap operations                                                  */
/* ------------------------------------------------------------------ */

/**
 * @brief Set a bit in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param bit Bit index to set
 * @return ELIB_FT_OK on success, error code otherwise
 */
elib_ft_err_t elib_ft_bitmap_set(uint8_t *buf, uint32_t max_bits, uint32_t bit);

/**
 * @brief Clear a bit in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param bit Bit index to clear
 * @return ELIB_FT_OK on success, error code otherwise
 */
elib_ft_err_t elib_ft_bitmap_clear(uint8_t *buf, uint32_t max_bits, uint32_t bit);

/**
 * @brief Get a bit value from bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param bit Bit index to query
 * @return 1 if set, 0 if clear or error
 */
uint32_t elib_ft_bitmap_get(const uint8_t *buf, uint32_t max_bits, uint32_t bit);

/**
 * @brief Set all bits in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 */
void elib_ft_bitmap_set_all(uint8_t *buf, uint32_t max_bits);

/**
 * @brief Clear all bits in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 */
void elib_ft_bitmap_clear_all(uint8_t *buf, uint32_t max_bits);

/**
 * @brief Left shift bitmap by n bits (MSB side, high bits lost)
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param n Number of bits to shift
 */
void elib_ft_bitmap_shift_left(uint8_t *buf, uint32_t max_bits, uint32_t n);

/**
 * @brief Right shift bitmap by n bits (LSB side, low bits lost)
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param n Number of bits to shift
 */
void elib_ft_bitmap_shift_right(uint8_t *buf, uint32_t max_bits, uint32_t n);

/**
 * @brief Find first set bit (1) from start index
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param start Bit index to start searching from
 * @return Index of first 1 bit, or max_bits if not found
 */
uint32_t elib_ft_bitmap_find_first_set(const uint8_t *buf, uint32_t max_bits, uint32_t start);

/**
 * @brief Find first clear bit (0) from start index
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param start Bit index to start searching from
 * @return Index of first 0 bit, or max_bits if not found
 */
uint32_t elib_ft_bitmap_find_first_clear(const uint8_t *buf, uint32_t max_bits, uint32_t start);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_MEM_H */
