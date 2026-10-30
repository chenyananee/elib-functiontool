/* elib_ft_time_core.c - FunctionTool Time and Calendar Implementation */
/* ELIB_FT_EPOCH_T controls output type, elib_ft_epoch_signed_t for internal arithmetic. */

#include "elib_ft.h"

typedef elib_ft_epoch_signed_t ies; /* internal epoch signed type */

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static ies is_leap_year(ies year)
{
    return ((year & 3) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}

static ies days_in_month(ies year, ies month)
{
    static const ies dim[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    if (month == 2 && is_leap_year(year)) return 29;
    return dim[month - 1];
}

/* days from 1970-01-01 to the civil date */
static ies days_from_epoch(ies year, ies month, ies day)
{
    ies y = year - 1;
    ies days = y * 365 + y / 4 - y / 100 + y / 400;

    for (ies m = 1; m < month; m++) {
        days += days_in_month(year, m);
    }
    days += day - 1;
    return days - 719162;
}

/* civil date from days since 1970-01-01 */
static void civil_from_days(ies days, ies *year, ies *month, ies *day)
{
    ies z = days + 719468;
    /* floor(z / 146097): the z - 146096 adjustment makes truncating
     * C division act as floor division for every negative z, exact
     * multiples included */
    ies era = (z >= 0) ? (z / 146097) : ((z - 146096) / 146097);
    ies doe = z - era * 146097;
    ies yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    ies y = yoe + era * 400;
    ies doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    ies mp = (5 * doy + 2) / 153;
    *day = doy - (mp * 153 + 2) / 5 + 1;
    *month = mp + (mp < 10 ? 3 : -9);
    *year = y + (mp >= 10 ? 1 : 0);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

elib_ft_err_t elib_ft_time_to_epoch(const elib_ft_time_t *t, ELIB_FT_EPOCH_T *epoch)
{
    if (t == NULL || epoch == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (t->year < 1970) {
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

#if UINT32_MAX == (ELIB_FT_EPOCH_T)-1
    /* uint32 epoch config: 2106-02-07 06:28:15 UTC is the last valid
     * second. Year-level gate first, then precise wrap detection so
     * the seconds sum never relies on 64-bit arithmetic. */
    if (t->year > 2106) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
#endif

    ies d = days_from_epoch(t->year, t->month, t->day);
    uint32_t hms = (uint32_t)t->hour * 3600u + (uint32_t)t->minute * 60u + (uint32_t)t->second;

    *epoch = 0;
#if UINT32_MAX == (ELIB_FT_EPOCH_T)-1
    {
        uint32_t s = (uint32_t)d;
        s *= 86400u;                        /* unsigned wrap is defined */
        if (s / 86400u != (uint32_t)d) {    /* days*86400 wrapped */
            return ELIB_FT_ERR_EXCEED_MAX;
        }
        if (s + hms < s) {                  /* seconds part wrapped */
            return ELIB_FT_ERR_EXCEED_MAX;
        }
        s += hms;
        *epoch = s;
    }
#else
    {
        ies sec = d * 86400 + (ies)hms;     /* int64: year cap 5883516 keeps this safe */
        *epoch = (ELIB_FT_EPOCH_T)sec;
    }
#endif

    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_epoch_to_time(ELIB_FT_EPOCH_T epoch, elib_ft_time_t *t)
{
    if (t == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    ies days = (ies)(epoch / 86400);
    uint32_t sec = (uint32_t)(epoch % 86400);

    ies year = 0, month = 0, day = 0;
    civil_from_days(days, &year, &month, &day);

    if (year < 1970) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (year > 65535) {
        return ELIB_FT_ERR_EXCEED_MAX;      /* calendar year cap of elib_ft_time_t.year */
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

uint8_t elib_ft_time_wday(ELIB_FT_EPOCH_T epoch)
{
    /* 1970-01-01 = Thursday (wday=4) */
    return (uint8_t)((epoch / 86400 + 4) % 7);
}
