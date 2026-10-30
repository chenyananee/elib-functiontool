/* test_elib_ft_bitmap.c - FunctionTool Bitmap Operations Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_bitmap_set_get(void)
{
    uint8_t buf[2] = {0};
    assert(elib_ft_bitmap_set(buf, 16, 0) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 16, 0) == 1);
    assert(elib_ft_bitmap_set(buf, 16, 7) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 16, 7) == 1);
    assert(elib_ft_bitmap_set(buf, 16, 8) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 16, 8) == 1);
    assert(elib_ft_bitmap_get(buf, 16, 1) == 0);
    assert(buf[0] == 0x81);
    assert(buf[1] == 0x01);
}

static void test_bitmap_clear(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_clear(buf, 8, 3) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 8, 3) == 0);
    assert(buf[0] == 0xF7);
}

static void test_bitmap_set_out_of_range(void)
{
    uint8_t buf[1] = {0};
    assert(elib_ft_bitmap_set(buf, 8, 8) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_bitmap_get(buf, 8, 8) == 0);
}

static void test_bitmap_clear_out_of_range(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_clear(buf, 8, 8) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_bitmap_null_buf(void)
{
    assert(elib_ft_bitmap_set(NULL, 8, 0) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_bitmap_clear(NULL, 8, 0) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_bitmap_get(NULL, 8, 0) == 0);
}

static void test_bitmap_set_all(void)
{
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set_all(buf, 10);
    /* 10 bits: 0b11_1111_1111 => buf[0]=0xFF, buf[1]=0x03 */
    assert(buf[0] == 0xFF);
    assert(buf[1] == 0x03);
}

static void test_bitmap_set_all_byte_aligned(void)
{
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set_all(buf, 16);
    assert(buf[0] == 0xFF);
    assert(buf[1] == 0xFF);
}

static void test_bitmap_clear_all(void)
{
    uint8_t buf[2] = {0xFF, 0xFF};
    elib_ft_bitmap_clear_all(buf, 16);
    assert(buf[0] == 0x00);
    assert(buf[1] == 0x00);
}

static void test_bitmap_shift_left(void)
{
    /* bits: 0,1,2 set -> shift left 1 -> bits: 1,2,3 set */
    uint8_t buf[1] = {0};
    elib_ft_bitmap_set(buf, 8, 0);
    elib_ft_bitmap_set(buf, 8, 1);
    elib_ft_bitmap_set(buf, 8, 2);
    elib_ft_bitmap_shift_left(buf, 8, 1);
    assert(elib_ft_bitmap_get(buf, 8, 0) == 0);
    assert(elib_ft_bitmap_get(buf, 8, 1) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 2) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 3) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 4) == 0);
}

static void test_bitmap_shift_left_overflow(void)
{
    uint8_t buf[1] = {0};
    elib_ft_bitmap_set(buf, 8, 7);
    elib_ft_bitmap_shift_left(buf, 8, 1);
    /* bit 7 shifted out, lost */
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_left_cross_byte(void)
{
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set(buf, 16, 0);
    elib_ft_bitmap_shift_left(buf, 16, 8);
    assert(elib_ft_bitmap_get(buf, 16, 0) == 0);
    assert(elib_ft_bitmap_get(buf, 16, 8) == 1);
    assert(buf[0] == 0x00);
    assert(buf[1] == 0x01);
}

static void test_bitmap_shift_left_all(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_left(buf, 8, 8);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_left_large(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_left(buf, 8, 100);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_right(void)
{
    /* bits: 5,6,7 set -> shift right 1 -> bits: 4,5,6 set */
    uint8_t buf[1] = {0xE0};
    elib_ft_bitmap_shift_right(buf, 8, 1);
    assert(buf[0] == 0x70);
    assert(elib_ft_bitmap_get(buf, 8, 6) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 5) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 4) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 7) == 0);
}

static void test_bitmap_shift_right_overflow(void)
{
    uint8_t buf[1] = {0x01};
    elib_ft_bitmap_shift_right(buf, 8, 1);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_right_cross_byte(void)
{
    uint8_t buf[2] = {0x00, 0x01};
    elib_ft_bitmap_shift_right(buf, 16, 8);
    assert(elib_ft_bitmap_get(buf, 16, 0) == 1);
    assert(buf[0] == 0x01);
    assert(buf[1] == 0x00);
}

static void test_bitmap_shift_right_all(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_right(buf, 8, 8);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_right_large(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_right(buf, 8, 100);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_zero(void)
{
    uint8_t buf[1] = {0xAA};
    elib_ft_bitmap_shift_left(buf, 8, 0);
    assert(buf[0] == 0xAA);
    elib_ft_bitmap_shift_right(buf, 8, 0);
    assert(buf[0] == 0xAA);
}

static void test_bitmap_non_byte_aligned(void)
{
    /* 12 bits across 2 bytes */
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set(buf, 12, 9);
    assert(elib_ft_bitmap_get(buf, 12, 9) == 1);
    assert(buf[1] == 0x02);

    elib_ft_bitmap_shift_right(buf, 12, 1);
    assert(elib_ft_bitmap_get(buf, 12, 8) == 1);
    assert(elib_ft_bitmap_get(buf, 12, 9) == 0);
}

static void test_bitmap_null_shift(void)
{
    elib_ft_bitmap_shift_left(NULL, 8, 1);
    elib_ft_bitmap_shift_right(NULL, 8, 1);
}

static void test_bitmap_null_set_clear_all(void)
{
    elib_ft_bitmap_set_all(NULL, 8);
    elib_ft_bitmap_clear_all(NULL, 8);
    elib_ft_bitmap_set_all((uint8_t *)1, 0);
    elib_ft_bitmap_clear_all((uint8_t *)1, 0);
}

static void test_bitmap_find_first_set_basic(void)
{
    /* buf = 0b00100100, bits 2 and 5 are set */
    uint8_t buf[1] = {0x24};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 0) == 2);
    assert(elib_ft_bitmap_find_first_set(buf, 8, 3) == 5);
    assert(elib_ft_bitmap_find_first_set(buf, 8, 6) == 8);
}

static void test_bitmap_find_first_set_none(void)
{
    uint8_t buf[1] = {0x00};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 0) == 8);
}

