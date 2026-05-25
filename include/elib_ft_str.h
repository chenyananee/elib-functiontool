/* elib_ft_str.h - FunctionTool String Operations */

#ifndef ELIB_FT_STR_H
#define ELIB_FT_STR_H

#include <stdint.h>
#include <stddef.h>

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

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_STR_H */
