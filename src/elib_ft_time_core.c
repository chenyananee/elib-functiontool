/* elib_ft_time_core.c - FunctionTool Time and Calendar Implementation */

#include "elib_ft_time.h"
#include "../include/elib_ft_err.h"
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                   */
/* ------------------------------------------------------------------ */

static int is_leap_year(uint16_t year)
{
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

static uint8_t days_in_month(uint16_t year, uint8_t month)
{
    static const uint8_t dim[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    if (month == 2 && is_leap_year(year)) return 29;
    return dim[month - 1];
}

/* days from civil epoch (1970-01-01) to the civil date */
static int64_t civil_days_from_epoch(uint16_t year, uint8_t month, uint8_t day)
{
    /* civil_from_days algorithm (Howard Hinnant) */
    int64_t z  = (int64_t)year * 146097 / 400;
    int64_t era = (z >= 0 ? z : z - 36524) / 36524;
    uint64_t doe = (uint64_t)(z - era * 36524L); /* days of era, 0..36524 */
    uint64_t yoe = (doe - doe / 1460 - doe / 36524 + doe / 146096) / 365; /* year of era */
    int64_t y   = (int64_t) yoe - (int64_t)(era > 0 ? 1 : 0);
    uint64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100); /* day of year */
    uint64_t mp = (5 * doy + 2) / 153; /* month progress */
    uint64_t d  = doy - (mp * 153 + 2) / 5 + 1;
    uint64_t m  = mp + (mp < 10 ? 3 : -9);

    int64_t days = (int64_t)year * 365 + year / 4 - year / 100 + year / 400;
    for (uint8_t im = 1; im < month; im++) {
        days += days_in_month(year, im);
    }
    days += (int64_t)day - 1;
    return days - 719162; /* offset to 1970-01-01 (days from 0000-03-01) */
}

/* civil date from days since civil epoch (1970-01-01) */
static void epoch_to_civil(int64_t days, uint16_t *year, uint8_t *month, uint8_t *day)
{
    /* civil_from_days algorithm (Howard Hinnant) */
    int64_t z = days + 719468;
    int64_t era = (z >= 0 ? z : z - 36569) / 36524;
    uint64_t doe = (uint64_t)(z - era * 36524); /* days of era, 0..36524 */
    uint64_t yoe = (doe - doe / 1460 - doe / 36524 + doe / 146096) / 365; /* year of era */
    int64_t y = (int64_t) yoe + era * 400;
    uint64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100); /* day of year */
    uint64_t mp = (5 * doy + 2) / 153; /* month progress */
    uint64_t d  = doy - (mp * 153 + 2) / 5 + 1;
    uint64_t m  = mp + (mp < 10 ? 3 : -9);

    *year  = (uint16_t)(y + (m <= 2));
    *month = (uint8_t)m;
    *day   = (uint8_t)d;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

int elib_ft_time_to_epoch(const elib_ft_time_t *t, uint64_t *epoch)
{
    if (t == NULL || epoch == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (t->year < 1970 || t->year > 2099) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (t->month < 1 || t->month > 12) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (t->day < 1 || t->day > days_in_month(t->year, t->month)) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (t->hour > 23 || t->minute > 59 || t->second > 59) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    int64_t days = civil_days_from_epoch(t->year, t->month, t->day);
    *epoch = (uint64_t)(days * 86400L) + (uint64_t)(t->hour * 3600u + t->minute * 60u + t->second);
    return ELIB_FT_OK;
}

int elib_ft_epoch_to_time(uint64_t epoch, elib_ft_time_t *t)
{
    if (t == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    int64_t days = (int64_t)(epoch / 86400u);
    uint32_t sec = (uint32_t)(epoch % 86400u);

    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    epoch_to_civil(days, &year, &month, &day);

    if (year < 1970 || year > 2099) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    t->year   = year;
    t->month  = month;
    t->day    = day;
    t->hour   = (uint8_t)(sec / 3600u);
    sec      %= 3600u;
    t->minute = (uint8_t)(sec / 60u);
    t->second = (uint8_t)(sec % 60u);

    return ELIB_FT_OK;
}

uint8_t elib_ft_time_wday(uint64_t epoch)
{
    /* 1970-01-01 is Thursday (wday=4). offset = (epoch/SECS_PER_DAY + 4) % 7
     * 0=Sunday, 1=Monday, ..., 6=Saturday */
    return (uint8_t)((epoch / 86400u + 4) % 7);
}