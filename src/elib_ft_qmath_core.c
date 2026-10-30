/* elib_ft_qmath_core.c - FunctionTool Qm.n Fixed-Point Arithmetic */

#include "elib_ft.h"

/* n range: signed types [0, bitwidth-2], unsigned types [0, bitwidth-1].
 * Invalid n is a defined fallback everywhere: mul/div/from_double
 * return 0, convert/floor/ceil/round return q unchanged. */
#define Q_N_VALID_S(n, bits)  ((n) >= 0 && (n) <= (bits) - 2)
#define Q_N_VALID_U(n, bits)  ((n) >= 0 && (n) <= (bits) - 1)

/* ------------------------------------------------------------------ */
/*  Portable 128-bit helpers (used when __int128 is unavailable)        */
/* ------------------------------------------------------------------ */

#if !defined(__SIZEOF_INT128__)

/* full unsigned 64x64 -> 128-bit multiply (hi:lo) */
static void qmath_umul64_128(uint64_t a, uint64_t b, uint64_t *hi, uint64_t *lo)
{
    uint64_t a_lo = a & 0xFFFFFFFFu;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = b & 0xFFFFFFFFu;
    uint64_t b_hi = b >> 32;

    uint64_t p0 = a_lo * b_lo;   /* fits 64 bits */
    uint64_t p1 = a_lo * b_hi;   /* fits 63-bit magnitude range */
    uint64_t p2 = a_hi * b_lo;
    uint64_t p3 = a_hi * b_hi;

    /* product = p3*2^64 + (p1+p2)*2^32 + p0; split p1+p2 at 2^32 */
    uint64_t mid = (p1 & 0xFFFFFFFFu) + (p2 & 0xFFFFFFFFu);
    *hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
    *lo = p0 + (mid << 32);
    if (*lo < p0) {
        *hi += 1u;
    }
}

/* divide (hi:lo) by d, restoring long division on a 128-bit remainder.
 * Requires hi < d so the quotient fits 64 bits; returns quotient,
 * remainder via *rem. */
static uint64_t qmath_udiv128_64(uint64_t hi, uint64_t lo, uint64_t d, uint64_t *rem)
{
    uint64_t q = 0;
    uint64_t rh = 0;   /* remainder high word */
    uint64_t rl = 0;   /* remainder low word */

    for (int i = 127; i >= 0; i--) {
        uint32_t bit = (i >= 64)
            ? (uint32_t)((hi >> (uint32_t)(i - 64)) & 1u)
            : (uint32_t)((lo >> (uint32_t)i) & 1u);

        /* remainder <<= 1 | bit (128-bit shift): the incoming bit
         * enters the least significant bit of the low word, and the
         * OLD low word's top bit carries into the high word */
        uint64_t carry = rl >> 63;
        rl = (rl << 1) | (uint64_t)bit;
        rh = (rh << 1) | carry;

        /* if remainder >= d, subtract and set quotient bit */
        if (rh != 0 || rl >= d) {
            uint64_t before = rl;
            rl -= d;
            if (rl > before) {
                rh -= 1u;
            }
            q = (q << 1) | 1u;
        } else {
            q <<= 1;
        }
    }

    *rem = rl;
    return q;
}

/* mag scaled by 2^n as 128-bit (n < 64): hi = mag >> (64-n), lo = mag << n */
static void qmath_scale128(uint64_t mag, int n, uint64_t *hi, uint64_t *lo)
{
    if (n == 0) {
        *hi = 0;
        *lo = mag;
    } else {
        *hi = mag >> (uint32_t)(64 - n);
        *lo = mag << (uint32_t)n;
    }
}

#endif /* !__SIZEOF_INT128__ */

/* ------------------------------------------------------------------ */
/*  Saturating from_double helpers                                     */
/* ------------------------------------------------------------------ */

static int16_t qmath_sat16(double val, double scale)
{
    double r = val * scale;
    if (r > (double)INT16_MAX) return INT16_MAX;
    if (r < (double)INT16_MIN) return INT16_MIN;
    return (int16_t)r;
}

