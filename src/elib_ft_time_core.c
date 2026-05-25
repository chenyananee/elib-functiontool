/* elib_ft_time_core.c - FunctionTool Time and Calendar Implementation */
/* 仅支持 32 位平台，年份范围 1970-2038 */

#include "elib_ft_time.h"
#include "../include/elib_ft_err.h"

typedef int32_t i32;

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static i32 days_in_month(i32 year, i32 month)
{
    static const i32 dim[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    if (month == 2 && (year & 3) == 0 && ((year % 100) != 0 || (year % 400) == 0)) {
        return 29;
    }
    return dim[month - 1];
}

static i32 is_leap_year(i32 year)
{
    return ((year & 3) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}

/* days from 1970-01-01 to the civil date */
static i32 days_from_epoch(i32 year, i32 month, i32 day)
{
    i32 y = year - 1;
    i32 days = y * 365 + y / 4 - y / 100 + y / 400; /* days to year start */

    /* days in months before this month */
    for (i32 m = 1; m < month; m++) {
        days += days_in_month(year, m);
    }
    days += day - 1; /* days into this month, 0-based */
    return days - 719162; /* offset: days from 0000-03-01 to 1970-01-01 = 719162 */
}

/* civil date from days since 1970-01-01 */
static void civil_from_days(i32 days, i32 *year, i32 *month, i32 *day)
{
    i32 z = days + 719468;
    i32 era = z / 146097;
    if (z < 0) era -= 1;
    i32 doe = z - era * 146097;
    i32 yoe = (doe - doe / 1460 - doe / 36524 + doe / 146096) / 365;
    i32 y = yoe + era * 400;
    i32 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    i32 mp = (5 * doy + 2) / 153;
    *day = doy - (mp * 153 + 2) / 5 + 1;
    *month = mp + (mp < 10 ? 3 : -9);
    *year = y + (mp >= 10 ? 1 : 0);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

/**
 * Calendar time -> epoch seconds
 * year range: 1970-2038 (32-bit safe)
 * epoch fits in uint32_t (max: 2106-02-07 06:28:15)
 */
int elib_ft_time_to_epoch(const elib_ft_time_t *t, uint32_t *epoch)
{
    if (t == NULL || epoch == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (t->year < 1970 || t->year > 2038) {
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

    i32 days = days_from_epoch(t->year, t->month, t->day);
    *epoch = (uint32_t)(days * 86400) + (uint32_t)(t->hour * 3600u + t->minute * 60u + t->second);
    return ELIB_FT_OK;
}

/**
 * epoch seconds -> calendar time
 * year range: 1970-2038 (32-bit safe)
 */
int elib_ft_epoch_to_time(uint32_t epoch, elib_ft_time_t *t)
{
    if (t == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    i32 days = (i32)(epoch / 86400u);
    uint32_t sec = epoch % 86400u;

    i32 year = 0, month = 0, day = 0;
    civil_from_days(days, &year, &month, &day);

    if (year < 1970 || year > 2038) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    t->year   = (uint16_t)year;
    t->month  = (uint8_t)month;
    t->day    = (uint8_t)day;
    t->hour   = (uint8_t)(sec / 3600u);
    sec      %= 3600u;
    t->minute = (uint8_t)(sec / 60u);
    t->second = (uint8_t)(sec % 60u);

    return ELIB_FT_OK;
}

/**
 * Day of week from epoch seconds
 * 1970-01-01 = Thursday (wday=4)
 * 0=Sunday, 1=Monday, ..., 6=Saturday
 */
uint8_t elib_ft_time_wday(uint32_t epoch)
{
    return (uint8_t)((epoch / 86400u + 4) % 7);
}