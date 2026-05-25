/* elib_ft_bit.h - FunctionTool Bit Operations */

#ifndef ELIB_FT_BIT_H
#define ELIB_FT_BIT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Population count                                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief Count number of set bits (1s) in a byte
 * @param x Input byte
 * @return Number of set bits (0-8)
 */
uint32_t elib_ft_popcount8(uint8_t x);

/**
 * @brief Count number of set bits (1s) in a 16-bit value
 * @param x Input value
 * @return Number of set bits (0-16)
 */
uint32_t elib_ft_popcount16(uint16_t x);

/**
 * @brief Count number of set bits (1s) in a 32-bit value
 * @param x Input value
 * @return Number of set bits (0-32)
 */
uint32_t elib_ft_popcount32(uint32_t x);

/* ------------------------------------------------------------------ */
/*  Find next set/clear bit                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Find next set bit (1) after given position in a bitmap
 * @param buf Bitmap buffer (uint8_t array)
 * @param max_bits Total bit count
 * @param start Search from this position (exclusive)
 * @return Index of next set bit, or max_bits if not found
 */
uint32_t elib_ft_bit_find_next_set(const uint8_t *buf, uint32_t max_bits, uint32_t start);

/**
 * @brief Find next clear bit (0) after given position in a bitmap
 * @param buf Bitmap buffer (uint8_t array)
 * @param max_bits Total bit count
 * @param start Search from this position (exclusive)
 * @return Index of next clear bit, or max_bits if not found
 */
uint32_t elib_ft_bit_find_next_clear(const uint8_t *buf, uint32_t max_bits, uint32_t start);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_BIT_H */
