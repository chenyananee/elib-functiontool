/* elib_ft_str_core.c - FunctionTool String Operations */
/* elib_ft_checksum.h included via elib_ft_str.h */

#include "elib_ft_str.h"
#include <string.h>
#include <stdlib.h>

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

/* ------------------------------------------------------------------ */
/*  String to number                                                    */
/* ------------------------------------------------------------------ */

static const char *skip_spaces(const char *s, uint32_t max_len, uint32_t *idx)
{
    uint32_t i = 0;
    while (i < max_len && s[i] == ' ') {
        i++;
    }
    *idx = i;
    return s;
}

int32_t elib_ft_atoi32(const char *s, uint32_t max_len, const char **endptr)
{
    uint32_t i = 0;
    if (s == NULL) {
        if (endptr != NULL) {
            *endptr = NULL;
        }
        return 0;
    }
    s = skip_spaces(s, max_len, &i);

    int sign = 1;
    if (i < max_len && (s[i] == '+' || s[i] == '-')) {
        sign = (s[i] == '-') ? -1 : 1;
        i++;
    }

    char buf[12];
    uint32_t j = 0;
    while (i < max_len && s[i] >= '0' && s[i] <= '9' && j < 11) {
        buf[j++] = s[i++];
    }
    buf[j] = '\0';

    if (j == 0) {
        if (endptr != NULL) {
            *endptr = (i < max_len) ? &s[i] : NULL;
        }
        return 0;
    }

    char *end = NULL;
    long val = strtol(buf, &end, 10);
    if (endptr != NULL) {
        *endptr = &s[i];
    }
    return (int32_t)(val * sign);
}

uint32_t elib_ft_atou32(const char *s, uint32_t max_len, const char **endptr)
{
    uint32_t i = 0;
    if (s == NULL) {
        if (endptr != NULL) {
            *endptr = NULL;
        }
        return 0;
    }
    s = skip_spaces(s, max_len, &i);

    char buf[11];
    uint32_t j = 0;
    while (i < max_len && s[i] >= '0' && s[i] <= '9' && j < 10) {
        buf[j++] = s[i++];
    }
    buf[j] = '\0';

    if (j == 0) {
        if (endptr != NULL) {
            *endptr = (i < max_len) ? &s[i] : NULL;
        }
        return 0;
    }

    char *end = NULL;
    unsigned long val = strtoul(buf, &end, 10);
    if (endptr != NULL) {
        *endptr = &s[i];
    }
    return (uint32_t)val;
}

double elib_ft_atof(const char *s, uint32_t max_len, const char **endptr)
{
    uint32_t i = 0;
    if (s == NULL) {
        if (endptr != NULL) {
            *endptr = NULL;
        }
        return 0.0;
    }
    s = skip_spaces(s, max_len, &i);

    /* copy up to 64 chars into temp buffer for strtod */
    char buf[65];
    uint32_t j = 0;
    while (i < max_len && j < 64) {
        char c = s[i++];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            break;
        }
        buf[j++] = c;
    }
    buf[j] = '\0';

    if (j == 0) {
        if (endptr != NULL) {
            *endptr = NULL;
        }
        return 0.0;
    }

    char *end = NULL;
    double val = strtod(buf, &end);
    if (endptr != NULL) {
        /* map back: end chars consumed within buf */
        uint32_t consumed = (uint32_t)(end - buf);
        uint32_t src_idx = i - (j - consumed);
        if (src_idx < max_len) {
            *endptr = &s[src_idx];
        } else {
            *endptr = NULL;
        }
    }
    return val;
}

/* ------------------------------------------------------------------ */
/*  String argument table (space-separated)                             */
/* ------------------------------------------------------------------ */

/* skip leading spaces, return index of first non-space char within [0, max_len) */
static uint32_t skip_spaces_buf(const char *s, uint32_t max_len)
{
    uint32_t i = 0;
    while (i < max_len && s[i] == ' ') {
        i++;
    }
    return i;
}

uint32_t elib_ft_strarg_count(const char *s, uint32_t max_len)
{
    if (s == NULL || max_len == 0) {
        return 0;
    }
    uint32_t count = 0;
    uint32_t i = skip_spaces_buf(s, max_len);
    while (i < max_len && s[i] != '\0') {
        count++;
        /* skip token */
        while (i < max_len && s[i] != '\0' && s[i] != ' ') {
            i++;
        }
        /* skip trailing spaces */
        i = skip_spaces_buf(s + i, max_len - i) + i;
    }
    return count;
}

const char *elib_ft_strarg_get(const char *s, uint32_t max_len, uint32_t index, const char **endptr)
{
    if (s == NULL || max_len == 0) {
        if (endptr != NULL) {
            *endptr = NULL;
        }
        return NULL;
    }
    uint32_t i = skip_spaces_buf(s, max_len);
    while (i < max_len && s[i] != '\0') {
        if (index == 0) {
            const char *tok = &s[i];
            uint32_t j = i;
            while (j < max_len && s[j] != '\0' && s[j] != ' ') {
                j++;
            }
            if (endptr != NULL) {
                if (j < max_len && s[j] != '\0') {
                    *endptr = &s[j];
                } else {
                    *endptr = NULL;
                }
            }
            return tok;
        }
        index--;
        while (i < max_len && s[i] != '\0' && s[i] != ' ') {
            i++;
        }
        i = skip_spaces_buf(s + i, max_len - i) + i;
    }
    if (endptr != NULL) {
        *endptr = NULL;
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  Fast string compare (hash-based)                                    */
/* ------------------------------------------------------------------ */

int elib_ft_strcmp_fast(const char *s1, const char *s2, uint32_t max_len)
{
    if (s1 == NULL || s2 == NULL) {
        if (s1 == NULL && s2 == NULL) {
            return 0;
        }
        return (s1 == NULL) ? -1 : 1;
    }
    if (s1 == s2) {
        return 0;
    }

    /* find actual length within max_len */
    uint32_t len1 = strnlen(s1, max_len);
    uint32_t len2 = strnlen(s2, max_len);

    /* length difference -> determine sign */
    if (len1 != len2) {
        return (len1 < len2) ? -1 : 1;
    }

    /* same length: compute FNV-1a hash */
    uint32_t h1 = elib_ft_hash_fnv1a32((const uint8_t *)s1, len1);
    uint32_t h2 = elib_ft_hash_fnv1a32((const uint8_t *)s2, len2);

    if (h1 != h2) {
        return (h1 < h2) ? -1 : 1;
    }

    /* hash collision possible (rare) -> confirm with memcmp */
    int cmp = memcmp(s1, s2, (size_t)len1);
    return cmp;
}