/* test_elib_ft_util.c - FunctionTool Shortcut Macros Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_util_min_int(void)
{
    assert(ELIB_FT_MIN(3, 7) == 3);
    assert(ELIB_FT_MIN(7, 3) == 3);
    assert(ELIB_FT_MIN(-1, 2) == -1);
    assert(ELIB_FT_MIN(5, 5) == 5);
}

static void test_util_max_int(void)
{
    assert(ELIB_FT_MAX(3, 7) == 7);
    assert(ELIB_FT_MAX(7, 3) == 7);
    assert(ELIB_FT_MAX(-1, 2) == 2);
    assert(ELIB_FT_MAX(5, 5) == 5);
}

static void test_util_clamp(void)
{
    assert(ELIB_FT_CLAMP(5, 0, 10) == 5);
    assert(ELIB_FT_CLAMP(-1, 0, 10) == 0);
    assert(ELIB_FT_CLAMP(15, 0, 10) == 10);
    assert(ELIB_FT_CLAMP(0, 0, 10) == 0);
    assert(ELIB_FT_CLAMP(10, 0, 10) == 10);
}

static void test_util_map_int(void)
{
    /* 0-100 -> 0-1000: map(50) = 500 */
    assert(ELIB_FT_MAP(50, 0, 100, 0, 1000) == 500);
    /* 0-100 -> 0-1000: map(0) = 0 */
    assert(ELIB_FT_MAP(0, 0, 100, 0, 1000) == 0);
    /* 0-100 -> 0-1000: map(100) = 1000 */
    assert(ELIB_FT_MAP(100, 0, 100, 0, 1000) == 1000);
    /* 0-100 -> 200-300: map(50) = 250 */
    assert(ELIB_FT_MAP(50, 0, 100, 200, 300) == 250);
}

static void test_util_map_reverse(void)
{
    /* 0-100 -> 1000-0: reverse mapping, map(0)=1000, map(100)=0 */
    assert(ELIB_FT_MAP(0, 0, 100, 1000, 0) == 1000);
    assert(ELIB_FT_MAP(100, 0, 100, 1000, 0) == 0);
}

static void test_util_map_f(void)
{
    /* 0-100 -> 0-1.0: map_f(50) = 0.5 */
    double r = ELIB_FT_MAP_F(50, 0, 100, 0.0, 1.0);
    assert(r > 0.499 && r < 0.501);
    /* 0-100 -> 0-1.0: map_f(0) = 0.0 */
    r = ELIB_FT_MAP_F(0, 0, 100, 0.0, 1.0);
    assert(r > -0.001 && r < 0.001);
}

int main(void)
{
    RUN_TEST(test_util_min_int);
    RUN_TEST(test_util_max_int);
    RUN_TEST(test_util_clamp);
    RUN_TEST(test_util_map_int);
    RUN_TEST(test_util_map_reverse);
    RUN_TEST(test_util_map_f);

    printf("\nAll %d tests passed.\n", 6);
    return 0;
}
