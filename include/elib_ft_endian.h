/* elib_ft_endian.h - FunctionTool Endianness Operations */

#ifndef ELIB_FT_ENDIAN_H
#define ELIB_FT_ENDIAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Byte swap                                                           */
/* ------------------------------------------------------------------ */

/**
 * @brief Swap bytes of a 16-bit value
 * @param val Input value
 * @return Byte-swapped value
 */
uint16_t elib_ft_swap16(uint16_t val);

/**
 * @brief Swap bytes of a 32-bit value
 * @param val Input value
 * @return Byte-swapped value
 */
uint32_t elib_ft_swap32(uint32_t val);

/* ------------------------------------------------------------------ */
/*  Host to network / Network to host                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief Convert 16-bit value from host byte order to network byte order
 * @param val Host value
 * @return Network value (BE)
 */
uint16_t elib_ft_hton16(uint16_t val);

/**
 * @brief Convert 16-bit value from network byte order to host byte order
 * @param val Network value (BE)
 * @return Host value
 */
uint16_t elib_ft_ntoh16(uint16_t val);

/**
 * @brief Convert 32-bit value from host byte order to network byte order
 * @param val Host value
 * @return Network value (BE)
 */
uint32_t elib_ft_hton32(uint32_t val);

/**
 * @brief Convert 32-bit value from network byte order to host byte order
 * @param val Network value (BE)
 * @return Host value
 */
uint32_t elib_ft_ntoh32(uint32_t val);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_ENDIAN_H */
