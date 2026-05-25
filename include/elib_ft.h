/* elib_ft.h - FunctionTool Umbrella Header */

#ifndef ELIB_FT_H
#define ELIB_FT_H

/* ------------------------------------------------------------------ */
/*  Compiler compatibility macros                                      */
/* ------------------------------------------------------------------ */

#if defined(__ICCARM__)
    #define ELIB_FT_WEAK __weak
#elif defined(__CC_ARM)
    #define ELIB_FT_WEAK __weak
#elif defined(__GNUC__)
    #define ELIB_FT_WEAK __attribute__((weak))
#else
    #define ELIB_FT_WEAK
#endif

/* ------------------------------------------------------------------ */
/*  Sub-modules                                                        */
/* ------------------------------------------------------------------ */

#include "elib_ft_err.h"
#include "elib_ft_mem.h"
#include "elib_ft_ringbuf.h"
#include "elib_ft_list.h"
#include "elib_ft_checksum.h"
#include "elib_ft_str.h"
#include "elib_ft_endian.h"

#endif /* ELIB_FT_H */
