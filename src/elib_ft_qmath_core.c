/* elib_ft_qmath_core.c - FunctionTool Qm.n Fixed-Point Arithmetic */

#include "elib_ft_qmath.h"

#include <limits.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Helpers                                                            */
/* ------------------------------------------------------------------ */

/* n range: signed types [0, bitwidth-2], unsigned types [0, bitwidth-1] */
#define Q_N_VALID_S(n, bits)  ((n) >= 0 && (n) <= (bits) - 2)
#define Q_N_VALID_U(n, bits)  ((n) >= 0 && (n) <= (bits) - 1)

/* ------------------------------------------------------------------ */
/*  Q16 - Signed 16-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int16_t elib_ft_q16_from_double(double val, int n)
{
    if (!Q_N_VALID_S(n, 16)) {
        return 0;
    }
    return (int16_t)(val * (double)((int16_t)1 << n));
}

double elib_ft_q16_to_double(int16_t q, int n)
{
    if (!Q_N_VALID_S(n, 16)) {
        return 0.0;
    }
    return (double)q / (double)((int16_t)1 << n);
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
    return (int16_t)(((int32_t)a * (int32_t)b) >> n);
}

int16_t elib_ft_q16_div(int16_t a, int16_t b, int n)
{
    if (b == 0) {
        return 0;
    }
    return (int16_t)(((int32_t)a << n) / (int32_t)b);
}

int elib_ft_q16_add_s(int16_t a, int16_t b, int16_t *out)
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

int elib_ft_q16_sub_s(int16_t a, int16_t b, int16_t *out)
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

int elib_ft_q16_mul_s(int16_t a, int16_t b, int n, int16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_S(n, 16)) {
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

int elib_ft_q16_div_s(int16_t a, int16_t b, int n, int16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_S(n, 16)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    int32_t shifted = (int32_t)a << n;
    /* check if left-shift overflowed */
    if ((shifted >> n) != (int32_t)a) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    int32_t r = shifted / (int32_t)b;
    if (r > INT16_MAX || r < INT16_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int16_t)r;
    return ELIB_FT_OK;
}

int16_t elib_ft_q16_convert(int16_t q, int n_from, int n_to)
{
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
    if (n == 0) {
        return q;
    }
    int16_t mask = (int16_t)((int16_t)1 << n) - 1;
    int16_t frac = q & mask;
    int16_t result = q & ~mask;
    if (q < 0 && frac != 0) {
        result = (int16_t)(result - ((int16_t)1 << n));
    }
    return result;
}

int16_t elib_ft_q16_ceil(int16_t q, int n)
{
    if (n == 0) {
        return q;
    }
    int16_t mask = (int16_t)((int16_t)1 << n) - 1;
    int16_t frac = q & mask;
    int16_t result = q & ~mask;
    if (frac != 0 && q > 0) {
        result = (int16_t)(result + ((int16_t)1 << n));
    }
    return result;
}

int16_t elib_ft_q16_round(int16_t q, int n)
{
    if (n == 0) {
        return q;
    }
    return (int16_t)((q + ((int16_t)1 << (n - 1))) & ~(((int16_t)1 << n) - 1));
}

int16_t elib_ft_q16_abs(int16_t q)
{
    return (q < 0) ? (int16_t)(-q) : q;
}

/* ------------------------------------------------------------------ */
/*  UQ16 - Unsigned 16-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint16_t elib_ft_uq16_from_double(double val, int n)
{
    if (!Q_N_VALID_U(n, 16)) {
        return 0;
    }
    return (uint16_t)(val * (double)((uint16_t)1 << n));
}

double elib_ft_uq16_to_double(uint16_t q, int n)
{
    if (!Q_N_VALID_U(n, 16)) {
        return 0.0;
    }
    return (double)q / (double)((uint16_t)1 << n);
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
    return (uint16_t)(((uint32_t)a * (uint32_t)b) >> n);
}

uint16_t elib_ft_uq16_div(uint16_t a, uint16_t b, int n)
{
    if (b == 0) {
        return 0;
    }
    return (uint16_t)(((uint32_t)a << n) / (uint32_t)b);
}

int elib_ft_uq16_add_s(uint16_t a, uint16_t b, uint16_t *out)
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

int elib_ft_uq16_sub_s(uint16_t a, uint16_t b, uint16_t *out)
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

int elib_ft_uq16_mul_s(uint16_t a, uint16_t b, int n, uint16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_U(n, 16)) {
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

int elib_ft_uq16_div_s(uint16_t a, uint16_t b, int n, uint16_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_U(n, 16)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    uint32_t shifted = (uint32_t)a << n;
    /* check if left-shift overflowed */
    if ((shifted >> n) != (uint32_t)a) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    uint32_t r = shifted / (uint32_t)b;
    if (r > UINT16_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint16_t)r;
    return ELIB_FT_OK;
}

