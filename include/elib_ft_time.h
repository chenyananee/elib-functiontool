/* elib_ft_time.h - FunctionTool Time and Calendar */

#ifndef ELIB_FT_TIME_H
#define ELIB_FT_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Epoch type (configurable)                                          */
/* ------------------------------------------------------------------ */

/**
 * @def ELIB_FT_EPOCH_T
 * @brief Epoch second type.
 *
 * Default: uint64_t (64-bit, no year limit, up to ~2920 billion AD).
 * On 8/16-bit platforms, redefine to uint32_t before including:
 *   #define ELIB_FT_EPOCH_T uint32_t
 *   #include "elib_ft_time.h"
 *
 * uint32_t limit: 1970-01-01 to 2106-02-07 06:28:15 UTC.
 */
#ifndef ELIB_FT_EPOCH_T
    #define ELIB_FT_EPOCH_T uint64_t
#endif

/**
 * @internal
 * @def ELIB_FT_TIME_I64
 * @brief Internal arithmetic type (always int64_t).
 *
 * User code should not use this type directly.
 *
 * Year range is limited to ~5883516 because: year * 365 must fit in
 * int32_t (max 2147483647 / 365 ≈ 5883516). This limit applies
 * regardless of ELIB_FT_EPOCH_T width.
 */
#ifndef ELIB_FT_TIME_I64
    #define ELIB_FT_TIME_I64 int64_t
#endif

/* ------------------------------------------------------------------ */
/*  Calendar time structure                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    uint16_t year;    /* year (no limit, int32_t arithmetic safe: 1970-5883516) */
    uint8_t  month;   /* 1-12 */
    uint8_t  day;     /* 1-31 */
    uint8_t  hour;    /* 0-23 */
    uint8_t  minute;  /* 0-59 */
    uint8_t  second;  /* 0-59 */
} elib_ft_time_t;

/* ------------------------------------------------------------------ */
/*  Epoch <-> Calendar conversion                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief Convert calendar time to Unix epoch seconds
 * @param t Calendar time
 * @param epoch Output epoch seconds
 * @return ELIB_FT_OK on success, error code otherwise
 *
 * Year must be >= 1970.
 * Does not account for leap seconds or timezone (always UTC).
 */
int elib_ft_time_to_epoch(const elib_ft_time_t *t, ELIB_FT_EPOCH_T *epoch);

/**
 * @brief Convert Unix epoch seconds to calendar time
 * @param epoch Epoch seconds
 * @param t Output calendar time
 * @return ELIB_FT_OK on success, error code otherwise
 *
 * Does not account for leap seconds or timezone (always UTC).
 */
int elib_ft_epoch_to_time(ELIB_FT_EPOCH_T epoch, elib_ft_time_t *t);

/**
 * @brief Get day of week from epoch seconds
 * @param epoch Epoch seconds
 * @return 0=Sunday, 1=Monday, ..., 6=Saturday
 */
uint8_t elib_ft_time_wday(ELIB_FT_EPOCH_T epoch);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_TIME_H */