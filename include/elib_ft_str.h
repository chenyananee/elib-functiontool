/* elib_ft_str.h - FunctionTool String Operations */

#ifndef ELIB_FT_STR_H
#define ELIB_FT_STR_H

#include <stdint.h>
#include <stddef.h>
#include "elib_ft_checksum.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  String length                                                      */
/* ------------------------------------------------------------------ */

/**
 * @brief Get string length with max limit
 * @param s String pointer
 * @param max_len Maximum allowed length
 * @return Actual string length (0 on NULL)
 */
uint32_t elib_ft_strlen(const char *s, uint32_t max_len);

/* ------------------------------------------------------------------ */
/*  String compare                                                      */
/* ------------------------------------------------------------------ */

/**
 * @brief Compare two strings with max length limit
 * @param s1 First string
 * @param s2 Second string
 * @param max_len Maximum characters to compare
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int elib_ft_strcmp(const char *s1, const char *s2);

/**
 * @brief Compare two strings up to n characters
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum characters to compare
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int elib_ft_strncmp(const char *s1, const char *s2, uint32_t n);

/* ------------------------------------------------------------------ */
/*  String to integer                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief Convert string to signed 32-bit integer
 * @param s String pointer
 * @param max_len Maximum characters to parse
 * @param endptr Output pointer for first unparsed character (can be NULL)
 * @return Parsed value, 0 on error
 */
int32_t elib_ft_atoi32(const char *s, uint32_t max_len, const char **endptr);

/**
 * @brief Convert string to unsigned 32-bit integer
 * @param s String pointer
 * @param max_len Maximum characters to parse
 * @param endptr Output pointer for first unparsed character (can be NULL)
 * @return Parsed value, 0 on error
 */
uint32_t elib_ft_atou32(const char *s, uint32_t max_len, const char **endptr);

/**
 * @brief Convert string to double (floating point)
 * @param s String pointer
 * @param max_len Maximum characters to parse
 * @param endptr Output pointer for first unparsed character (can be NULL)
 * @return Parsed value, 0.0 on error
 */
double elib_ft_atof(const char *s, uint32_t max_len, const char **endptr);

/* ------------------------------------------------------------------ */
/*  String argument table (space-separated)                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Count space-separated tokens in a string
 * @param s String pointer
 * @param max_len Maximum characters to scan
 * @return Token count (0 on NULL, empty, or spaces-only input)
 */
uint32_t elib_ft_strarg_count(const char *s, uint32_t max_len);

/**
 * @brief Get pointer to token at given index
 * @param s String pointer
 * @param max_len Maximum characters to scan
 * @param index Zero-based token index
 * @param endptr Output pointer for first character after token (can be NULL)
 * @return Pointer to token start, or NULL if not found / NULL input
 */
const char *elib_ft_strarg_get(const char *s, uint32_t max_len, uint32_t index, const char **endptr);

/* ------------------------------------------------------------------ */
/*  Fast string compare (hash-based)                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief Quick string compare using FNV-1a hash pre-check
 * @param s1 First string
 * @param s2 Second string
 * @param max_len Maximum characters to compare
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 *
 * Algorithm:
 *   - If lengths differ -> immediately return comparison result
 *   - Compute FNV-1a hash of both strings
 *   - If hashes differ  -> return difference (fast O(n) skip)
 *   - If hashes equal   -> fallback to memcmp for confirmation
 *
 * Benefits: O(n) worst case, often O(n/8) when strings differ early.
 *          No dynamic allocation, no NULL pointer dereference.
 */
int elib_ft_strcmp_fast(const char *s1, const char *s2, uint32_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_STR_H */