uint16_t elib_ft_uq16_convert(uint16_t q, int n_from, int n_to)
{
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
    return (int32_t)(val * (double)((int64_t)1 << n));
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
    return (int32_t)(((int64_t)a * (int64_t)b) >> n);
}

int32_t elib_ft_q32_div(int32_t a, int32_t b, int n)
{
    if (b == 0) {
        return 0;
    }
    return (int32_t)(((int64_t)a << n) / (int64_t)b);
}

int elib_ft_q32_add_s(int32_t a, int32_t b, int32_t *out)
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

int elib_ft_q32_sub_s(int32_t a, int32_t b, int32_t *out)
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

int elib_ft_q32_mul_s(int32_t a, int32_t b, int n, int32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_S(n, 32)) {
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

int elib_ft_q32_div_s(int32_t a, int32_t b, int n, int32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_S(n, 32)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    int64_t shifted = (int64_t)a << n;
    /* check if left-shift overflowed */
    if ((shifted >> n) != (int64_t)a) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    int64_t r = shifted / (int64_t)b;
    if (r > INT32_MAX || r < INT32_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int32_t)r;
    return ELIB_FT_OK;
}

int32_t elib_ft_q32_convert(int32_t q, int n_from, int n_to)
{
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
    if (n == 0) {
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
    if (n == 0) {
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
    if (n == 0) {
        return q;
    }
    return (q + ((int32_t)1 << (n - 1))) & ~(((int32_t)1 << n) - 1);
}

int32_t elib_ft_q32_abs(int32_t q)
{
    return (q < 0) ? -q : q;
}

/* ------------------------------------------------------------------ */
/*  UQ32 - Unsigned 32-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint32_t elib_ft_uq32_from_double(double val, int n)
{
    if (!Q_N_VALID_U(n, 32)) {
        return 0;
    }
    return (uint32_t)(val * (double)((uint64_t)1 << n));
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
    return (uint32_t)(((uint64_t)a * (uint64_t)b) >> n);
}

uint32_t elib_ft_uq32_div(uint32_t a, uint32_t b, int n)
{
    if (b == 0) {
        return 0;
    }
    return (uint32_t)(((uint64_t)a << n) / (uint64_t)b);
}

int elib_ft_uq32_add_s(uint32_t a, uint32_t b, uint32_t *out)
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

int elib_ft_uq32_sub_s(uint32_t a, uint32_t b, uint32_t *out)
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

int elib_ft_uq32_mul_s(uint32_t a, uint32_t b, int n, uint32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_U(n, 32)) {
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

int elib_ft_uq32_div_s(uint32_t a, uint32_t b, int n, uint32_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_U(n, 32)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    uint64_t shifted = (uint64_t)a << n;
    /* check if left-shift overflowed */
    if ((shifted >> n) != (uint64_t)a) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    uint64_t r = shifted / (uint64_t)b;
    if (r > UINT32_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint32_t)r;
    return ELIB_FT_OK;
}

uint32_t elib_ft_uq32_convert(uint32_t q, int n_from, int n_to)
{
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
    return (int64_t)(val * (double)((int64_t)1 << n));
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
#if defined(__GNUC__) || defined(__clang__)
    __int128 r = (__int128)a * (__int128)b;
    return (int64_t)(r >> n);
#else
    /* split: (a_hi*2^32 + a_lo) * (b_hi*2^32 + b_lo) */
    uint64_t a_lo = (uint64_t)a & 0xFFFFFFFFu;
    int64_t  a_hi = a >> 32;
    uint64_t b_lo = (uint64_t)b & 0xFFFFFFFFu;
    int64_t  b_hi = b >> 32;
    int64_t mid = (int64_t)(a_lo * b_lo >> 32) + a_hi * (int64_t)b_lo + (int64_t)a_lo * b_hi;
    int64_t r = (a_hi * b_hi << (64 - n)) + (mid >> n);
    return r;
#endif
}

int64_t elib_ft_q64_div(int64_t a, int64_t b, int n)
{
    if (b == 0) {
        return 0;
    }
#if defined(__GNUC__) || defined(__clang__)
    return ((__int128)a << n) / (__int128)b;
#else
    int64_t shifted = a << n;
    if ((shifted >> n) == a) {
        return shifted / b;
    }
    return (int64_t)((double)a / (double)b * (double)((int64_t)1 << n));
#endif
}

int elib_ft_q64_add_s(int64_t a, int64_t b, int64_t *out)
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

int elib_ft_q64_sub_s(int64_t a, int64_t b, int64_t *out)
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

int elib_ft_q64_mul_s(int64_t a, int64_t b, int n, int64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_S(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__GNUC__) || defined(__clang__)
    __int128 r = (__int128)a * (__int128)b;
    r >>= n;
    if (r > INT64_MAX || r < INT64_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int64_t)r;
    return ELIB_FT_OK;
#else
    /* approximate: use double for overflow check */
    double da = (double)a;
    double db = (double)b;
    double dr = da * db / (double)((int64_t)1 << n);
    if (dr > (double)INT64_MAX || dr < (double)INT64_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = elib_ft_q64_mul(a, b, n);
    return ELIB_FT_OK;
#endif
}

int elib_ft_q64_div_s(int64_t a, int64_t b, int n, int64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_S(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__GNUC__) || defined(__clang__)
    __int128 shifted = (__int128)a << n;
    __int128 r = shifted / (__int128)b;
    if (r > INT64_MAX || r < INT64_MIN) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (int64_t)r;
    return ELIB_FT_OK;
#else
    int64_t shifted = a << n;
    if ((shifted >> n) != a) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    int64_t r = shifted / b;
    *out = r;
    return ELIB_FT_OK;
#endif
}

int64_t elib_ft_q64_convert(int64_t q, int n_from, int n_to)
{
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
    if (n == 0) {
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
    if (n == 0) {
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
    if (n == 0) {
        return q;
    }
    return (q + ((int64_t)1 << (n - 1))) & ~(((int64_t)1 << n) - 1);
}

int64_t elib_ft_q64_abs(int64_t q)
{
    return (q < 0) ? -q : q;
}

/* ------------------------------------------------------------------ */
/*  UQ64 - Unsigned 64-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint64_t elib_ft_uq64_from_double(double val, int n)
{
    if (!Q_N_VALID_U(n, 64)) {
        return 0;
    }
    return (uint64_t)(val * (double)((uint64_t)1 << n));
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
#if defined(__GNUC__) || defined(__clang__)
    unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
    return (uint64_t)(r >> n);
#else
    /* split: (a_hi*2^32 + a_lo) * (b_hi*2^32 + b_lo) */
    uint64_t a_lo = a & 0xFFFFFFFFu;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = b & 0xFFFFFFFFu;
    uint64_t b_hi = b >> 32;
    uint64_t mid = (a_lo * b_lo >> 32) + a_hi * b_lo + a_lo * b_hi;
    uint64_t r = (a_hi * b_hi << (64 - n)) + (mid >> n);
    return r;
#endif
}

uint64_t elib_ft_uq64_div(uint64_t a, uint64_t b, int n)
{
    if (b == 0) {
        return 0;
    }
#if defined(__GNUC__) || defined(__clang__)
    return ((unsigned __int128)a << n) / (unsigned __int128)b;
#else
    uint64_t shifted = a << n;
    if ((shifted >> n) == a) {
        return shifted / b;
    }
    return (uint64_t)((double)a / (double)b * (double)((uint64_t)1 << n));
#endif
}

int elib_ft_uq64_add_s(uint64_t a, uint64_t b, uint64_t *out)
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

int elib_ft_uq64_sub_s(uint64_t a, uint64_t b, uint64_t *out)
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

int elib_ft_uq64_mul_s(uint64_t a, uint64_t b, int n, uint64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_U(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__GNUC__) || defined(__clang__)
    unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
    r >>= n;
    if (r > UINT64_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint64_t)r;
    return ELIB_FT_OK;
#else
    double da = (double)a;
    double db = (double)b;
    double dr = da * db / (double)((uint64_t)1 << n);
    if (dr > (double)UINT64_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = elib_ft_uq64_mul(a, b, n);
    return ELIB_FT_OK;
#endif
}

int elib_ft_uq64_div_s(uint64_t a, uint64_t b, int n, uint64_t *out)
{
    if (out == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (b == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!Q_N_VALID_U(n, 64)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
#if defined(__GNUC__) || defined(__clang__)
    unsigned __int128 shifted = (unsigned __int128)a << n;
    unsigned __int128 r = shifted / (unsigned __int128)b;
    if (r > UINT64_MAX) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    *out = (uint64_t)r;
    return ELIB_FT_OK;
#else
    uint64_t shifted = a << n;
    if ((shifted >> n) != a) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    uint64_t r = shifted / b;
    *out = r;
    return ELIB_FT_OK;
#endif
}

uint64_t elib_ft_uq64_convert(uint64_t q, int n_from, int n_to)
{
    if (n_to > n_from) {
        return q << (n_to - n_from);
    }
    if (n_to < n_from) {
        return q >> (n_from - n_to);
    }
    return q;
}
