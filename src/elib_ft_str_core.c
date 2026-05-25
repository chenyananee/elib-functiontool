/* elib_ft_str_core.c - FunctionTool String Operations */

#include "elib_ft_str.h"

uint32_t elib_ft_strlen(const char *s, uint32_t max_len)
{
    uint32_t len = 0;
    if (s == NULL) {
        return 0;
    }
    while (len < max_len && s[len] != '\0') {
        len++;
    }
    return len;
}

int elib_ft_strcmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) {
        if (s1 == NULL && s2 == NULL) {
            return 0;
        }
        return (s1 == NULL) ? -1 : 1;
    }
    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 != *s2) {
            return (unsigned char)*s1 - (unsigned char)*s2;
        }
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
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
    uint32_t i = 0;
    while (i < n && s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        i++;
    }
    if (i == n) {
        return 0;
    }
    return (unsigned char)s1[i] - (unsigned char)s2[i];
}

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