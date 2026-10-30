/* test_elib_ft_str.c - FunctionTool Bounded Strings / Parsing Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_strlen_normal(void)
{
    assert(elib_ft_strlen("hello", 10) == 5);
}

static void test_strlen_with_limit(void)
{
    assert(elib_ft_strlen("hello", 3) == 3);
}

static void test_strlen_null(void)
{
    assert(elib_ft_strlen(NULL, 10) == 0);
}

static void test_strlen_empty(void)
{
    assert(elib_ft_strlen("", 10) == 0);
}

static void test_strcmp_equal(void)
{
    assert(elib_ft_strcmp("hello", "hello") == 0);
}

static void test_strcmp_less(void)
{
    assert(elib_ft_strcmp("abc", "abd") < 0);
}

static void test_strcmp_greater(void)
{
    assert(elib_ft_strcmp("abd", "abc") > 0);
}

static void test_strcmp_null_s1(void)
{
    assert(elib_ft_strcmp(NULL, "abc") < 0);
}

static void test_strcmp_null_s2(void)
{
    assert(elib_ft_strcmp("abc", NULL) > 0);
}

static void test_strcmp_both_null(void)
{
    assert(elib_ft_strcmp(NULL, NULL) == 0);
}

static void test_strncmp_equal(void)
{
    assert(elib_ft_strncmp("hello", "hello", 5) == 0);
}

static void test_strncmp_prefix(void)
{
    assert(elib_ft_strncmp("hello", "helloworld", 5) == 0);
}

static void test_strncmp_less(void)
{
    assert(elib_ft_strncmp("abc", "abd", 3) < 0);
}

static void test_strncmp_greater(void)
{
    assert(elib_ft_strncmp("abd", "abc", 3) > 0);
}

static void test_strncmp_zero_n(void)
{
    assert(elib_ft_strncmp("abc", "xyz", 0) == 0);
}

static void test_strncmp_null_s1(void)
{
    assert(elib_ft_strncmp(NULL, "abc", 3) < 0);
}

static void test_strncmp_null_s2(void)
{
    assert(elib_ft_strncmp("abc", NULL, 3) > 0);
}

static void test_strncmp_null_both(void)
{
    assert(elib_ft_strncmp(NULL, NULL, 3) == 0);
}

static void test_strncmp_null_n(void)
{
    assert(elib_ft_strncmp("abc", "abc", 0) == 0);
}

static void test_atoi32_normal(void)
{
    const char *end;
    assert(elib_ft_atoi32("12345", 10, &end) == 12345);
    assert(*end == '\0');
}

static void test_atoi32_negative(void)
{
    const char *end;
    assert(elib_ft_atoi32("-12345", 10, &end) == -12345);
    assert(*end == '\0');
}

static void test_atoi32_positive_sign(void)
{
    const char *end;
    assert(elib_ft_atoi32("+12345", 10, &end) == 12345);
    assert(*end == '\0');
}

static void test_atoi32_with_limit(void)
{
    const char *end;
    assert(elib_ft_atoi32("12345", 3, &end) == 12);
    assert(*end == '3');
}

static void test_atoi32_zero(void)
{
    const char *end;
    assert(elib_ft_atoi32("0", 10, &end) == 0);
    assert(*end == '\0');
}

static void test_atoi32_leading_spaces(void)
{
    const char *end;
    assert(elib_ft_atoi32("  -42abc", 10, &end) == -42);
    assert(*end == 'a');
}

static void test_atoi32_invalid(void)
{
    const char *end;
    assert(elib_ft_atoi32("abc", 10, &end) == 0);
}

static void test_atoi32_null(void)
{
    const char *end;
    assert(elib_ft_atoi32(NULL, 10, &end) == 0);
    assert(end == NULL);
}

static void test_atou32_normal(void)
{
    const char *end;
    assert(elib_ft_atou32("12345", 10, &end) == 12345);
    assert(*end == '\0');
}

static void test_atou32_zero(void)
{
    const char *end;
    assert(elib_ft_atou32("0", 10, &end) == 0);
}

static void test_atou32_with_limit(void)
{
    const char *end;
    assert(elib_ft_atou32("12345", 3, &end) == 12);
    assert(*end == '3');
}

static void test_atou32_null(void)
{
    const char *end;
    assert(elib_ft_atou32(NULL, 10, &end) == 0);
    assert(end == NULL);
}

static void test_atof_normal(void)
{
    const char *end;
    double v = elib_ft_atof("3.14", 10, &end);
    assert(v > 3.13 && v < 3.15);
    assert(*end == '\0');
}

static void test_atof_negative(void)
{
    const char *end;
    double v = elib_ft_atof("-2.718", 10, &end);
    assert(v < -2.71 && v > -2.73);
    assert(*end == '\0');
}

static void test_atof_integer(void)
{
    const char *end;
    double v = elib_ft_atof("42", 10, &end);
    assert(v > 41.9 && v < 42.1);
    assert(*end == '\0');
}

static void test_atof_with_limit(void)
{
    const char *end;
    double v = elib_ft_atof("3.14159", 4, &end);
    assert(v > 3.13 && v < 3.15);
}

static void test_atof_null(void)
{
    const char *end;
    assert(elib_ft_atof(NULL, 10, &end) == 0.0);
    assert(end == NULL);
}

static void test_strarg_count_normal(void)
{
    assert(elib_ft_strarg_count("hello world foo", 15) == 3);
}

static void test_strarg_count_single(void)
{
    assert(elib_ft_strarg_count("hello", 5) == 1);
}

static void test_strarg_count_leading_spaces(void)
{
    assert(elib_ft_strarg_count("  hello world", 13) == 2);
}

static void test_strarg_count_trailing_spaces(void)
{
    assert(elib_ft_strarg_count("hello world  ", 13) == 2);
}

static void test_strarg_count_multi_spaces(void)
{
    assert(elib_ft_strarg_count("hello   world   foo", 17) == 3);
}

static void test_strarg_count_empty(void)
{
    assert(elib_ft_strarg_count("", 0) == 0);
    assert(elib_ft_strarg_count("   ", 3) == 0);
}

static void test_strarg_count_null(void)
{
    assert(elib_ft_strarg_count(NULL, 10) == 0);
}

static void test_strarg_get_normal(void)
{
    const char *end;
    const char *tok = elib_ft_strarg_get("hello world foo", 15, 0, &end);
    assert(tok != NULL);
    assert(strncmp(tok, "hello", 5) == 0);
    assert(*end == ' ');

    tok = elib_ft_strarg_get("hello world foo", 15, 1, &end);
    assert(tok != NULL);
    assert(strncmp(tok, "world", 5) == 0);
    assert(*end == ' ');

    tok = elib_ft_strarg_get("hello world foo", 15, 2, &end);
    assert(tok != NULL);
    assert(strncmp(tok, "foo", 3) == 0);
    assert(*end == '\0' || end == NULL);
}

static void test_strarg_get_out_of_range(void)
{
    const char *end;
    assert(elib_ft_strarg_get("hello world", 11, 5, &end) == NULL);
    assert(end == NULL);
}

static void test_strarg_get_last(void)
{
    const char *end;
    const char *tok = elib_ft_strarg_get("hello world foo", 15, 2, &end);
    assert(tok != NULL);
    assert(strncmp(tok, "foo", 3) == 0);
}

static void test_strarg_get_null(void)
{
    const char *end;
    assert(elib_ft_strarg_get(NULL, 10, 0, &end) == NULL);
    assert(end == NULL);
}

static void test_strarg_get_null_endptr(void)
{
    assert(elib_ft_strarg_get("hello world", 11, 0, NULL) != NULL);
}

static void test_strcmp_fast_equal(void)
{
    assert(elib_ft_strcmp_fast("hello", "hello", 10) == 0);
}

static void test_strcmp_fast_less(void)
{
    assert(elib_ft_strcmp_fast("abc", "abd", 10) < 0);
}

static void test_strcmp_fast_greater(void)
{
    assert(elib_ft_strcmp_fast("abd", "abc", 10) > 0);
}

static void test_strcmp_fast_len_diff(void)
{
    /* length differs, hash alone resolves without memcmp */
    assert(elib_ft_strcmp_fast("abc", "abcd", 10) < 0);
    assert(elib_ft_strcmp_fast("abcd", "abc", 10) > 0);
}