static uint16_t qmath_sat16u(double val, double scale)
{
    double r = val * scale;
    if (r > (double)UINT16_MAX) return UINT16_MAX;
    if (r < 0.0) return 0;
    return (uint16_t)r;
}

static int32_t qmath_sat32(double val, double scale)
{
    double r = val * scale;
    if (r > (double)INT32_MAX) return INT32_MAX;
    if (r < (double)INT32_MIN) return INT32_MIN;
    return (int32_t)r;
}

static uint32_t qmath_sat32u(double val, double scale)
{
    double r = val * scale;
    if (r > (double)UINT32_MAX) return UINT32_MAX;
    if (r < 0.0) return 0;
    return (uint32_t)r;
}

/* INT64_MAX/UINT64_MAX are not exactly representable in double: use
 * the exact power-of-two upper bounds 2^63 / 2^64 as clamp limits. */
#define QMATH_DOUBLE_2POW63  9.223372036854775808e18
#define QMATH_DOUBLE_2POW64  1.8446744073709551616e19

static int64_t qmath_sat64(double val, double scale)
{
    double r = val * scale;
    if (r >= QMATH_DOUBLE_2POW63) return INT64_MAX;
    if (r <= -QMATH_DOUBLE_2POW63) return INT64_MIN;
    return (int64_t)r;
}

static uint64_t qmath_sat64u(double val, double scale)
{
    double r = val * scale;
    if (r >= QMATH_DOUBLE_2POW64) return UINT64_MAX;
    if (r < 0.0) return 0;
    return (uint64_t)r;
}

