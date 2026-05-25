/* elib_ft_bitmap_core.c - FunctionTool Bitmap Implementation */

#include "elib_ft_bitmap_core.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Bitmap operations                                                  */
/* ------------------------------------------------------------------ */

elib_ft_err_t elib_ft_bitmap_set(uint8_t *buf, uint32_t max_bits, uint32_t bit)
{
    if (buf == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (bit >= max_bits) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    buf[bit / 8] |= (uint8_t)(1u << (bit % 8));
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_bitmap_clear(uint8_t *buf, uint32_t max_bits, uint32_t bit)
{
    if (buf == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (bit >= max_bits) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    buf[bit / 8] &= (uint8_t)~(1u << (bit % 8));
    return ELIB_FT_OK;
}

uint32_t elib_ft_bitmap_get(const uint8_t *buf, uint32_t max_bits, uint32_t bit)
{
    if (buf == NULL || bit >= max_bits) {
        return 0;
    }

    return (buf[bit / 8] >> (bit % 8)) & 1u;
}

void elib_ft_bitmap_set_all(uint8_t *buf, uint32_t max_bits)
{
    if (buf == NULL || max_bits == 0) {
        return;
    }

    uint32_t bytes = (max_bits + 7) / 8;
    memset(buf, 0xFF, (size_t)bytes);

    /* clear unused bits in the last byte */
    uint32_t tail = max_bits % 8;
    if (tail != 0) {
        buf[bytes - 1] &= (uint8_t)((1u << tail) - 1u);
    }
}

void elib_ft_bitmap_clear_all(uint8_t *buf, uint32_t max_bits)
{
    if (buf == NULL || max_bits == 0) {
        return;
    }

    uint32_t bytes = (max_bits + 7) / 8;
    memset(buf, 0x00, (size_t)bytes);
}

void elib_ft_bitmap_shift_left(uint8_t *buf, uint32_t max_bits, uint32_t n)
{
    if (buf == NULL || max_bits == 0 || n == 0) {
        return;
    }
    if (n >= max_bits) {
        elib_ft_bitmap_clear_all(buf, max_bits);
        return;
    }

    uint32_t total_bytes = (max_bits + 7) / 8;
    uint32_t byte_shift = n / 8;
    uint32_t bit_shift  = n % 8;

    /* byte-level: move data to higher addresses */
    if (byte_shift > 0) {
        memmove(buf + byte_shift, buf, (size_t)(total_bytes - byte_shift));
        memset(buf, 0, (size_t)byte_shift);
    }

    /* bit-level: carry propagates low to high */
    if (bit_shift > 0) {
        uint8_t carry = 0;
        for (uint32_t i = 0; i < total_bytes; i++) {
            uint8_t new_carry = (uint8_t)(buf[i] >> (8 - bit_shift));
            buf[i] = (uint8_t)(buf[i] << bit_shift) | carry;
            carry = new_carry;
        }
    }

    /* clear unused tail bits */
    uint32_t tail = max_bits % 8;
    if (tail != 0) {
        buf[total_bytes - 1] &= (uint8_t)((1u << tail) - 1u);
    }
}

void elib_ft_bitmap_shift_right(uint8_t *buf, uint32_t max_bits, uint32_t n)
{
    if (buf == NULL || max_bits == 0 || n == 0) {
        return;
    }
    if (n >= max_bits) {
        elib_ft_bitmap_clear_all(buf, max_bits);
        return;
    }

    uint32_t total_bytes = (max_bits + 7) / 8;
    uint32_t byte_shift = n / 8;
    uint32_t bit_shift  = n % 8;

    /* byte-level: move data to lower addresses */
    if (byte_shift > 0) {
        memmove(buf, buf + byte_shift, (size_t)(total_bytes - byte_shift));
        memset(buf + total_bytes - byte_shift, 0, (size_t)byte_shift);
    }

    /* bit-level: carry propagates high to low */
    if (bit_shift > 0) {
        uint8_t carry = 0;
        uint32_t i = total_bytes;
        while (i-- > 0) {
            uint8_t new_carry = (uint8_t)(buf[i] << (8 - bit_shift));
            buf[i] = (buf[i] >> bit_shift) | carry;
            carry = new_carry;
        }
    }

    /* clear unused tail bits */
    uint32_t tail = max_bits % 8;
    if (tail != 0) {
        buf[total_bytes - 1] &= (uint8_t)((1u << tail) - 1u);
    }
}

/* ------------------------------------------------------------------ */
/*  Bitmap helpers                                                     */
/* ------------------------------------------------------------------ */

/* count trailing zeros in a byte (binary search, 3 steps) */
static uint32_t ctz8(uint8_t x)
{
    if (x == 0) return 8;
    uint32_t n = 0;
    if ((x & 0x0Fu) == 0) { n += 4; x >>= 4; }
    if ((x & 0x03u) == 0) { n += 2; x >>= 2; }
    if ((x & 0x01u) == 0) { n += 1; }
    return n;
}

/* mask for valid bits in the last byte */
static uint8_t tail_mask(uint32_t max_bits)
{
    uint32_t tail = max_bits % 8;
    return (tail == 0) ? 0xFFu : (uint8_t)((1u << tail) - 1u);
}

uint32_t elib_ft_bitmap_find_first_set(const uint8_t *buf, uint32_t max_bits, uint32_t start)
{
    if (buf == NULL || start >= max_bits) {
        return max_bits;
    }

    uint32_t total_bytes = (max_bits + 7) / 8;
    uint32_t byte_idx   = start / 8;
    uint32_t bit_offset = start % 8;

    /* first byte: mask off bits below start, mask off bits above max_bits */
    uint8_t first_mask = (uint8_t)(0xFFu << bit_offset);
    if (byte_idx == total_bytes - 1) {
        first_mask &= tail_mask(max_bits);
    }

    uint8_t first = buf[byte_idx] & first_mask;
    if (first != 0) {
        return byte_idx * 8 + ctz8(first);
    }

    /* scan remaining bytes, skip all-zero bytes */
    for (byte_idx++; byte_idx < total_bytes; byte_idx++) {
        if (buf[byte_idx] != 0) {
            uint32_t pos = byte_idx * 8 + ctz8(buf[byte_idx]);
            return (pos < max_bits) ? pos : max_bits;
        }
    }

    return max_bits;
}

uint32_t elib_ft_bitmap_find_first_clear(const uint8_t *buf, uint32_t max_bits, uint32_t start)
{
    if (buf == NULL || start >= max_bits) {
        return max_bits;
    }

    uint32_t total_bytes = (max_bits + 7) / 8;
    uint32_t byte_idx   = start / 8;
    uint32_t bit_offset = start % 8;

    /* first byte: mask off bits below start, mask off bits above max_bits */
    uint8_t first_mask = (uint8_t)(0xFFu << bit_offset);
    if (byte_idx == total_bytes - 1) {
        first_mask &= tail_mask(max_bits);
    }

    uint8_t first = ~buf[byte_idx] & first_mask;
    if (first != 0) {
        return byte_idx * 8 + ctz8(first);
    }

    /* scan remaining bytes, skip all-0xFF bytes */
    for (byte_idx++; byte_idx < total_bytes; byte_idx++) {
        uint8_t inv;
        if (byte_idx == total_bytes - 1) {
            inv = ~buf[byte_idx] & tail_mask(max_bits);
        } else {
            inv = ~buf[byte_idx];
        }
        if (inv != 0) {
            return byte_idx * 8 + ctz8(inv);
        }
    }

    return max_bits;
}