static void test_strcmp_fast_null_s1(void)
{
    assert(elib_ft_strcmp_fast(NULL, "abc", 10) < 0);
}

static void test_strcmp_fast_null_s2(void)
{
    assert(elib_ft_strcmp_fast("abc", NULL, 10) > 0);
}

static void test_strcmp_fast_both_null(void)
{
    assert(elib_ft_strcmp_fast(NULL, NULL, 10) == 0);
}

static void test_strcmp_fast_max_len(void)
{
    /* "hello" vs "hello123" within max_len=5, equal */
    assert(elib_ft_strcmp_fast("hello123", "hello456", 5) == 0);
}

static void test_strcmp_fast_max_len_diff(void)
{
    assert(elib_ft_strcmp_fast("abcde", "abfgh", 3) == 0);
    assert(elib_ft_strcmp_fast("abcde", "abfgh", 2) == 0);
}

static void test_atoi32_overflow(void)
{
    char buf[24];
    const char *end = buf;

    /* INT32_MAX parses exactly; endptr past the digits (non-NULL) */
    memcpy(buf, "2147483647", 11);
    assert(elib_ft_atoi32(buf, 11, &end) == INT32_MAX);
    assert(end == buf + 10);

    /* INT32_MAX + 1: return 0 AND *endptr = NULL (v2 overflow sentinel) */
    memcpy(buf, "2147483648", 11);
    assert(elib_ft_atoi32(buf, 11, &end) == 0);
    assert(end == NULL);

    /* INT32_MIN parses exactly */
    memcpy(buf, "-2147483648", 12);
    assert(elib_ft_atoi32(buf, 12, &end) == INT32_MIN);
    assert(end == buf + 11);

    /* INT32_MIN - 1: overflow sentinel */
    memcpy(buf, "-2147483649", 12);
    assert(elib_ft_atoi32(buf, 12, &end) == 0);
    assert(end == NULL);

    /* way beyond int64 as well */
    memcpy(buf, "99999999999999999999", 21);
    assert(elib_ft_atoi32(buf, 21, &end) == 0);
    assert(end == NULL);
}