static void test_bitmap_find_first_set_all_set(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 0) == 0);
    assert(elib_ft_bitmap_find_first_set(buf, 8, 7) == 7);
}

static void test_bitmap_find_first_set_cross_byte(void)
{
    /* bit 9 set */
    uint8_t buf[2] = {0x00, 0x02};
    assert(elib_ft_bitmap_find_first_set(buf, 16, 0) == 9);
}

static void test_bitmap_find_first_clear_basic(void)
{
    /* buf = 0b11011011, bits 2 and 5 are clear */
    uint8_t buf[1] = {0xDB};
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 0) == 2);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 3) == 5);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 6) == 8);
}

static void test_bitmap_find_first_clear_none(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 0) == 8);
}

static void test_bitmap_find_first_clear_all_clear(void)
{
    uint8_t buf[1] = {0x00};
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 0) == 0);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 7) == 7);
}

static void test_bitmap_find_first_clear_cross_byte(void)
{
    /* bit 10 clear */
    uint8_t buf[2] = {0xFF, 0xFB};
    assert(elib_ft_bitmap_find_first_clear(buf, 16, 0) == 10);
}

static void test_bitmap_find_null_buf(void)
{
    assert(elib_ft_bitmap_find_first_set(NULL, 8, 0) == 8);
    assert(elib_ft_bitmap_find_first_clear(NULL, 8, 0) == 8);
}

static void test_bitmap_find_start_out_of_range(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 8) == 8);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 8) == 8);
}

static void test_bitmap_find_non_byte_aligned(void)
{
    /* 10 bits, bit 9 set */
    uint8_t buf[2] = {0x00, 0x02};
    assert(elib_ft_bitmap_find_first_set(buf, 10, 0) == 9);
    assert(elib_ft_bitmap_find_first_set(buf, 10, 10) == 10);
}

int main(void)
{
    RUN_TEST(test_bitmap_set_get);
    RUN_TEST(test_bitmap_clear);
    RUN_TEST(test_bitmap_set_out_of_range);
    RUN_TEST(test_bitmap_clear_out_of_range);
    RUN_TEST(test_bitmap_null_buf);
    RUN_TEST(test_bitmap_set_all);
    RUN_TEST(test_bitmap_set_all_byte_aligned);
    RUN_TEST(test_bitmap_clear_all);
    RUN_TEST(test_bitmap_shift_left);
    RUN_TEST(test_bitmap_shift_left_overflow);
    RUN_TEST(test_bitmap_shift_left_cross_byte);
    RUN_TEST(test_bitmap_shift_left_all);
    RUN_TEST(test_bitmap_shift_left_large);
    RUN_TEST(test_bitmap_shift_right);
    RUN_TEST(test_bitmap_shift_right_overflow);
    RUN_TEST(test_bitmap_shift_right_cross_byte);
    RUN_TEST(test_bitmap_shift_right_all);
    RUN_TEST(test_bitmap_shift_right_large);
    RUN_TEST(test_bitmap_shift_zero);
    RUN_TEST(test_bitmap_non_byte_aligned);
    RUN_TEST(test_bitmap_null_shift);
    RUN_TEST(test_bitmap_null_set_clear_all);
    RUN_TEST(test_bitmap_find_first_set_basic);
    RUN_TEST(test_bitmap_find_first_set_none);
    RUN_TEST(test_bitmap_find_first_set_all_set);
    RUN_TEST(test_bitmap_find_first_set_cross_byte);
    RUN_TEST(test_bitmap_find_first_clear_basic);
    RUN_TEST(test_bitmap_find_first_clear_none);
    RUN_TEST(test_bitmap_find_first_clear_all_clear);
    RUN_TEST(test_bitmap_find_first_clear_cross_byte);
    RUN_TEST(test_bitmap_find_null_buf);
    RUN_TEST(test_bitmap_find_start_out_of_range);
    RUN_TEST(test_bitmap_find_non_byte_aligned);

    printf("\nAll %d tests passed.\n", 33);
    return 0;
}
