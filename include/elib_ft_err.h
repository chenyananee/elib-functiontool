/* elib_ft_err.h - FunctionTool Error Codes */

#ifndef ELIB_FT_ERR_H
#define ELIB_FT_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ELIB_FT_OK = 0,
    ELIB_FT_ERR_INVALID_PARAM,
    ELIB_FT_ERR_NOT_INITIALIZED,
    ELIB_FT_ERR_EXCEED_MAX,
    ELIB_FT_ERR_NO_SPACE,
    ELIB_FT_ERR_NO_DATA,
} elib_ft_err_t;

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_ERR_H */
