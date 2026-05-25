/* elib_ft_endian_core.c - FunctionTool Endianness Operations */

#include "elib_ft_endian.h"

/* ------------------------------------------------------------------ */
/*  Byte swap                                                           */
/* ------------------------------------------------------------------ */

uint16_t elib_ft_swap16(uint16_t val)
{
    return (uint16_t)((val >> 8) | (val << 8));
}

uint32_t elib_ft_swap32(uint32_t val)
{
    return ((val >> 24) & 0x000000FFu) |
           ((val >>  8) & 0x0000FF00u) |
           ((val <<  8) & 0x00FF0000u) |
           ((val << 24) & 0xFF000000u);
}

/* ------------------------------------------------------------------ */
/*  Host to network / Network to host                                   */
/* ------------------------------------------------------------------ */

uint16_t elib_ft_hton16(uint16_t val)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    (void)0;
#else
    val = elib_ft_swap16(val);
#endif
    return val;
}

uint16_t elib_ft_ntoh16(uint16_t val)
{
    return elib_ft_hton16(val);
}

uint32_t elib_ft_hton32(uint32_t val)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    (void)0;
#else
    val = elib_ft_swap32(val);
#endif
    return val;
}

uint32_t elib_ft_ntoh32(uint32_t val)
{
    return elib_ft_hton32(val);
}