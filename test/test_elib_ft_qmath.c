/* test_elib_ft_qmath.c - FunctionTool Qm.n Fixed-Point Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_q32_from_double_basic(void)
{
    int32_t q = elib_ft_q32_from_double(1.5, 16);
    assert(q == 98304);
}

static void test_q32_to_double_basic(void)
{
    double v = elib_ft_q32_to_double(98304, 16);
    assert(v > 1.499 && v < 1.501);
}

static void test_q32_roundtrip(void)
{
    int32_t q = elib_ft_q32_from_double(0.5, 31);
    double v = elib_ft_q32_to_double(q, 31);
    assert(v > 0.499 && v < 0.501);
}

static void test_q32_from_double_negative(void)
{
    assert(elib_ft_q32_from_double(-1.0, 16) == -65536);
}

static void test_q32_from_double_zero(void)
{
    assert(elib_ft_q32_from_double(0.0, 16) == 0);
}

static void test_q32_from_float_basic(void)
{
    assert(elib_ft_q32_from_float(1.5f, 16) == 98304);
}

static void test_q32_to_float_basic(void)
{
    float v = elib_ft_q32_to_float(98304, 16);
    assert(v > 1.499f && v < 1.501f);
}

static void test_q32_from_double_n0(void)
{
    assert(elib_ft_q32_from_double(42.0, 0) == 42);
}

static void test_q32_add_basic(void)
{
    int32_t a = elib_ft_q32_from_double(1.0, 16);
    int32_t b = elib_ft_q32_from_double(2.0, 16);
    assert(elib_ft_q32_add(a, b) == elib_ft_q32_from_double(3.0, 16));
}

static void test_q32_sub_basic(void)
{
    int32_t a = elib_ft_q32_from_double(3.0, 16);
    int32_t b = elib_ft_q32_from_double(1.0, 16);
    assert(elib_ft_q32_sub(a, b) == elib_ft_q32_from_double(2.0, 16));
}

static void test_q32_mul_basic(void)
{
    int32_t a = elib_ft_q32_from_double(2.0, 16);
    int32_t b = elib_ft_q32_from_double(3.0, 16);
    double v = elib_ft_q32_to_double(elib_ft_q32_mul(a, b, 16), 16);
    assert(v > 5.999 && v < 6.001);
}

static void test_q32_mul_fraction(void)
{
    int32_t a = elib_ft_q32_from_double(1.5, 16);
    int32_t b = elib_ft_q32_from_double(2.0, 16);
    double v = elib_ft_q32_to_double(elib_ft_q32_mul(a, b, 16), 16);
    assert(v > 2.999 && v < 3.001);
}

static void test_q32_div_basic(void)
{
    int32_t a = elib_ft_q32_from_double(6.0, 16);
    int32_t b = elib_ft_q32_from_double(2.0, 16);
    double v = elib_ft_q32_to_double(elib_ft_q32_div(a, b, 16), 16);
    assert(v > 2.999 && v < 3.001);
}

static void test_q32_div_fraction(void)
{
    int32_t a = elib_ft_q32_from_double(1.0, 16);
    int32_t b = elib_ft_q32_from_double(3.0, 16);
    double v = elib_ft_q32_to_double(elib_ft_q32_div(a, b, 16), 16);
    assert(v > 0.333 && v < 0.334);
}

static void test_q32_add_s_overflow(void)
{
    int32_t out;
    assert(elib_ft_q32_add_s(INT32_MAX, 1, &out) == ELIB_FT_ERR_EXCEED_MAX);
}

static void test_q32_sub_s_overflow(void)
{
    int32_t out;
    assert(elib_ft_q32_sub_s(INT32_MIN, 1, &out) == ELIB_FT_ERR_EXCEED_MAX);
}

static void test_q32_div_s_by_zero(void)
{
    int32_t out;
    assert(elib_ft_q32_div_s(100, 0, 16, &out) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_q32_add_s_ok(void)
{
    int32_t out;
    assert(elib_ft_q32_add_s(1, 2, &out) == ELIB_FT_OK);
    assert(out == 3);
}

static void test_q32_convert_up(void)
{
    int32_t q = 1 << 8;
    assert(elib_ft_q32_convert(q, 8, 12) == (1 << 12));
}

static void test_q32_convert_down(void)
{
    int32_t q = 1 << 12;
    assert(elib_ft_q32_convert(q, 12, 8) == (1 << 8));
}

static void test_q32_convert_same(void)
{
    assert(elib_ft_q32_convert(0x1234, 16, 16) == 0x1234);
}

static void test_q32_floor(void)
{
    int32_t q = elib_ft_q32_from_double(1.75, 16);
    assert(elib_ft_q32_floor(q, 16) == elib_ft_q32_from_double(1.0, 16));
}

static void test_q32_ceil(void)
{
    int32_t q = elib_ft_q32_from_double(1.25, 16);
    assert(elib_ft_q32_ceil(q, 16) == elib_ft_q32_from_double(2.0, 16));
}

static void test_q32_ceil_exact(void)
{
    int32_t q = elib_ft_q32_from_double(2.0, 16);
    assert(elib_ft_q32_ceil(q, 16) == q);
}

static void test_q32_round_basic(void)
{
    int32_t q = elib_ft_q32_from_double(1.5, 16);
    assert(elib_ft_q32_round(q, 16) == elib_ft_q32_from_double(2.0, 16));
}

static void test_q32_round_down(void)
{
    int32_t q = elib_ft_q32_from_double(1.4, 16);
    assert(elib_ft_q32_round(q, 16) == elib_ft_q32_from_double(1.0, 16));
}

static void test_q32_round_n0(void)
{
    assert(elib_ft_q32_round(42, 0) == 42);
}

static void test_q32_abs(void)
{
    assert(elib_ft_q32_abs(-5) == 5);
    assert(elib_ft_q32_abs(5) == 5);
    assert(elib_ft_q32_abs(0) == 0);
}

static void test_q32_floor_negative(void)
{
    int32_t q = elib_ft_q32_from_double(-1.25, 16);
    assert(elib_ft_q32_floor(q, 16) == elib_ft_q32_from_double(-2.0, 16));
}

static void test_uq32_from_double_basic(void) { assert(elib_ft_uq32_from_double(1.5, 16) == 98304u); }

static void test_uq32_to_double_basic(void) { double v = elib_ft_uq32_to_double(98304u, 16); assert(v > 1.499 && v < 1.501); }

static void test_uq32_add_basic(void) { uint32_t a = elib_ft_uq32_from_double(1.0, 16); uint32_t b = elib_ft_uq32_from_double(2.0, 16); assert(elib_ft_uq32_add(a, b) == elib_ft_uq32_from_double(3.0, 16)); }

static void test_uq32_sub_basic(void) { uint32_t a = elib_ft_uq32_from_double(3.0, 16); uint32_t b = elib_ft_uq32_from_double(1.0, 16); assert(elib_ft_uq32_sub(a, b) == elib_ft_uq32_from_double(2.0, 16)); }

static void test_uq32_mul_basic(void) { uint32_t a = elib_ft_uq32_from_double(2.0, 16); uint32_t b = elib_ft_uq32_from_double(3.0, 16); double v = elib_ft_uq32_to_double(elib_ft_uq32_mul(a, b, 16), 16); assert(v > 5.999 && v < 6.001); }

static void test_uq32_div_basic(void) { uint32_t a = elib_ft_uq32_from_double(6.0, 16); uint32_t b = elib_ft_uq32_from_double(2.0, 16); double v = elib_ft_uq32_to_double(elib_ft_uq32_div(a, b, 16), 16); assert(v > 2.999 && v < 3.001); }

static void test_uq32_add_s_overflow(void) { uint32_t out; assert(elib_ft_uq32_add_s(UINT32_MAX, 1, &out) == ELIB_FT_ERR_EXCEED_MAX); }

static void test_uq32_sub_s_underflow(void) { uint32_t out; assert(elib_ft_uq32_sub_s(0, 1, &out) == ELIB_FT_ERR_EXCEED_MAX); }

static void test_uq32_div_s_by_zero(void) { uint32_t out; assert(elib_ft_uq32_div_s(100, 0, 16, &out) == ELIB_FT_ERR_INVALID_PARAM); }

static void test_uq32_convert_up(void) { assert(elib_ft_uq32_convert(1u << 8, 8, 12) == 1u << 12); }

static void test_uq32_convert_down(void) { assert(elib_ft_uq32_convert(1u << 12, 12, 8) == 1u << 8); }

static void test_q16_from_double_basic(void) { assert(elib_ft_q16_from_double(0.5, 15) == 0x4000); }

static void test_q16_to_double_basic(void) { double v = elib_ft_q16_to_double(0x4000, 15); assert(v > 0.499 && v < 0.501); }

static void test_q16_mul_basic(void) { int16_t a = elib_ft_q16_from_double(0.5, 14); int16_t b = elib_ft_q16_from_double(0.5, 14); double v = elib_ft_q16_to_double(elib_ft_q16_mul(a, b, 14), 14); assert(v > 0.249 && v < 0.251); }

static void test_q16_div_basic(void) { int16_t a = elib_ft_q16_from_double(1.0, 14); int16_t b = elib_ft_q16_from_double(2.0, 14); double v = elib_ft_q16_to_double(elib_ft_q16_div(a, b, 14), 14); assert(v > 0.499 && v < 0.501); }

static void test_uq16_from_double_basic(void) { assert(elib_ft_uq16_from_double(0.5, 16) == 0x8000); }

static void test_uq16_mul_basic(void) { uint16_t a = elib_ft_uq16_from_double(0.5, 16); uint16_t b = elib_ft_uq16_from_double(0.5, 16); double v = elib_ft_uq16_to_double(elib_ft_uq16_mul(a, b, 16), 16); assert(v > 0.249 && v < 0.251); }

static void test_q64_from_double_basic(void) { assert(elib_ft_q64_from_double(1.5, 16) == 98304LL); }

static void test_q64_to_double_basic(void) { double v = elib_ft_q64_to_double(98304LL, 16); assert(v > 1.499 && v < 1.501); }

static void test_q64_mul_basic(void) { int64_t a = elib_ft_q64_from_double(2.0, 16); int64_t b = elib_ft_q64_from_double(3.0, 16); double v = elib_ft_q64_to_double(elib_ft_q64_mul(a, b, 16), 16); assert(v > 5.999 && v < 6.001); }

static void test_q64_div_basic(void) { int64_t a = elib_ft_q64_from_double(6.0, 16); int64_t b = elib_ft_q64_from_double(2.0, 16); double v = elib_ft_q64_to_double(elib_ft_q64_div(a, b, 16), 16); assert(v > 2.999 && v < 3.001); }

static void test_uq64_from_double_basic(void) { assert(elib_ft_uq64_from_double(1.5, 16) == 98304ULL); }

static void test_uq64_to_double_basic(void) { double v = elib_ft_uq64_to_double(98304ULL, 16); assert(v > 1.499 && v < 1.501); }

static void test_q_n_invalid_fast(void)
{
    /* invalid n on fast variants: defined fallbacks, never UB dispatch */
    assert(elib_ft_q16_mul(100, 100, 15) == 0);     /* signed 16: n <= 14 */
    assert(elib_ft_q16_mul(100, 100, -1) == 0);
    assert(elib_ft_q16_div(100, 2, 15) == 0);
    assert(elib_ft_q16_from_double(1.0, 15) == 0);
    assert(elib_ft_uq16_mul(100, 100, 17) == 0);    /* unsigned 16: n <= 15 */
    assert(elib_ft_uq16_from_double(1.0, 17) == 0);
    assert(elib_ft_q32_mul(100, 100, 31) == 0);     /* signed 32: n <= 30 */
    assert(elib_ft_uq32_mul(100, 100, 32) == 0);    /* unsigned 32: n <= 31 */
    assert(elib_ft_q64_mul(100, 100, 63) == 0);     /* signed 64: n <= 62 */
    assert(elib_ft_uq64_mul(100, 100, 64) == 0);    /* unsigned 64: n <= 63 */
    /* convert/floor/ceil/round: invalid n -> q unchanged */
    assert(elib_ft_q16_convert(1234, 15, 7) == 1234);
    assert(elib_ft_q32_floor(100, 32) == 100);
    assert(elib_ft_q64_round(1 << 22, 63) == (1 << 22));
}

