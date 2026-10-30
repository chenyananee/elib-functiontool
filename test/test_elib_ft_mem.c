/* test_elib_ft_mem.c - FunctionTool Bounded Memcpy/Memset Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_memcpy_normal(void)
{
    uint8_t src[] = {1, 2, 3, 4, 5};
    uint8_t dst[5] = {0};

    uint32_t ret = elib_ft_memcpy(dst, src, 5, 10);
    assert(ret == 5);
    assert(memcmp(dst, src, 5) == 0);
}

static void test_memcpy_truncate(void)
{
    uint8_t src[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t dst[8];
    memset(dst, 0xAA, sizeof(dst));

    uint32_t ret = elib_ft_memcpy(dst, src, 8, 4);
    assert(ret == 4);
    assert(memcmp(dst, src, 4) == 0);
    assert(dst[4] == 0xAA);
    assert(dst[5] == 0xAA);
}

static void test_memcpy_len_eq_max(void)
{
    uint8_t src[] = {10, 20, 30};
    uint8_t dst[3] = {0};

    uint32_t ret = elib_ft_memcpy(dst, src, 3, 3);
    assert(ret == 3);
    assert(memcmp(dst, src, 3) == 0);
}

static void test_memcpy_zero_len(void)
{
    uint8_t src[] = {1, 2, 3};
    uint8_t dst[3] = {0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memcpy(dst, src, 0, 10);
    assert(ret == 0);
    assert(dst[0] == 0xFF);
}

static void test_memcpy_zero_max(void)
{
    uint8_t src[] = {1, 2, 3};
    uint8_t dst[3] = {0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memcpy(dst, src, 3, 0);
    assert(ret == 0);
    assert(dst[0] == 0xFF);
}

static void test_memcpy_null_dst(void)
{
    uint8_t src[] = {1, 2, 3};

    uint32_t ret = elib_ft_memcpy(NULL, src, 3, 10);
    assert(ret == 0);
}

static void test_memcpy_null_src(void)
{
    uint8_t dst[3] = {0};

    uint32_t ret = elib_ft_memcpy(dst, NULL, 3, 10);
    assert(ret == 0);
}

static void test_memcpy_single_byte(void)
{
    uint8_t src = 0x42;
    uint8_t dst = 0;

    uint32_t ret = elib_ft_memcpy(&dst, &src, 1, 1);
    assert(ret == 1);
    assert(dst == 0x42);
}

static void test_memset_normal(void)
{
    uint8_t buf[8] = {0};

    uint32_t ret = elib_ft_memset(buf, 0xAB, 6, 10);
    assert(ret == 6);
    for (uint32_t i = 0; i < 6; i++) {
        assert(buf[i] == 0xAB);
    }
    assert(buf[6] == 0);
    assert(buf[7] == 0);
}

static void test_memset_truncate(void)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));

    uint32_t ret = elib_ft_memset(buf, 0xFF, 8, 3);
    assert(ret == 3);
    for (uint32_t i = 0; i < 3; i++) {
        assert(buf[i] == 0xFF);
    }
    assert(buf[3] == 0);
    assert(buf[4] == 0);
}

static void test_memset_len_eq_max(void)
{
    uint8_t buf[4] = {0};

    uint32_t ret = elib_ft_memset(buf, 0x55, 4, 4);
    assert(ret == 4);
    for (uint32_t i = 0; i < 4; i++) {
        assert(buf[i] == 0x55);
    }
}

static void test_memset_zero_len(void)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memset(buf, 0x00, 0, 10);
    assert(ret == 0);
    assert(buf[0] == 0xFF);
}

static void test_memset_zero_max(void)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memset(buf, 0x00, 4, 0);
    assert(ret == 0);
    assert(buf[0] == 0xFF);
}

static void test_memset_null_dst(void)
{
    uint32_t ret = elib_ft_memset(NULL, 0x00, 4, 10);
    assert(ret == 0);
}

static void test_memset_zero_value(void)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memset(buf, 0x00, 4, 10);
    assert(ret == 4);
    for (uint32_t i = 0; i < 4; i++) {
        assert(buf[i] == 0x00);
    }
}

static void test_memset_single_byte(void)
{
    uint8_t buf = 0;

    uint32_t ret = elib_ft_memset(&buf, 0x99, 1, 1);
    assert(ret == 1);
    assert(buf == 0x99);
}

int main(void)
{
    RUN_TEST(test_memcpy_normal);
    RUN_TEST(test_memcpy_truncate);
    RUN_TEST(test_memcpy_len_eq_max);
    RUN_TEST(test_memcpy_zero_len);
    RUN_TEST(test_memcpy_zero_max);
    RUN_TEST(test_memcpy_null_dst);
    RUN_TEST(test_memcpy_null_src);
    RUN_TEST(test_memcpy_single_byte);
    RUN_TEST(test_memset_normal);
    RUN_TEST(test_memset_truncate);
    RUN_TEST(test_memset_len_eq_max);
    RUN_TEST(test_memset_zero_len);
    RUN_TEST(test_memset_zero_max);
    RUN_TEST(test_memset_null_dst);
    RUN_TEST(test_memset_zero_value);
    RUN_TEST(test_memset_single_byte);

    printf("\nAll %d tests passed.\n", 16);
    return 0;
}