static void test_atou32_overflow(void)
{
    char buf[24];
    const char *end = buf;

    /* UINT32_MAX parses exactly */
    memcpy(buf, "4294967295", 11);
    assert(elib_ft_atou32(buf, 11, &end) == UINT32_MAX);
    assert(end == buf + 10);

    /* UINT32_MAX + 1: return 0 AND *endptr = NULL */
    memcpy(buf, "4294967296", 11);
    assert(elib_ft_atou32(buf, 11, &end) == 0);
    assert(end == NULL);

    /* no sign parsing: '-' yields zero digits, endptr stays non-NULL */
    memcpy(buf, "-1", 3);
    assert(elib_ft_atou32(buf, 3, &end) == 0);
    assert(end == buf);

    /* way beyond int64 as well */
    memcpy(buf, "99999999999999999999", 21);
    assert(elib_ft_atou32(buf, 21, &end) == 0);
    assert(end == NULL);
}

int main(void)
{
    RUN_TEST(test_strlen_normal);
    RUN_TEST(test_strlen_with_limit);
    RUN_TEST(test_strlen_null);
    RUN_TEST(test_strlen_empty);
    RUN_TEST(test_strcmp_equal);
    RUN_TEST(test_strcmp_less);
    RUN_TEST(test_strcmp_greater);
    RUN_TEST(test_strcmp_null_s1);
    RUN_TEST(test_strcmp_null_s2);
    RUN_TEST(test_strcmp_both_null);
    RUN_TEST(test_strncmp_equal);
    RUN_TEST(test_strncmp_prefix);
    RUN_TEST(test_strncmp_less);
    RUN_TEST(test_strncmp_greater);
    RUN_TEST(test_strncmp_zero_n);
    RUN_TEST(test_strncmp_null_s1);
    RUN_TEST(test_strncmp_null_s2);
    RUN_TEST(test_strncmp_null_both);
    RUN_TEST(test_strncmp_null_n);
    RUN_TEST(test_atoi32_normal);
    RUN_TEST(test_atoi32_negative);
    RUN_TEST(test_atoi32_positive_sign);
    RUN_TEST(test_atoi32_with_limit);
    RUN_TEST(test_atoi32_zero);
    RUN_TEST(test_atoi32_leading_spaces);
    RUN_TEST(test_atoi32_invalid);
    RUN_TEST(test_atoi32_null);
    RUN_TEST(test_atou32_normal);
    RUN_TEST(test_atou32_zero);
    RUN_TEST(test_atou32_with_limit);
    RUN_TEST(test_atou32_null);
    RUN_TEST(test_atof_normal);
    RUN_TEST(test_atof_negative);
    RUN_TEST(test_atof_integer);
    RUN_TEST(test_atof_with_limit);
    RUN_TEST(test_atof_null);
    RUN_TEST(test_strarg_count_normal);
    RUN_TEST(test_strarg_count_single);
    RUN_TEST(test_strarg_count_leading_spaces);
    RUN_TEST(test_strarg_count_trailing_spaces);
    RUN_TEST(test_strarg_count_multi_spaces);
    RUN_TEST(test_strarg_count_empty);
    RUN_TEST(test_strarg_count_null);
    RUN_TEST(test_strarg_get_normal);
    RUN_TEST(test_strarg_get_out_of_range);
    RUN_TEST(test_strarg_get_last);
    RUN_TEST(test_strarg_get_null);
    RUN_TEST(test_strarg_get_null_endptr);
    RUN_TEST(test_strcmp_fast_equal);
    RUN_TEST(test_strcmp_fast_less);
    RUN_TEST(test_strcmp_fast_greater);
    RUN_TEST(test_strcmp_fast_len_diff);
    RUN_TEST(test_strcmp_fast_null_s1);
    RUN_TEST(test_strcmp_fast_null_s2);
    RUN_TEST(test_strcmp_fast_both_null);
    RUN_TEST(test_strcmp_fast_max_len);
    RUN_TEST(test_strcmp_fast_max_len_diff);

    RUN_TEST(test_atoi32_overflow);
    RUN_TEST(test_atou32_overflow);
    printf("\nAll %d tests passed.\n", 59);
    return 0;
}
