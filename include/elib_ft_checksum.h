/* elib_ft_checksum.h - FunctionTool Checksum Algorithms */

#ifndef ELIB_FT_CHECKSUM_H
#define ELIB_FT_CHECKSUM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Sum8                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    uint32_t sum;
} elib_ft_sum8_ctx_t;

void    elib_ft_sum8_init(elib_ft_sum8_ctx_t *ctx);
void    elib_ft_sum8_update(elib_ft_sum8_ctx_t *ctx, const uint8_t *data, uint32_t len);
uint8_t elib_ft_sum8_final(elib_ft_sum8_ctx_t *ctx);

uint8_t elib_ft_sum8(const uint8_t *data, uint32_t len);

/* ------------------------------------------------------------------ */
/*  CRC8                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t  crc;
    uint8_t  poly;
    uint8_t  xor_out;
    uint8_t  ref : 1;    /* 0=MSB-first, 1=LSB-first */
} elib_ft_crc8_ctx_t;

void    elib_ft_crc8_init(elib_ft_crc8_ctx_t *ctx, uint8_t poly, uint8_t init_val, uint8_t xor_out, int ref);
void    elib_ft_crc8_update(elib_ft_crc8_ctx_t *ctx, const uint8_t *data, uint32_t len);
uint8_t elib_ft_crc8_final(elib_ft_crc8_ctx_t *ctx);

uint8_t elib_ft_crc8(const uint8_t *data, uint32_t len, uint8_t poly, uint8_t init_val, uint8_t xor_out, int ref);

#define ELIB_FT_CRC8_CCITT_INIT(ctx)    elib_ft_crc8_init(ctx, 0x07, 0x00, 0x00, 0)
#define ELIB_FT_CRC8_MAXIM_INIT(ctx)    elib_ft_crc8_init(ctx, 0x8C, 0x00, 0x00, 1)

/* ------------------------------------------------------------------ */
/*  CRC16                                                              */
/* ------------------------------------------------------------------ */

typedef struct {
    uint16_t crc;
    uint16_t poly;
    uint16_t xor_out;
    uint16_t ref : 1;    /* 0=MSB-first, 1=LSB-first */
} elib_ft_crc16_ctx_t;

void     elib_ft_crc16_init(elib_ft_crc16_ctx_t *ctx, uint16_t poly, uint16_t init_val, uint16_t xor_out, int ref);
void     elib_ft_crc16_update(elib_ft_crc16_ctx_t *ctx, const uint8_t *data, uint32_t len);
uint16_t elib_ft_crc16_final(elib_ft_crc16_ctx_t *ctx);

uint16_t elib_ft_crc16(const uint8_t *data, uint32_t len, uint16_t poly, uint16_t init_val, uint16_t xor_out, int ref);

#define ELIB_FT_CRC16_XMODEM_INIT(ctx)  elib_ft_crc16_init(ctx, 0x1021, 0x0000, 0x0000, 0)
#define ELIB_FT_CRC16_CCITT_INIT(ctx)   elib_ft_crc16_init(ctx, 0x1021, 0xFFFF, 0x0000, 0)
#define ELIB_FT_CRC16_MODBUS_INIT(ctx)  elib_ft_crc16_init(ctx, 0xA001, 0xFFFF, 0x0000, 1)

/* ------------------------------------------------------------------ */
/*  CRC32                                                              */
/* ------------------------------------------------------------------ */

typedef struct {
    uint32_t crc;
    uint32_t poly;
    uint32_t xor_out;
    uint32_t ref : 1;    /* 0=MSB-first, 1=LSB-first */
} elib_ft_crc32_ctx_t;

void     elib_ft_crc32_init(elib_ft_crc32_ctx_t *ctx, uint32_t poly, uint32_t init_val, uint32_t xor_out, int ref);
void     elib_ft_crc32_update(elib_ft_crc32_ctx_t *ctx, const uint8_t *data, uint32_t len);
uint32_t elib_ft_crc32_final(elib_ft_crc32_ctx_t *ctx);

uint32_t elib_ft_crc32(const uint8_t *data, uint32_t len, uint32_t poly, uint32_t init_val, uint32_t xor_out, int ref);

#define ELIB_FT_CRC32_ETHERNET_INIT(ctx) elib_ft_crc32_init(ctx, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1)

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_CHECKSUM_H */