static void test_q64_fallback_negative_fraction(void)
{
    /* negative products follow the arithmetic-shift (floor) semantics
     * of the __int128 path exactly */
    assert(elib_ft_q64_mul(1, -1, 1) == -1);
    assert(elib_ft_q64_mul(-5, 1, 1) == -3);        /* floor(-2.5) = -3 */
    assert(elib_ft_q64_mul(5, -1, 1) == -3);
    assert(elib_ft_q64_mul(5, 1, 1) == 2);          /* trunc(+2.5) = +2 */
    assert(elib_ft_q64_mul(-1, INT64_MIN, 0) == INT64_MIN);
}

static void test_q64_s_boundary(void)
{
    int64_t out = 0;

    assert(elib_ft_q64_mul_s(-5, 1, 1, &out) == ELIB_FT_OK && out == -3);
    assert(elib_ft_q64_mul_s(INT64_MIN, 1, 0, &out) == ELIB_FT_OK && out == INT64_MIN);
    assert(elib_ft_q64_mul_s(-(1LL << 62), 3, 0, &out) == ELIB_FT_ERR_EXCEED_MAX);
    assert(elib_ft_q64_mul_s(1LL << 62, 4, 0, &out) == ELIB_FT_ERR_EXCEED_MAX);
    assert(elib_ft_q64_mul_s(1, 1, 63, &out) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_q64_div_s(1, 0, 5, &out) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_q64_div_s(1, 2, 63, &out) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_q64_add_s(1, 2, NULL) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_saturate_from_double(void)
{
    /* out-of-range double input clamps, never out-of-range-casts */
    assert(elib_ft_q16_from_double(2.0, 15) == INT16_MAX);
    assert(elib_ft_q16_from_double(-2.0, 15) == INT16_MIN);
    assert(elib_ft_uq16_from_double(2.0, 16) == UINT16_MAX);
    assert(elib_ft_uq16_from_double(-1.0, 16) == 0);
    assert(elib_ft_q32_from_double(2.0, 31) == INT32_MAX);
    assert(elib_ft_uq32_from_double(2.0, 31) == UINT32_MAX);
    assert(elib_ft_uq64_from_double(-1.0, 32) == 0);
    /* documented abs(IntMin) passthrough */
    assert(elib_ft_q16_abs(INT16_MIN) == INT16_MIN);
    assert(elib_ft_q32_abs(INT32_MIN) == INT32_MIN);
    assert(elib_ft_q64_abs(INT64_MIN) == INT64_MIN);
    /* uq64_div_s: quotient >= 2^64 is rejected, exact fits succeed */
    uint64_t uout = 0;
    assert(elib_ft_uq64_div_s((uint64_t)1 << 63, 1, 62, &uout) == ELIB_FT_ERR_EXCEED_MAX);
    assert(elib_ft_uq64_div_s(5, 2, 1, &uout) == ELIB_FT_OK && uout == 5);
}

int main(void)
{
    RUN_TEST(test_q32_from_double_basic);
    RUN_TEST(test_q32_to_double_basic);
    RUN_TEST(test_q32_roundtrip);
    RUN_TEST(test_q32_from_double_negative);
    RUN_TEST(test_q32_from_double_zero);
    RUN_TEST(test_q32_from_float_basic);
    RUN_TEST(test_q32_to_float_basic);
    RUN_TEST(test_q32_from_double_n0);
    RUN_TEST(test_q32_add_basic);
    RUN_TEST(test_q32_sub_basic);
    RUN_TEST(test_q32_mul_basic);
    RUN_TEST(test_q32_mul_fraction);
    RUN_TEST(test_q32_div_basic);
    RUN_TEST(test_q32_div_fraction);
    RUN_TEST(test_q32_add_s_overflow);
    RUN_TEST(test_q32_sub_s_overflow);
    RUN_TEST(test_q32_div_s_by_zero);
    RUN_TEST(test_q32_add_s_ok);
    RUN_TEST(test_q32_convert_up);
    RUN_TEST(test_q32_convert_down);
    RUN_TEST(test_q32_convert_same);
    RUN_TEST(test_q32_floor);
    RUN_TEST(test_q32_ceil);
    RUN_TEST(test_q32_ceil_exact);
    RUN_TEST(test_q32_round_basic);
    RUN_TEST(test_q32_round_down);
    RUN_TEST(test_q32_round_n0);
    RUN_TEST(test_q32_abs);
    RUN_TEST(test_q32_floor_negative);
    RUN_TEST(test_uq32_from_double_basic);
    RUN_TEST(test_uq32_to_double_basic);
    RUN_TEST(test_uq32_add_basic);
    RUN_TEST(test_uq32_sub_basic);
    RUN_TEST(test_uq32_mul_basic);
    RUN_TEST(test_uq32_div_basic);
    RUN_TEST(test_uq32_add_s_overflow);
    RUN_TEST(test_uq32_sub_s_underflow);
    RUN_TEST(test_uq32_div_s_by_zero);
    RUN_TEST(test_uq32_convert_up);
    RUN_TEST(test_uq32_convert_down);
    RUN_TEST(test_q16_from_double_basic);
    RUN_TEST(test_q16_to_double_basic);
    RUN_TEST(test_q16_mul_basic);
    RUN_TEST(test_q16_div_basic);
    RUN_TEST(test_uq16_from_double_basic);
    RUN_TEST(test_uq16_mul_basic);
    RUN_TEST(test_q64_from_double_basic);
    RUN_TEST(test_q64_to_double_basic);
    RUN_TEST(test_q64_mul_basic);
    RUN_TEST(test_q64_div_basic);
    RUN_TEST(test_uq64_from_double_basic);
    RUN_TEST(test_uq64_to_double_basic);

    RUN_TEST(test_q_n_invalid_fast);
    RUN_TEST(test_q64_fallback_negative_fraction);
    RUN_TEST(test_q64_s_boundary);
    RUN_TEST(test_saturate_from_double);
    printf("\nAll %d tests passed.\n", 56);
    return 0;
}
