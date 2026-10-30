/* test_elib_ft_err.c - FunctionTool Shared Error Codes Unit Tests (v2)
 *
 * The enum values are part of the library ABI: numeric order is
 * checked explicitly so accidental reordering is caught by any
 * byte-serializing or persisted usage of the codes.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_err_enum_stable(void)
{
    assert(ELIB_FT_OK == 0);
    assert(ELIB_FT_ERR_INVALID_PARAM == 1);
    assert(ELIB_FT_ERR_NOT_INITIALIZED == 2);
    assert(ELIB_FT_ERR_EXCEED_MAX == 3);
    assert(ELIB_FT_ERR_NO_SPACE == 4);
    assert(ELIB_FT_ERR_NO_DATA == 5);
}

static void test_err_type_size(void)
{
    /* enum is int-sized on every supported target */
    assert(sizeof(elib_ft_err_t) == sizeof(int));
}

static void test_version_macro(void)
{
    assert(ELIB_FT_VERSION[0] == '2');
    assert(ELIB_FT_VERSION[1] == '.');
    assert(ELIB_FT_VERSION[2] == '0');
    assert(ELIB_FT_VERSION[3] == '.');
    assert(ELIB_FT_VERSION[4] == '0');
}

int main(void)
{
    RUN_TEST(test_err_enum_stable);
    RUN_TEST(test_err_type_size);
    RUN_TEST(test_version_macro);

    printf("\nAll %d tests passed.\n", 3);
    return 0;
}