/* ------------------------------------------------------------------ */
/*  Q16 - Signed 16-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int16_t elib_ft_q16_from_double(double val, int n)
{
    if (!Q_N_VALID_S(n, 16)) {
        return 0;
    }
    return qmath_sat16(val, (double)((int32_t)1 << n));
}

double elib_ft_q16_to_double(int16_t q, int n)
{
    if (!Q_N_VALID_S(n, 16)) {
        return 0.0;
    }
    return (double)q / (double)((int32_t)1 << n);
}

int16_t elib_ft_q16_from_float(float val, int n)
{
    return elib_ft_q16_from_double((double)val, n);
}

float elib_ft_q16_to_float(int16_t q, int n)
{
    return (float)elib_ft_q16_to_double(q, n);
}

int16_t elib_ft_q16_add(int16_t a, int16_t b)
{
    return (int16_t)(a + b);
}

int16_t elib_ft_q16_sub(int16_t a, int16_t b)
{
    return (int16_t)(a - b);
}

int16_t elib_ft_q16_mul(int16_t a, int16_t b, int n)
{
    if (!Q_N_VALID_S(n, 16)) {
        return 0;
    }
    return (int16_t)(((int32_t)a * (int32_t)b) >> n);
}

int16_t elib_ft_q16_div(int16_t a, int16_t b, int n)
{
    if (b == 0 || !Q_N_VALID_S(n, 16)) {
        return 0;
    }
    return (int16_t)(((int32_t)a << n) / (int32_t)b);
}

elib_ft_err_t elib_ft_q16_add_s(int16_t a, int16_t b, int16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if ((b > 0 && a > INT16_MAX - b) || (b < 0 && a < INT16_MIN - b)) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int16_t)(a + b);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q16_sub_s(int16_t a, int16_t b, int16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if ((b > 0 && a < INT16_MIN + b) || (b < 0 && a > INT16_MAX + b)) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int16_t)(a - b);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q16_mul_s(int16_t a, int16_t b, int n, int16_t *out)
{
    if (out == NULL || !Q_N_VALID_S(n, 16)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    int32_t r = (int32_t)a * (int32_t)b;
    r >>= n;
    if (r > INT16_MAX || r < INT16_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int16_t)r;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q16_div_s(int16_t a, int16_t b, int n, int16_t *out)
{
    if (out == NULL || !Q_N_VALID_S(n, 16)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    int32_t shifted = (int32_t)a << n;
    int32_t r = shifted / (int32_t)b;
    if (r > INT16_MAX || r < INT16_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int16_t)r;
    return ELIB_FT_OK;
}

int16_t elib_ft_q16_convert(int16_t q, int n_from, int n_to)
{
    if (!Q_N_VALID_S(n_from, 16) || !Q_N_VALID_S(n_to, 16)) {
        return q;
    }
    if (n_to > n_from) {
        return (int16_t)(q << (n_to - n_from));
    }
    if (n_to < n_from) {
        return (int16_t)(q >> (n_from - n_to));
    }
    return q;
}

int16_t elib_ft_q16_floor(int16_t q, int n)
{
    if (!Q_N_VALID_S(n, 16) || n == 0) {
        return q;
    }
    int16_t mask = (int16_t)(((int32_t)1 << n) - 1);
    int16_t frac = q & mask;
    int16_t result = q & (int16_t)~mask;
    if (q < 0 && frac != 0) {
        result = (int16_t)(result - (int16_t)((int32_t)1 << n));
    }
    return result;
}

int16_t elib_ft_q16_ceil(int16_t q, int n)
{
    if (!Q_N_VALID_S(n, 16) || n == 0) {
        return q;
    }
    int16_t mask = (int16_t)(((int32_t)1 << n) - 1);
    int16_t frac = q & mask;
    int16_t result = q & (int16_t)~mask;
    if (frac != 0 && q > 0) {
        result = (int16_t)(result + (int16_t)((int32_t)1 << n));
    }
    return result;
}

int16_t elib_ft_q16_round(int16_t q, int n)
{
    if (!Q_N_VALID_S(n, 16) || n == 0) {
        return q;
    }
    return (int16_t)((q + (int16_t)((int32_t)1 << (n - 1))) & (int16_t)~(((int32_t)1 << n) - 1));
}

int16_t elib_ft_q16_abs(int16_t q)
{
    return (q >= 0) ? q : (int16_t)(-(int)q);
}

/* ------------------------------------------------------------------ */
/*  UQ16 - Unsigned 16-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint16_t elib_ft_uq16_from_double(double val, int n)
{
    if (!Q_N_VALID_U(n, 16)) {
        return 0;
    }
    return qmath_sat16u(val, (double)((uint32_t)1 << n));
}

double elib_ft_uq16_to_double(uint16_t q, int n)
{
    if (!Q_N_VALID_U(n, 16)) {
        return 0.0;
    }
    return (double)q / (double)((uint32_t)1 << n);
}

uint16_t elib_ft_uq16_add(uint16_t a, uint16_t b)
{
    return (uint16_t)(a + b);
}

uint16_t elib_ft_uq16_sub(uint16_t a, uint16_t b)
{
    return (uint16_t)(a - b);
}

uint16_t elib_ft_uq16_mul(uint16_t a, uint16_t b, int n)
{
    if (!Q_N_VALID_U(n, 16)) {
        return 0;
    }
    return (uint16_t)(((uint32_t)a * (uint32_t)b) >> n);
}

uint16_t elib_ft_uq16_div(uint16_t a, uint16_t b, int n)
{
    if (b == 0 || !Q_N_VALID_U(n, 16)) {
        return 0;
    }
    return (uint16_t)(((uint32_t)a << n) / (uint32_t)b);
}

elib_ft_err_t elib_ft_uq16_add_s(uint16_t a, uint16_t b, uint16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (a > UINT16_MAX - b) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint16_t)(a + b);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq16_sub_s(uint16_t a, uint16_t b, uint16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (a < b) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint16_t)(a - b);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq16_mul_s(uint16_t a, uint16_t b, int n, uint16_t *out)
{
    if (out == NULL || !Q_N_VALID_U(n, 16)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    uint32_t r = (uint32_t)a * (uint32_t)b;
    r >>= n;
    if (r > UINT16_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint16_t)r;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq16_div_s(uint16_t a, uint16_t b, int n, uint16_t *out)
{
    if (out == NULL || !Q_N_VALID_U(n, 16)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    uint32_t shifted = (uint32_t)a << n;
    uint32_t r = shifted / (uint32_t)b;
    if (r > UINT16_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint16_t)r;
    return ELIB_FT_OK;
}

uint16_t elib_ft_uq16_convert(uint16_t q, int n_from, int n_to)
{
    if (!Q_N_VALID_U(n_from, 16) || !Q_N_VALID_U(n_to, 16)) {
        return q;
    }
    if (n_to > n_from) {
        return (uint16_t)(q << (n_to - n_from));
    }
    if (n_to < n_from) {
        return (uint16_t)(q >> (n_from - n_to));
    }
    return q;
}

/* ------------------------------------------------------------------ */
/*  Q32 - Signed 32-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int32_t elib_ft_q32_from_double(double val, int n)
{
    if (!Q_N_VALID_S(n, 32)) {
        return 0;
    }
    return qmath_sat32(val, (double)((int64_t)1 << n));
}

double elib_ft_q32_to_double(int32_t q, int n)
{
    if (!Q_N_VALID_S(n, 32)) {
        return 0.0;
    }
    return (double)q / (double)((int64_t)1 << n);
}

int32_t elib_ft_q32_from_float(float val, int n)
{
    return elib_ft_q32_from_double((double)val, n);
}

float elib_ft_q32_to_float(int32_t q, int n)
{
    return (float)elib_ft_q32_to_double(q, n);
}

int32_t elib_ft_q32_add(int32_t a, int32_t b)
{
    return a + b;
}

int32_t elib_ft_q32_sub(int32_t a, int32_t b)
{
    return a - b;
}

int32_t elib_ft_q32_mul(int32_t a, int32_t b, int n)
{
    if (!Q_N_VALID_S(n, 32)) {
        return 0;
    }
    return (int32_t)(((int64_t)a * (int64_t)b) >> n);
}

int32_t elib_ft_q32_div(int32_t a, int32_t b, int n)
{
    if (b == 0 || !Q_N_VALID_S(n, 32)) {
        return 0;
    }
    return (int32_t)(((int64_t)a << n) / (int64_t)b);
}

elib_ft_err_t elib_ft_q32_add_s(int32_t a, int32_t b, int32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if ((b > 0 && a > INT32_MAX - b) || (b < 0 && a < INT32_MIN - b)) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a + b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q32_sub_s(int32_t a, int32_t b, int32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if ((b > 0 && a < INT32_MIN + b) || (b < 0 && a > INT32_MAX + b)) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a - b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q32_mul_s(int32_t a, int32_t b, int n, int32_t *out)
{
    if (out == NULL || !Q_N_VALID_S(n, 32)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    int64_t r = (int64_t)a * (int64_t)b;
    r >>= n;
    if (r > INT32_MAX || r < INT32_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int32_t)r;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q32_div_s(int32_t a, int32_t b, int n, int32_t *out)
{
    if (out == NULL || !Q_N_VALID_S(n, 32)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    int64_t shifted = (int64_t)a << n;
    int64_t r = shifted / (int64_t)b;
    if (r > INT32_MAX || r < INT32_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int32_t)r;
    return ELIB_FT_OK;
}

int32_t elib_ft_q32_convert(int32_t q, int n_from, int n_to)
{
    if (!Q_N_VALID_S(n_from, 32) || !Q_N_VALID_S(n_to, 32)) {
        return q;
    }
    if (n_to > n_from) {
        return q << (n_to - n_from);
    }
    if (n_to < n_from) {
        return q >> (n_from - n_to);
    }
    return q;
}

int32_t elib_ft_q32_floor(int32_t q, int n)
{
    if (!Q_N_VALID_S(n, 32) || n == 0) {
        return q;
    }
    int32_t mask = ((int32_t)1 << n) - 1;
    int32_t frac = q & mask;
    int32_t result = q & ~mask;
    if (q < 0 && frac != 0) {
        result -= ((int32_t)1 << n);
    }
    return result;
}

int32_t elib_ft_q32_ceil(int32_t q, int n)
{
    if (!Q_N_VALID_S(n, 32) || n == 0) {
        return q;
    }
    int32_t mask = ((int32_t)1 << n) - 1;
    int32_t frac = q & mask;
    int32_t result = q & ~mask;
    if (frac != 0 && q > 0) {
        result += ((int32_t)1 << n);
    }
    return result;
}

int32_t elib_ft_q32_round(int32_t q, int n)
{
    if (!Q_N_VALID_S(n, 32) || n == 0) {
        return q;
    }
    return (q + ((int32_t)1 << (n - 1))) & ~(((int32_t)1 << n) - 1);
}

int32_t elib_ft_q32_abs(int32_t q)
{
    return (q >= 0) ? q : (int32_t)(0u - (uint32_t)q);
}

/* ------------------------------------------------------------------ */
/*  UQ32 - Unsigned 32-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint32_t elib_ft_uq32_from_double(double val, int n)
{
    if (!Q_N_VALID_U(n, 32)) {
        return 0;
    }
    return qmath_sat32u(val, (double)((uint64_t)1 << n));
}

double elib_ft_uq32_to_double(uint32_t q, int n)
{
    if (!Q_N_VALID_U(n, 32)) {
        return 0.0;
    }
    return (double)q / (double)((uint64_t)1 << n);
}

uint32_t elib_ft_uq32_add(uint32_t a, uint32_t b)
{
    return a + b;
}

uint32_t elib_ft_uq32_sub(uint32_t a, uint32_t b)
{
    return a - b;
}

uint32_t elib_ft_uq32_mul(uint32_t a, uint32_t b, int n)
{
    if (!Q_N_VALID_U(n, 32)) {
        return 0;
    }
    return (uint32_t)(((uint64_t)a * (uint64_t)b) >> n);
}

uint32_t elib_ft_uq32_div(uint32_t a, uint32_t b, int n)
{
    if (b == 0 || !Q_N_VALID_U(n, 32)) {
        return 0;
    }
    return (uint32_t)(((uint64_t)a << n) / (uint64_t)b);
}

elib_ft_err_t elib_ft_uq32_add_s(uint32_t a, uint32_t b, uint32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (a > UINT32_MAX - b) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a + b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq32_sub_s(uint32_t a, uint32_t b, uint32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (a < b) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a - b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq32_mul_s(uint32_t a, uint32_t b, int n, uint32_t *out)
{
    if (out == NULL || !Q_N_VALID_U(n, 32)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    uint64_t r = (uint64_t)a * (uint64_t)b;
    r >>= n;
    if (r > UINT32_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint32_t)r;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq32_div_s(uint32_t a, uint32_t b, int n, uint32_t *out)
{
    if (out == NULL || !Q_N_VALID_U(n, 32)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    uint64_t shifted = (uint64_t)a << n;
    uint64_t r = shifted / (uint64_t)b;
    if (r > UINT32_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint32_t)r;
    return ELIB_FT_OK;
}

uint32_t elib_ft_uq32_convert(uint32_t q, int n_from, int n_to)
{
    if (!Q_N_VALID_U(n_from, 32) || !Q_N_VALID_U(n_to, 32)) {
        return q;
    }
    if (n_to > n_from) {
        return q << (n_to - n_from);
    }
    if (n_to < n_from) {
        return q >> (n_from - n_to);
    }
    return q;
}

/* ------------------------------------------------------------------ */
/*  Q64 - Signed 64-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int64_t elib_ft_q64_from_double(double val, int n)
{
    if (!Q_N_VALID_S(n, 64)) {
        return 0;
    }
    return qmath_sat64(val, (double)((int64_t)1 << n));
}

double elib_ft_q64_to_double(int64_t q, int n)
{
    if (!Q_N_VALID_S(n, 64)) {
        return 0.0;
    }
    return (double)q / (double)((int64_t)1 << n);
}

int64_t elib_ft_q64_add(int64_t a, int64_t b)
{
    return a + b;
}

int64_t elib_ft_q64_sub(int64_t a, int64_t b)
{
    return a - b;
}

int64_t elib_ft_q64_mul(int64_t a, int64_t b, int n)
{
    if (!Q_N_VALID_S(n, 64)) {
        return 0;
    }
#if defined(__SIZEOF_INT128__)
    __int128 r = (__int128)a * (__int128)b;
    return (int64_t)(r >> n);
#else
    int neg = ((a < 0) != (b < 0));
    uint64_t maga = (a < 0) ? (0u - (uint64_t)a) : (uint64_t)a;
    uint64_t magb = (b < 0) ? (0u - (uint64_t)b) : (uint64_t)b;

    uint64_t hi, lo;
    qmath_umul64_128(maga, magb, &hi, &lo);

    /* Unsigned shift of the magnitude, sign folded back afterwards so
     * the low 64 bits match the __int128 path exactly: for x < 0,
     * floor(x / 2^n) == -ceil(|x| / 2^n). */
    uint64_t lo_s, rem;
    if (n == 0) {
        lo_s = lo;
        rem = 0;
    } else {
        lo_s = (lo >> (uint32_t)n) | (hi << (uint32_t)(64 - n));
        rem = lo & ((((uint64_t)1) << n) - 1);
    }
    if (neg && rem != 0) {
        lo_s += 1u;
    }
    return (int64_t)(neg ? ((uint64_t)0 - lo_s) : lo_s);
