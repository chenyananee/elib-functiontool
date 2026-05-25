/* elib_ft_util.h - FunctionTool Utility Macros */

#ifndef ELIB_FT_UTIL_H
#define ELIB_FT_UTIL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Compiler compatibility                                              */
/* ------------------------------------------------------------------ */

#if defined(__ICCARM__)
    #define ELIB_FT_WEAK __weak
#elif defined(__CC_ARM)
    #define ELIB_FT_WEAK __weak
#elif defined(__GNUC__)
    #define ELIB_FT_WEAK __attribute__((weak))
#else
    #define ELIB_FT_WEAK
#endif

/* ------------------------------------------------------------------ */
/*  Array helper                                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Get number of elements in an array
 * @param arr Array variable (not a pointer)
 * @return Element count as uint32_t
 */
#define ELIB_FT_ARRAY_SIZE(arr) ((uint32_t)(sizeof(arr) / sizeof((arr)[0])))

/* ------------------------------------------------------------------ */
/*  MIN / MAX                                                           */
/* ------------------------------------------------------------------ */

/** @brief Return the smaller of two values */
#define ELIB_FT_MIN(a, b)  ((a) < (b) ? (a) : (b))

/** @brief Return the larger of two values */
#define ELIB_FT_MAX(a, b)  ((a) > (b) ? (a) : (b))

/* ------------------------------------------------------------------ */
/*  CLAMP                                                               */
/* ------------------------------------------------------------------ */

/** @brief Clamp value to [lo, hi] range */
#define ELIB_FT_CLAMP(val, lo, hi)  ELIB_FT_MIN(ELIB_FT_MAX(val, lo), hi)

/* ------------------------------------------------------------------ */
/*  MAP — linear mapping                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief Integer linear mapping with rounding
 * @param val    Input value
 * @param in_lo  Input range lower bound
 * @param in_hi  Input range upper bound
 * @param out_lo Output range lower bound
 * @param out_hi Output range upper bound
 * @return Mapped value (rounded to nearest integer)
 *
 * Maps val from [in_lo, in_hi] to [out_lo, out_hi].
 * Uses rounding: +((in_hi - in_lo) / 2) before division.
 */
#define ELIB_FT_MAP(val, in_lo, in_hi, out_lo, out_hi) \
    ((out_lo) + (((val) - (in_lo)) * ((out_hi) - (out_lo)) + \
     ((in_hi) - (in_lo)) / 2) / ((in_hi) - (in_lo)))

/**
 * @brief Floating-point linear mapping
 * @param val    Input value
 * @param in_lo  Input range lower bound
 * @param in_hi  Input range upper bound
 * @param out_lo Output range lower bound
 * @param out_hi Output range upper bound
 * @return Mapped value (double precision)
 */
#define ELIB_FT_MAP_F(val, in_lo, in_hi, out_lo, out_hi) \
    ((out_lo) + (double)((val) - (in_lo)) * ((out_hi) - (out_lo)) / ((in_hi) - (in_lo)))

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_UTIL_H */
