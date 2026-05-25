/* elib_ft_ringbuf_core.c - FunctionTool RingBuf Core Implementation */

#include "elib_ft_ringbuf_core.h"
#include <string.h>

elib_ft_err_t elib_ft_ringbuf_init(elib_ft_ringbuf_ctx_t *ctx, uint8_t *buf, uint32_t size, uint32_t cell_size)
{
    if (ctx == NULL || buf == NULL || size == 0 || cell_size == 0) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    ctx->buf = buf;
    ctx->size = size;
    ctx->cell_size = cell_size;
    ctx->head = 0;
    ctx->tail = 0;
    ctx->initialized = 1;

    return ELIB_FT_OK;
}

void elib_ft_ringbuf_deinit(elib_ft_ringbuf_ctx_t *ctx)
{
    if (ctx == NULL) {
        return;
    }
    ctx->initialized = 0;
}

static uint32_t ringbuf_used(const elib_ft_ringbuf_ctx_t *ctx)
{
    if (ctx->head >= ctx->tail) {
        return ctx->head - ctx->tail;
    }
    return ctx->size - (ctx->tail - ctx->head);
}

uint32_t elib_ft_ringbuf_used(elib_ft_ringbuf_ctx_t *ctx)
{
    if (ctx == NULL || !ctx->initialized) {
        return 0;
    }
    return ringbuf_used(ctx);
}

uint32_t elib_ft_ringbuf_free(elib_ft_ringbuf_ctx_t *ctx)
{
    if (ctx == NULL || !ctx->initialized) {
        return 0;
    }
    return ctx->size - 1 - ringbuf_used(ctx);
}

elib_ft_err_t elib_ft_ringbuf_write(elib_ft_ringbuf_ctx_t *ctx, const void *data, uint32_t len, uint32_t max_len)
{
    if (ctx == NULL || data == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!ctx->initialized) {
        return ELIB_FT_ERR_NOT_INITIALIZED;
    }
    if (len > max_len) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    if (len > elib_ft_ringbuf_free(ctx)) {
        return ELIB_FT_ERR_NO_SPACE;
    }

    const uint8_t *src = (const uint8_t *)data;
    uint32_t cell_size = ctx->cell_size;

    for (uint32_t i = 0; i < len; i++) {
        memcpy(&ctx->buf[ctx->head * cell_size], &src[i * cell_size], (size_t)cell_size);
        ctx->head = (ctx->head + 1) % ctx->size;
    }

    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_ringbuf_read(elib_ft_ringbuf_ctx_t *ctx, void *data, uint32_t len, uint32_t max_len)
{
    if (ctx == NULL || data == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (!ctx->initialized) {
        return ELIB_FT_ERR_NOT_INITIALIZED;
    }
    if (len > max_len) {
        return ELIB_FT_ERR_EXCEED_MAX;
    }
    if (len > ringbuf_used(ctx)) {
        return ELIB_FT_ERR_NO_DATA;
    }

    uint8_t *dst = (uint8_t *)data;
    uint32_t cell_size = ctx->cell_size;

    for (uint32_t i = 0; i < len; i++) {
        memcpy(&dst[i * cell_size], &ctx->buf[ctx->tail * cell_size], (size_t)cell_size);
        ctx->tail = (ctx->tail + 1) % ctx->size;
    }

    return ELIB_FT_OK;
}

void elib_ft_ringbuf_reset(elib_ft_ringbuf_ctx_t *ctx)
{
    if (ctx == NULL || !ctx->initialized) {
        return;
    }
    ctx->head = 0;
    ctx->tail = 0;
}
