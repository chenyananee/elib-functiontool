/* elib_ft_checksum_core.c - FunctionTool Checksum Core Implementation */

#include "elib_ft_checksum_core.h"

/* ------------------------------------------------------------------ */
/*  Sum8                                                               */
/* ------------------------------------------------------------------ */

void elib_ft_sum8_init(elib_ft_sum8_ctx_t *ctx)
{
    if (ctx == NULL) return;
    ctx->sum = 0;
}

void elib_ft_sum8_update(elib_ft_sum8_ctx_t *ctx, const uint8_t *data, uint32_t len)
{
    if (ctx == NULL || data == NULL) return;
    for (uint32_t i = 0; i < len; i++) {
        ctx->sum += data[i];
    }
}

uint8_t elib_ft_sum8_final(elib_ft_sum8_ctx_t *ctx)
{
    if (ctx == NULL) return 0;
    return (uint8_t)(ctx->sum & 0xFFu);
}

uint8_t elib_ft_sum8(const uint8_t *data, uint32_t len)
{
    elib_ft_sum8_ctx_t ctx;
    elib_ft_sum8_init(&ctx);
    elib_ft_sum8_update(&ctx, data, len);
    return elib_ft_sum8_final(&ctx);
}

/* ------------------------------------------------------------------ */
/*  CRC8                                                               */
/* ------------------------------------------------------------------ */

void elib_ft_crc8_init(elib_ft_crc8_ctx_t *ctx, uint8_t poly, uint8_t init_val, uint8_t xor_out, int ref)
{
    if (ctx == NULL) return;
    ctx->crc     = init_val;
    ctx->poly    = poly;
    ctx->xor_out = xor_out;
    ctx->ref     = (uint8_t)(ref & 1);
}

void elib_ft_crc8_update(elib_ft_crc8_ctx_t *ctx, const uint8_t *data, uint32_t len)
{
    if (ctx == NULL || data == NULL) return;

    uint8_t crc  = ctx->crc;
    uint8_t poly = ctx->poly;

    if (ctx->ref) {
        for (uint32_t i = 0; i < len; i++) {
            crc ^= data[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 0x01u)
                    crc = (crc >> 1) ^ poly;
                else
                    crc >>= 1;
            }
        }
    } else {
        for (uint32_t i = 0; i < len; i++) {
            crc ^= data[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 0x80u)
                    crc = (crc << 1) ^ poly;
                else
                    crc <<= 1;
            }
        }
    }

    ctx->crc = crc;
}

uint8_t elib_ft_crc8_final(elib_ft_crc8_ctx_t *ctx)
{
    if (ctx == NULL) return 0;
    return ctx->crc ^ ctx->xor_out;
}

uint8_t elib_ft_crc8(const uint8_t *data, uint32_t len, uint8_t poly, uint8_t init_val, uint8_t xor_out, int ref)
{
    elib_ft_crc8_ctx_t ctx;
    elib_ft_crc8_init(&ctx, poly, init_val, xor_out, ref);
    elib_ft_crc8_update(&ctx, data, len);
    return elib_ft_crc8_final(&ctx);
}

/* ------------------------------------------------------------------ */
/*  CRC16                                                              */
/* ------------------------------------------------------------------ */

void elib_ft_crc16_init(elib_ft_crc16_ctx_t *ctx, uint16_t poly, uint16_t init_val, uint16_t xor_out, int ref)
{
    if (ctx == NULL) return;
    ctx->crc     = init_val;
    ctx->poly    = poly;
    ctx->xor_out = xor_out;
    ctx->ref     = (uint16_t)(ref & 1);
}

void elib_ft_crc16_update(elib_ft_crc16_ctx_t *ctx, const uint8_t *data, uint32_t len)
{
    if (ctx == NULL || data == NULL) return;

    uint16_t crc  = ctx->crc;
    uint16_t poly = ctx->poly;

    if (ctx->ref) {
        for (uint32_t i = 0; i < len; i++) {
            crc ^= (uint16_t)data[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 0x0001u)
                    crc = (crc >> 1) ^ poly;
                else
                    crc >>= 1;
            }
        }
    } else {
        for (uint32_t i = 0; i < len; i++) {
            crc ^= (uint16_t)data[i] << 8;
            for (int j = 0; j < 8; j++) {
                if (crc & 0x8000u)
                    crc = (crc << 1) ^ poly;
                else
                    crc <<= 1;
            }
        }
    }

    ctx->crc = crc;
}

uint16_t elib_ft_crc16_final(elib_ft_crc16_ctx_t *ctx)
{
    if (ctx == NULL) return 0;
    return ctx->crc ^ ctx->xor_out;
}

uint16_t elib_ft_crc16(const uint8_t *data, uint32_t len, uint16_t poly, uint16_t init_val, uint16_t xor_out, int ref)
{
    elib_ft_crc16_ctx_t ctx;
    elib_ft_crc16_init(&ctx, poly, init_val, xor_out, ref);
    elib_ft_crc16_update(&ctx, data, len);
    return elib_ft_crc16_final(&ctx);
}

/* ------------------------------------------------------------------ */
/*  CRC32                                                              */
/* ------------------------------------------------------------------ */

void elib_ft_crc32_init(elib_ft_crc32_ctx_t *ctx, uint32_t poly, uint32_t init_val, uint32_t xor_out, int ref)
{
    if (ctx == NULL) return;
    ctx->crc     = init_val;
    ctx->poly    = poly;
    ctx->xor_out = xor_out;
    ctx->ref     = (uint32_t)(ref & 1);
}

void elib_ft_crc32_update(elib_ft_crc32_ctx_t *ctx, const uint8_t *data, uint32_t len)
{
    if (ctx == NULL || data == NULL) return;

    uint32_t crc  = ctx->crc;
    uint32_t poly = ctx->poly;

    if (ctx->ref) {
        for (uint32_t i = 0; i < len; i++) {
            crc ^= (uint32_t)data[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 0x00000001u)
                    crc = (crc >> 1) ^ poly;
                else
                    crc >>= 1;
            }
        }
    } else {
        for (uint32_t i = 0; i < len; i++) {
            crc ^= (uint32_t)data[i] << 24;
            for (int j = 0; j < 8; j++) {
                if (crc & 0x80000000u)
                    crc = (crc << 1) ^ poly;
                else
                    crc <<= 1;
            }
        }
    }

    ctx->crc = crc;
}

uint32_t elib_ft_crc32_final(elib_ft_crc32_ctx_t *ctx)
{
    if (ctx == NULL) return 0;
    return ctx->crc ^ ctx->xor_out;
}

uint32_t elib_ft_crc32(const uint8_t *data, uint32_t len, uint32_t poly, uint32_t init_val, uint32_t xor_out, int ref)
{
    elib_ft_crc32_ctx_t ctx;
    elib_ft_crc32_init(&ctx, poly, init_val, xor_out, ref);
    elib_ft_crc32_update(&ctx, data, len);
    return elib_ft_crc32_final(&ctx);
}
