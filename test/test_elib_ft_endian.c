/* test_elib_ft_endian.c - FunctionTool Byte Swap / Network Order Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_swap16(void)
{
    assert(elib_ft_swap16(0x1234) == 0x3412);
    assert(elib_ft_swap16(0x00FF) == 0xFF00);
    assert(elib_ft_swap16(0xABCD) == 0xCDAB);
}

static void test_swap32(void)
{
    assert(elib_ft_swap32(0x12345678) == 0x78563412);
    assert(elib_ft_swap32(0x11223344) == 0x44332211);
    assert(elib_ft_swap32(0xAABBCCDD) == 0xDDCCBBAABB);
}

static void test_swap32_zero(void)
{
    assert(elib_ft_swap32(0) == 0);
}

static void test_swap32_pattern(void)
{
    /* 0x01020304 -> 0x04030201 */
    assert(elib_ft_swap32(0x01020304) == 0x04030201);
}

int main(void)
{
    RUN_TEST(test_swap16);
    RUN_TEST(test_swap32);
    RUN_TEST(test_swap32_zero);
    RUN_TEST(test_swap32_pattern);

    printf("\nAll %d tests passed.\n", 4);
    return 0;
}