#endif
}

int64_t elib_ft_q64_div(int64_t a, int64_t b, int n)
{
    if (b == 0 || !Q_N_VALID_S(n, 64)) {
        return 0;
    }
#if defined(__SIZEOF_INT128__)
    return ((__int128)a << n) / (__int128)b;
#else
    int neg = ((a < 0) != (b < 0));
    uint64_t maga = (a < 0) ? (0u - (uint64_t)a) : (uint64_t)a;
    uint64_t magb = (b < 0) ? (0u - (uint64_t)b) : (uint64_t)b;

    uint64_t hi, lo;
    qmath_scale128(maga, n, &hi, &lo);

    /* quotient may not fit 64 bits: report the low word (documented
     * wide-domain fallback) */
    uint64_t rem = 0;
    uint64_t q = qmath_udiv128_64(hi, lo, magb, &rem);

    return neg ? (int64_t)(0u - q) : (int64_t)q;
#endif
}

elib_ft_err_t elib_ft_q64_add_s(int64_t a, int64_t b, int64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if ((b > 0 && a > INT64_MAX - b) || (b < 0 && a < INT64_MIN - b)) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a + b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q64_sub_s(int64_t a, int64_t b, int64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if ((b > 0 && a < INT64_MIN + b) || (b < 0 && a > INT64_MAX + b)) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a - b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_q64_mul_s(int64_t a, int64_t b, int n, int64_t *out)
{
    if (out == NULL || !Q_N_VALID_S(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__SIZEOF_INT128__)
    __int128 r = (__int128)a * (__int128)b;
    r >>= n;
    if (r > INT64_MAX || r < INT64_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int64_t)r;
    return ELIB_FT_OK;
#else
    int neg = ((a < 0) != (b < 0));
    uint64_t maga = (a < 0) ? (0u - (uint64_t)a) : (uint64_t)a;
    uint64_t magb = (b < 0) ? (0u - (uint64_t)b) : (uint64_t)b;

    uint64_t hi, lo;
    qmath_umul64_128(maga, magb, &hi, &lo);

    /* |result| = (|a*b| >> n), plus 1 when a negative product has
     * sticky low bits (matches the arithmetic shift of the __int128
     * path exactly). The magnitude must fit the signed range. */
    uint64_t t_hi, t_lo, rem;
    if (n == 0) {
        t_hi = hi;
        t_lo = lo;
        rem = 0;
    } else {
        t_hi = hi >> (uint32_t)n;
        t_lo = (lo >> (uint32_t)n) | (hi << (uint32_t)(64 - n));
        rem = lo & ((((uint64_t)1) << n) - 1);
    }
    if (neg && rem != 0) {
        uint64_t before = t_lo;
        t_lo += 1u;
        if (t_lo < before) {
            t_hi += 1u;
        }
    }

    if (t_hi != 0) {
        return ELIB_FT_ERR_EXCEED_MAX;   /* |result| >= 2^64 */
    }
    if (neg) {
        if (t_lo > 0x8000000000000000ull) {
            return ELIB_FT_ERR_EXCEED_MAX;
        }
        /* t_lo == 2^63 maps to INT64_MIN */
        *out = (t_lo == 0x8000000000000000ull) ? INT64_MIN : (int64_t)((uint64_t)0 - t_lo);
    } else {
        if (t_lo > (uint64_t)INT64_MAX) {
            return ELIB_FT_ERR_EXCEED_MAX;
        }
        *out = (int64_t)t_lo;
    }
    return ELIB_FT_OK;
#endif
}

elib_ft_err_t elib_ft_q64_div_s(int64_t a, int64_t b, int n, int64_t *out)
{
    if (out == NULL || !Q_N_VALID_S(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__SIZEOF_INT128__)
    __int128 shifted = (__int128)a << n;
    __int128 r = shifted / (__int128)b;
    if (r > INT64_MAX || r < INT64_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int64_t)r;
    return ELIB_FT_OK;
#else
    int neg = ((a < 0) != (b < 0));
    uint64_t maga = (a < 0) ? (0u - (uint64_t)a) : (uint64_t)a;
    uint64_t magb = (b < 0) ? (0u - (uint64_t)b) : (uint64_t)b;

    uint64_t hi, lo;
    qmath_scale128(maga, n, &hi, &lo);

    if (hi >= magb) {
        return ELIB_FT_ERR_EXCEED_MAX;   /* |quotient| >= 2^64 */
    }

    uint64_t rem = 0;
    uint64_t q = qmath_udiv128_64(hi, lo, magb, &rem);

    /* |quotient| must fit the signed range */
    if (neg) {
        if (q > 0x8000000000000000ull) {
            return ELIB_FT_ERR_EXCEED_MAX;
        }
        *out = (int64_t)(0u - q);        /* q == 2^63 maps to INT64_MIN */
    } else {
        if (q > (uint64_t)INT64_MAX) {
            return ELIB_FT_ERR_EXCEED_MAX;
        }
        *out = (int64_t)q;
    }
    return ELIB_FT_OK;
#endif
}

int64_t elib_ft_q64_convert(int64_t q, int n_from, int n_to)
{
    if (!Q_N_VALID_S(n_from, 64) || !Q_N_VALID_S(n_to, 64)) {
        return q;
    }
    if (n_to > n_from) {
        return q << (n_to - n_from);
    }
    if (n_to < n_from) {
        return q >> (n_from - n_to);
    }
    return q;
}

int64_t elib_ft_q64_floor(int64_t q, int n)
{
    if (!Q_N_VALID_S(n, 64) || n == 0) {
        return q;
    }
    int64_t mask = ((int64_t)1 << n) - 1;
    int64_t frac = q & mask;
    int64_t result = q & ~mask;
    if (q < 0 && frac != 0) {
        result -= ((int64_t)1 << n);
    }
    return result;
}

int64_t elib_ft_q64_ceil(int64_t q, int n)
{
    if (!Q_N_VALID_S(n, 64) || n == 0) {
        return q;
    }
    int64_t mask = ((int64_t)1 << n) - 1;
    int64_t frac = q & mask;
    int64_t result = q & ~mask;
    if (frac != 0 && q > 0) {
        result += ((int64_t)1 << n);
    }
    return result;
}

int64_t elib_ft_q64_round(int64_t q, int n)
{
    if (!Q_N_VALID_S(n, 64) || n == 0) {
        return q;
    }
    return (q + ((int64_t)1 << (n - 1))) & ~(((int64_t)1 << n) - 1);
}

int64_t elib_ft_q64_abs(int64_t q)
{
    return (q >= 0) ? q : (int64_t)(0u - (uint64_t)q);
}

/* ------------------------------------------------------------------ */
/*  UQ64 - Unsigned 64-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint64_t elib_ft_uq64_from_double(double val, int n)
{
    if (!Q_N_VALID_U(n, 64)) {
        return 0;
    }
    return qmath_sat64u(val, (double)((uint64_t)1 << n));
}

double elib_ft_uq64_to_double(uint64_t q, int n)
{
    if (!Q_N_VALID_U(n, 64)) {
        return 0.0;
    }
    return (double)q / (double)((uint64_t)1 << n);
}

uint64_t elib_ft_uq64_add(uint64_t a, uint64_t b)
{
    return a + b;
}

uint64_t elib_ft_uq64_sub(uint64_t a, uint64_t b)
{
    return a - b;
}

uint64_t elib_ft_uq64_mul(uint64_t a, uint64_t b, int n)
{
    if (!Q_N_VALID_U(n, 64)) {
        return 0;
    }
#if defined(__SIZEOF_INT128__)
    unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
    return (uint64_t)(r >> n);
#else
    uint64_t hi, lo;
    qmath_umul64_128(a, b, &hi, &lo);

    if (n == 0) {
        return lo;
    }
    return (lo >> (uint32_t)n) | (hi << (uint32_t)(64 - n));
#endif
}

uint64_t elib_ft_uq64_div(uint64_t a, uint64_t b, int n)
{
    if (b == 0 || !Q_N_VALID_U(n, 64)) {
        return 0;
    }
#if defined(__SIZEOF_INT128__)
    return ((unsigned __int128)a << n) / (unsigned __int128)b;
#else
    uint64_t hi, lo;
    qmath_scale128(a, n, &hi, &lo);

    uint64_t rem = 0;
    return qmath_udiv128_64(hi, lo, b, &rem);
#endif
}

elib_ft_err_t elib_ft_uq64_add_s(uint64_t a, uint64_t b, uint64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (a > UINT64_MAX - b) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a + b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq64_sub_s(uint64_t a, uint64_t b, uint64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (a < b) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = a - b;
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_uq64_mul_s(uint64_t a, uint64_t b, int n, uint64_t *out)
{
    if (out == NULL || !Q_N_VALID_U(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__SIZEOF_INT128__)
    unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
    r >>= n;
    if (r > UINT64_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint64_t)r;
    return ELIB_FT_OK;
#else
    uint64_t hi, lo;
    qmath_umul64_128(a, b, &hi, &lo);

    uint64_t rh, rl;
    if (n == 0) {
        rh = hi;
        rl = lo;
    } else {
        rh = hi >> (uint32_t)n;
        rl = (lo >> (uint32_t)n) | (hi << (uint32_t)(64 - n));
    }

    if (rh != 0) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = rl;
    return ELIB_FT_OK;
#endif
}

elib_ft_err_t elib_ft_uq64_div_s(uint64_t a, uint64_t b, int n, uint64_t *out)
{
    if (out == NULL || !Q_N_VALID_U(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__SIZEOF_INT128__)
    unsigned __int128 shifted = (unsigned __int128)a << n;
    unsigned __int128 r = shifted / (unsigned __int128)b;
    if (r > UINT64_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint64_t)r;
    return ELIB_FT_OK;
#else
    uint64_t hi, lo;
    qmath_scale128(a, n, &hi, &lo);

    if (hi >= b) {
        return ELIB_FT_ERR_EXCEED_MAX;   /* quotient >= 2^64 */
    }

    uint64_t rem = 0;
    *out = qmath_udiv128_64(hi, lo, b, &rem);
    return ELIB_FT_OK;
#endif
}

uint64_t elib_ft_uq64_convert(uint64_t q, int n_from, int n_to)
{
    if (!Q_N_VALID_U(n_from, 64) || !Q_N_VALID_U(n_to, 64)) {
        return q;
    }
    if (n_to > n_from) {
        return q << (n_to - n_from);
    }
    if (n_to < n_from) {
        return q >> (n_from - n_to);
    }
    return q;
}
