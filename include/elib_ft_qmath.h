/* elib_ft_qmath.h - FunctionTool Qm.n Fixed-Point Arithmetic */

#ifndef ELIB_FT_QMATH_H
#define ELIB_FT_QMATH_H

#include "elib_ft_err.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Q16 - Signed 16-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int16_t  elib_ft_q16_from_double(double val, int n);
double   elib_ft_q16_to_double(int16_t q, int n);
int16_t  elib_ft_q16_from_float(float val, int n);
float    elib_ft_q16_to_float(int16_t q, int n);
int16_t  elib_ft_q16_add(int16_t a, int16_t b);
int16_t  elib_ft_q16_sub(int16_t a, int16_t b);
int16_t  elib_ft_q16_mul(int16_t a, int16_t b, int n);
int16_t  elib_ft_q16_div(int16_t a, int16_t b, int n);
int elib_ft_q16_add_s(int16_t a, int16_t b, int16_t *out);
int elib_ft_q16_sub_s(int16_t a, int16_t b, int16_t *out);
int elib_ft_q16_mul_s(int16_t a, int16_t b, int n, int16_t *out);
int elib_ft_q16_div_s(int16_t a, int16_t b, int n, int16_t *out);
int16_t  elib_ft_q16_convert(int16_t q, int n_from, int n_to);
int16_t  elib_ft_q16_floor(int16_t q, int n);
int16_t  elib_ft_q16_ceil(int16_t q, int n);
int16_t  elib_ft_q16_round(int16_t q, int n);
int16_t  elib_ft_q16_abs(int16_t q);

/* ------------------------------------------------------------------ */
/*  UQ16 - Unsigned 16-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint16_t elib_ft_uq16_from_double(double val, int n);
double   elib_ft_uq16_to_double(uint16_t q, int n);
uint16_t elib_ft_uq16_add(uint16_t a, uint16_t b);
uint16_t elib_ft_uq16_sub(uint16_t a, uint16_t b);
uint16_t elib_ft_uq16_mul(uint16_t a, uint16_t b, int n);
uint16_t elib_ft_uq16_div(uint16_t a, uint16_t b, int n);
int elib_ft_uq16_add_s(uint16_t a, uint16_t b, uint16_t *out);
int elib_ft_uq16_sub_s(uint16_t a, uint16_t b, uint16_t *out);
int elib_ft_uq16_mul_s(uint16_t a, uint16_t b, int n, uint16_t *out);
int elib_ft_uq16_div_s(uint16_t a, uint16_t b, int n, uint16_t *out);
uint16_t elib_ft_uq16_convert(uint16_t q, int n_from, int n_to);

/* ------------------------------------------------------------------ */
/*  Q32 - Signed 32-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int32_t  elib_ft_q32_from_double(double val, int n);
double   elib_ft_q32_to_double(int32_t q, int n);
int32_t  elib_ft_q32_from_float(float val, int n);
float    elib_ft_q32_to_float(int32_t q, int n);
int32_t  elib_ft_q32_add(int32_t a, int32_t b);
int32_t  elib_ft_q32_sub(int32_t a, int32_t b);
int32_t  elib_ft_q32_mul(int32_t a, int32_t b, int n);
int32_t  elib_ft_q32_div(int32_t a, int32_t b, int n);
int elib_ft_q32_add_s(int32_t a, int32_t b, int32_t *out);
int elib_ft_q32_sub_s(int32_t a, int32_t b, int32_t *out);
int elib_ft_q32_mul_s(int32_t a, int32_t b, int n, int32_t *out);
int elib_ft_q32_div_s(int32_t a, int32_t b, int n, int32_t *out);
int32_t  elib_ft_q32_convert(int32_t q, int n_from, int n_to);
int32_t  elib_ft_q32_floor(int32_t q, int n);
int32_t  elib_ft_q32_ceil(int32_t q, int n);
int32_t  elib_ft_q32_round(int32_t q, int n);
int32_t  elib_ft_q32_abs(int32_t q);

/* ------------------------------------------------------------------ */
/*  UQ32 - Unsigned 32-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint32_t elib_ft_uq32_from_double(double val, int n);
double   elib_ft_uq32_to_double(uint32_t q, int n);
uint32_t elib_ft_uq32_add(uint32_t a, uint32_t b);
uint32_t elib_ft_uq32_sub(uint32_t a, uint32_t b);
uint32_t elib_ft_uq32_mul(uint32_t a, uint32_t b, int n);
uint32_t elib_ft_uq32_div(uint32_t a, uint32_t b, int n);
int elib_ft_uq32_add_s(uint32_t a, uint32_t b, uint32_t *out);
int elib_ft_uq32_sub_s(uint32_t a, uint32_t b, uint32_t *out);
int elib_ft_uq32_mul_s(uint32_t a, uint32_t b, int n, uint32_t *out);
int elib_ft_uq32_div_s(uint32_t a, uint32_t b, int n, uint32_t *out);
uint32_t elib_ft_uq32_convert(uint32_t q, int n_from, int n_to);

/* ------------------------------------------------------------------ */
/*  Q64 - Signed 64-bit Qm.n                                          */
/* ------------------------------------------------------------------ */

int64_t  elib_ft_q64_from_double(double val, int n);
double   elib_ft_q64_to_double(int64_t q, int n);
int64_t  elib_ft_q64_add(int64_t a, int64_t b);
int64_t  elib_ft_q64_sub(int64_t a, int64_t b);
int64_t  elib_ft_q64_mul(int64_t a, int64_t b, int n);
int64_t  elib_ft_q64_div(int64_t a, int64_t b, int n);
int elib_ft_q64_add_s(int64_t a, int64_t b, int64_t *out);
int elib_ft_q64_sub_s(int64_t a, int64_t b, int64_t *out);
int elib_ft_q64_mul_s(int64_t a, int64_t b, int n, int64_t *out);
int elib_ft_q64_div_s(int64_t a, int64_t b, int n, int64_t *out);
int64_t  elib_ft_q64_convert(int64_t q, int n_from, int n_to);
int64_t  elib_ft_q64_floor(int64_t q, int n);
int64_t  elib_ft_q64_ceil(int64_t q, int n);
int64_t  elib_ft_q64_round(int64_t q, int n);
int64_t  elib_ft_q64_abs(int64_t q);

/* ------------------------------------------------------------------ */
/*  UQ64 - Unsigned 64-bit Qm.n                                       */
/* ------------------------------------------------------------------ */

uint64_t elib_ft_uq64_from_double(double val, int n);
double   elib_ft_uq64_to_double(uint64_t q, int n);
uint64_t elib_ft_uq64_add(uint64_t a, uint64_t b);
uint64_t elib_ft_uq64_sub(uint64_t a, uint64_t b);
uint64_t elib_ft_uq64_mul(uint64_t a, uint64_t b, int n);
uint64_t elib_ft_uq64_div(uint64_t a, uint64_t b, int n);
int elib_ft_uq64_add_s(uint64_t a, uint64_t b, uint64_t *out);
int elib_ft_uq64_sub_s(uint64_t a, uint64_t b, uint64_t *out);
int elib_ft_uq64_mul_s(uint64_t a, uint64_t b, int n, uint64_t *out);
int elib_ft_uq64_div_s(uint64_t a, uint64_t b, int n, uint64_t *out);
uint64_t elib_ft_uq64_convert(uint64_t q, int n_from, int n_to);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_QMATH_H */
