/* elib_ft_str_core.c - FunctionTool String Operations */

#include "elib_ft_str.h"
#include <string.h>

uint32_t elib_ft_strlen(const char *s, uint32_t max_len)
{
    if (s == NULL) {
        return 0;
    }
    return strnlen(s, max_len);
}

int elib_ft_strcmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) {
        if (s1 == NULL && s2 == NULL) {
            return 0;
        }
        return (s1 == NULL) ? -1 : 1;
    }
    return strcmp(s1, s2);
}

int elib_ft_strncmp(const char *s1, const char *s2, uint32_t n)
{
    if (n == 0) {
        return 0;
    }
    if (s1 == NULL || s2 == NULL) {
        if (s1 == NULL && s2 == NULL) {
            return 0;
        }
        return (s1 == NULL) ? -1 : 1;
    }
    return strncmp(s1, s2, (size_t)n);
}

/* atoi: no direct standard equivalent (strtoul lacks max_len), keep custom */
uint32_t elib_ft_atoi(const char *s, uint32_t max_len, const char **endptr)
{
    uint32_t val = 0;
    uint32_t i = 0;
    if (s == NULL) {
        if (endptr != NULL) {
            *endptr = NULL;
        }
        return 0;
    }
    while (i < max_len && s[i] == ' ') {
        i++;
    }
    if (i >= max_len || s[i] < '0' || s[i] > '9') {
        if (endptr != NULL) {
            *endptr = (i < max_len) ? &s[i] : NULL;
        }
        return 0;
    }
    while (i < max_len && s[i] >= '0' && s[i] <= '9') {
        val = val * 10 + (uint32_t)(s[i] - '0');
        i++;
    }
    if (endptr != NULL) {
        *endptr = &s[i];
    }
    return val;
}