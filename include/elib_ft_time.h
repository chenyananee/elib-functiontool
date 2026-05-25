/* elib_ft_time.h - FunctionTool Time and Calendar */

#ifndef ELIB_FT_TIME_H
#define ELIB_FT_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Calendar time structure                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    uint16_t year;    /* 1970-2099 */
    uint8_t  month;   /* 1-12 */
    uint8_t  day;     /* 1-31 */
    uint8_t  hour;    /* 0-23 */
    uint8_t  minute;  /* 0-59 */
    uint8_t  second;  /* 0-59 */
} elib_ft_time_t;

/* ------------------------------------------------------------------ */
/*  Epoch <-> Calendar conversion                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief Convert calendar time to Unix epoch seconds
 * @param t Calendar time (year 1970-2038)
 * @param epoch Output epoch seconds (from 1970-01-01 00:00:00 UTC)
 * @return ELIB_FT_OK on success, error code otherwise
 *
 * Supported year range: 1970-2038 (32-bit safe).
 * epoch fits in uint32_t (max: 2106-02-07 06:28:15).
 * Does not account for leap seconds or timezone (always UTC).
 */
int elib_ft_time_to_epoch(const elib_ft_time_t *t, uint32_t *epoch);

/**
 * @brief Convert Unix epoch seconds to calendar time
 * @param epoch Epoch seconds (from 1970-01-01 00:00:00 UTC)
 * @param t Output calendar time
 * @return ELIB_FT_OK on success, error code otherwise
 *
 * Supported year range: 1970-2038 (32-bit safe).
 * Does not account for leap seconds or timezone (always UTC).
 */
int elib_ft_epoch_to_time(uint32_t epoch, elib_ft_time_t *t);

/**
 * @brief Get day of week from epoch seconds
 * @param epoch Epoch seconds
 * @return 0=Sunday, 1=Monday, ..., 6=Saturday
 */
uint8_t elib_ft_time_wday(uint32_t epoch);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_TIME_H */