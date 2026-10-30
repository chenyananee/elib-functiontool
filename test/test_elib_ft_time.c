/* test_elib_ft_time.c - FunctionTool Calendar <-> Epoch Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

typedef ELIB_FT_EPOCH_T test_epoch_t;

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_time_to_epoch_basic(void)
{
    elib_ft_time_t t = {2000, 1, 1, 0, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_OK);
    /* 2000-01-01 00:00:00 UTC = 946684800 */
    assert(epoch == (test_epoch_t)946684800U);
}

static void test_time_to_epoch_leap_year(void)
{
    elib_ft_time_t t = {2000, 2, 29, 12, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_OK);
    /* 2000-02-29 12:00:00 UTC */
    assert(epoch == (test_epoch_t)951868800U);
}

static void test_time_to_epoch_invalid_year(void)
{
    elib_ft_time_t t = {1969, 1, 1, 0, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_ERR_INVALID_PARAM);

    t.year = 2039;  /* should now succeed — no upper limit */
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_OK);
}

static void test_time_to_epoch_invalid_month(void)
{
    elib_ft_time_t t = {2000, 0, 1, 0, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_ERR_INVALID_PARAM);

    t.month = 13;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_time_to_epoch_invalid_day(void)
{
    elib_ft_time_t t = {2000, 2, 30, 0, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_time_to_epoch_invalid_time(void)
{
    elib_ft_time_t t = {2000, 1, 1, 24, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t, &epoch) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_time_to_epoch_null(void)
{
    elib_ft_time_t t = {2000, 1, 1, 0, 0, 0};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(NULL, &epoch) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_time_to_epoch(&t, NULL) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_epoch_to_time_basic(void)
{
    elib_ft_time_t t;
    assert(elib_ft_epoch_to_time(946684800U, &t) == ELIB_FT_OK);
    assert(t.year == 2000);
    assert(t.month == 1);
    assert(t.day == 1);
    assert(t.hour == 0);
    assert(t.minute == 0);
    assert(t.second == 0);
}

static void test_epoch_to_time_leap(void)
{
    elib_ft_time_t t;
    assert(elib_ft_epoch_to_time(951868800U, &t) == ELIB_FT_OK);
    assert(t.year == 2000);
    assert(t.month == 2);
    assert(t.day == 29);
    assert(t.hour == 12);
}

static void test_epoch_to_time_roundtrip(void)
{
    elib_ft_time_t t_in = {2025, 6, 15, 8, 30, 45};
    test_epoch_t epoch = 0;
    assert(elib_ft_time_to_epoch(&t_in, &epoch) == ELIB_FT_OK);

    elib_ft_time_t t_out;
    assert(elib_ft_epoch_to_time(epoch, &t_out) == ELIB_FT_OK);
    assert(t_out.year == t_in.year);
    assert(t_out.month == t_in.month);
    assert(t_out.day == t_in.day);
    assert(t_out.hour == t_in.hour);
    assert(t_out.minute == t_in.minute);
    assert(t_out.second == t_in.second);
}

static void test_epoch_to_time_null(void)
{
    assert(elib_ft_epoch_to_time(0, NULL) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_time_wday(void)
{
    /* 2000-01-01 is Saturday (wday=6) */
    assert(elib_ft_time_wday(946684800U) == 6);
    /* 2000-01-02 is Sunday (wday=0) */
    assert(elib_ft_time_wday(946771200U) == 0);
}

static void test_time_wday_epoch0(void)
{
    /* 1970-01-01 is Thursday (wday=4) */
    assert(elib_ft_time_wday(0) == 4);
}

static void test_time_known_seconds(void)
{
    ELIB_FT_EPOCH_T e = 0;
    elib_ft_time_t t = {2000, 2, 29, 0, 0, 0};
    assert(elib_ft_time_to_epoch(&t, &e) == ELIB_FT_OK && e == 951782400);
    elib_ft_time_t u = {1970, 1, 2, 0, 0, 0};
    assert(elib_ft_time_to_epoch(&u, &e) == ELIB_FT_OK && e == 86400);
}

static void test_epoch_to_time_year_cap(void)
{
    /* the calendar year field tops out at 65535 */
    elib_ft_time_t in = {65535, 12, 31, 23, 59, 59};
    ELIB_FT_EPOCH_T e = 0;
    assert(elib_ft_time_to_epoch(&in, &e) == ELIB_FT_OK);
    elib_ft_time_t out;
    assert(elib_ft_epoch_to_time(e, &out) == ELIB_FT_OK);
    assert(out.year == 65535 && out.month == 12 && out.day == 31);
    assert(out.hour == 23 && out.minute == 59 && out.second == 59);
    e++;                                            /* new year 65536 */
    memset(&out, 0, sizeof(out));
    assert(elib_ft_epoch_to_time(e, &out) == ELIB_FT_ERR_EXCEED_MAX);
    /* below 1970 floor (huge unsigned epoch) is out of range as well */
    assert(elib_ft_epoch_to_time((ELIB_FT_EPOCH_T)-1, &out) == ELIB_FT_ERR_EXCEED_MAX);
}

static void test_wday_known_dates(void)
{
    assert(elib_ft_time_wday(0) == 4);               /* 1970-01-01 Thu */
    assert(elib_ft_time_wday(951782400) == 2);       /* 2000-02-29 Tue */
}

#if UINT32_MAX == (ELIB_FT_EPOCH_T)-1
static void test_time_uint32_cap(void)
{
    /* last representable second of the uint32 epoch config */
    elib_ft_time_t t = {2106, 2, 7, 6, 28, 15};
    ELIB_FT_EPOCH_T e = 0;
    assert(elib_ft_time_to_epoch(&t, &e) == ELIB_FT_OK && e == 4294967295u);
    elib_ft_time_t out = {0, 0, 0, 0, 0, 0};
    assert(elib_ft_epoch_to_time(4294967295u, &out) == ELIB_FT_OK);
    assert(out.year == 2106 && out.month == 2 && out.day == 7);
    assert(out.hour == 6 && out.minute == 28 && out.second == 15);
    /* one second past the cap */
    t.second = 16;
    assert(elib_ft_time_to_epoch(&t, &e) == ELIB_FT_ERR_EXCEED_MAX);
}
#endif

int main(void)
{
    RUN_TEST(test_time_to_epoch_basic);
    RUN_TEST(test_time_to_epoch_leap_year);
    RUN_TEST(test_time_to_epoch_invalid_year);
    RUN_TEST(test_time_to_epoch_invalid_month);
    RUN_TEST(test_time_to_epoch_invalid_day);
    RUN_TEST(test_time_to_epoch_invalid_time);
    RUN_TEST(test_time_to_epoch_null);
    RUN_TEST(test_epoch_to_time_basic);
    RUN_TEST(test_epoch_to_time_leap);
    RUN_TEST(test_epoch_to_time_roundtrip);
    RUN_TEST(test_epoch_to_time_null);
    RUN_TEST(test_time_wday);
    RUN_TEST(test_time_wday_epoch0);
    RUN_TEST(test_time_known_seconds);
    RUN_TEST(test_epoch_to_time_year_cap);
    RUN_TEST(test_wday_known_dates);
#if UINT32_MAX == (ELIB_FT_EPOCH_T)-1
    RUN_TEST(test_time_uint32_cap);
#endif

    printf("\nAll %d tests passed.\n", 16);
    return 0;
}
