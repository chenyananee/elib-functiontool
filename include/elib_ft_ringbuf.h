/* elib_ft_ringbuf.h - FunctionTool Ring Buffer */

#ifndef ELIB_FT_RINGBUF_H
#define ELIB_FT_RINGBUF_H

#include "elib_ft_err.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t  *buf;        /* user-provided buffer */
    uint32_t  size;       /* buffer capacity in cells */
    uint32_t  cell_size;  /* size of each cell in bytes */
    uint32_t  head;       /* write index */
    uint32_t  tail;       /* read index */
    struct {
        uint8_t initialized : 1;
    } bit_flags;
} elib_ft_ringbuf_ctx_t;

/**
 * @brief Initialize ring buffer
 * @param ctx User-allocated context
 * @param buf User-provided buffer
 * @param size Buffer capacity in cells
 * @param cell_size Size of each cell in bytes
 * @return ELIB_FT_OK on success
 */
elib_ft_err_t elib_ft_ringbuf_init(elib_ft_ringbuf_ctx_t *ctx, uint8_t *buf, uint32_t size, uint32_t cell_size);

/**
 * @brief Deinitialize ring buffer
 * @param ctx Ring buffer context
 */
void elib_ft_ringbuf_deinit(elib_ft_ringbuf_ctx_t *ctx);

/**
 * @brief Get number of used cells
 * @param ctx Ring buffer context
 * @return Used cell count (0 if not initialized)
 */
uint32_t elib_ft_ringbuf_used(elib_ft_ringbuf_ctx_t *ctx);

/**
 * @brief Get number of free cells
 * @param ctx Ring buffer context
 * @return Free cell count (0 if not initialized)
 */
uint32_t elib_ft_ringbuf_free(elib_ft_ringbuf_ctx_t *ctx);

/**
 * @brief Write data to ring buffer
 * @param ctx Ring buffer context
 * @param data Data to write
 * @param len Number of cells to write
 * @param max_len Maximum allowed cells to write
 * @return ELIB_FT_OK on success, error code otherwise
 */
elib_ft_err_t elib_ft_ringbuf_write(elib_ft_ringbuf_ctx_t *ctx, const void *data, uint32_t len, uint32_t max_len);

/**
 * @brief Read data from ring buffer
 * @param ctx Ring buffer context
 * @param data Buffer to store read data
 * @param len Number of cells to read
 * @param max_len Maximum allowed cells to read
 * @return ELIB_FT_OK on success, error code otherwise
 */
elib_ft_err_t elib_ft_ringbuf_read(elib_ft_ringbuf_ctx_t *ctx, void *data, uint32_t len, uint32_t max_len);

/**
 * @brief Reset ring buffer (discard all data)
 * @param ctx Ring buffer context
 */
void elib_ft_ringbuf_reset(elib_ft_ringbuf_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_RINGBUF_H */
