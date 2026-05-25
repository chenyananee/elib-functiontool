/* elib_ft_bit_core.c - FunctionTool Bit Operations */

#include "elib_ft_bit.h"

/* ------------------------------------------------------------------ */
/*  Internal: count trailing zeros in a nibble (0-4, 4 means none)     */
/* ------------------------------------------------------------------ */

static uint32_t ctz4(uint8_t x)
{
    x &= 0x0F;
    if ((x & 0x03) == 0) {
        return 2 + ((x & 0x08) ? 0 : 1);
    }
    return (x & 0x01) ? 0 : 1;
}

/* ------------------------------------------------------------------ */
/*  Internal: count trailing zeros in a byte (0-8, 8 means none)       */
/* ------------------------------------------------------------------ */

static uint32_t ctz8(uint8_t x)
{
    if (x == 0) {
        return 8;
    }
    if ((x & 0x0F) == 0) {
        return 4 + ctz4(x >> 4);
    }
    return ctz4(x);
}

/* ------------------------------------------------------------------ */
/*  Population count                                                    */
/* ------------------------------------------------------------------ */

uint32_t elib_ft_popcount8(uint8_t x)
{
    x = (x & 0x55) + ((x >> 1) & 0x55);
    x = (x & 0x33) + ((x >> 2) & 0x33);
    x = (x & 0x0F) + ((x >> 4) & 0x0F);
    return x;
}

uint32_t elib_ft_popcount16(uint16_t x)
{
    x = (x & 0x5555u) + ((x >> 1) & 0x5555u);
    x = (x & 0x3333u) + ((x >> 2) & 0x3333u);
    x = (x & 0x0F0Fu) + ((x >> 4) & 0x0F0Fu);
    x = (x & 0x00FFu) + ((x >> 8) & 0x00FFu);
    return x;
}

uint32_t elib_ft_popcount32(uint32_t x)
{
    x = (x & 0x55555555u) + ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x & 0x0F0F0F0Fu) + ((x >> 4) & 0x0F0F0F0Fu);
    x = (x & 0x00FF00FFu) + ((x >> 8) & 0x00FF00FFu);
    x = (x & 0x0000FFFFu) + ((x >> 16) & 0x0000FFFFu);
    return x;
}

/* ------------------------------------------------------------------ */
/*  Find next set/clear bit                                             */
/* ------------------------------------------------------------------ */

uint32_t elib_ft_bit_find_next_set(const uint8_t *buf, uint32_t max_bits, uint32_t start)
{
    if (buf == NULL || start >= max_bits) {
        return max_bits;
    }
    uint32_t byte_idx = start >> 3;
    uint32_t bit_offset = start & 7;
    uint8_t mask = (uint8_t)(0xFFu << bit_offset);
    uint8_t b = buf[byte_idx] & mask;
    while (b == 0) {
        byte_idx++;
        if (byte_idx >= (max_bits + 7) >> 3) {
            return max_bits;
        }
        mask = 0xFF;
        b = buf[byte_idx];
        bit_offset = 0;
    }
    uint32_t base = byte_idx << 3;
    uint32_t tz = ctz8(b);
    uint32_t pos = base + tz;
    return (pos < max_bits) ? pos : max_bits;
}

uint32_t elib_ft_bit_find_next_clear(const uint8_t *buf, uint32_t max_bits, uint32_t start)
{
    if (buf == NULL || start >= max_bits) {
        return max_bits;
    }
    uint32_t byte_idx = start >> 3;
    uint32_t bit_offset = start & 7;
    /* mask covers only bits from bit_offset to bit 7 */
    uint8_t mask = (uint8_t)(0xFFu << (8 - bit_offset));
    uint8_t b = buf[byte_idx] & ~mask;
    while (b == 0) {
        byte_idx++;
        if (byte_idx >= (max_bits + 7) >> 3) {
            return max_bits;
        }
        mask = 0xFF;
        b = buf[byte_idx];
    }
    uint32_t base = byte_idx << 3;
    uint32_t tz = ctz8(b);
    uint32_t pos = base + tz;
    return (pos < max_bits) ? pos : max_bits;
}