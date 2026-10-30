/* elib_ft.h - FunctionTool utility library, single umbrella header.
 *
 * Ten independent modules:
 *   err       shared error codes (elib_ft_err_t)
 *   util      shortcut macros (MIN/MAX/CLAMP/MAP/BIT/CONTAINER_OF/...)
 *   mem       bounded memcpy/memset
 *   bitmap    bit array ops (set/clear/shift/find + tail-bit safety)
 *   ringbuf   cell-based ring buffer with initialized guard
 *   list      intrusive doubly-linked list (owner tag + O(1) tail)
 *   checksum  Sum8 / CRC8 / CRC16 / CRC32 / FNV-1a (init-guarded ctx)
 *   str       bounded strings + parsing (overflow via NULL endptr)
 *   endian    byte-swap / host-network conversions
 *   time      calendar <-> Unix epoch (validated year range)
 *   qmath     Qm.n fixed-point (16/32/64, signed/unsigned)
 *
 * Rules of the game:
 *  - zero dynamic memory: every context is user allocated and self
 *    contained; no globals anywhere in the library
 *  - configuration is validated ONCE at init and rejected there;
 *    APIs called before a valid init return ELIB_FT_ERR_NOT_INITIALIZED
 *    (or a documented sentinel such as 0 / NULL)
 *  - `_s` variants of qmath and all context-based APIs return
 *    elib_ft_err_t; the plain variants are the fast path and return a
 *    VALUE -- their non-validated domains are documented per function
 *    (caller guarantees, typically "no overflow by construction")
 *  - C99, no external dependencies, no build system: compile every
 *    source file under "src" with -Iinclude
 *
 * Version: 2.0.0
 */
#ifndef ELIB_FT_H
#define ELIB_FT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELIB_FT_VERSION "2.0.0"

/* ================================================================
 * err -- shared error codes
 * ================================================================ */

typedef enum {
    ELIB_FT_OK = 0,
    ELIB_FT_ERR_INVALID_PARAM,   /* bad argument (NULL / out of range) */
    ELIB_FT_ERR_NOT_INITIALIZED, /* API called before a valid init */
    ELIB_FT_ERR_EXCEED_MAX,      /* value or range exceeds the cap */
    ELIB_FT_ERR_NO_SPACE,        /* destination has no room */
    ELIB_FT_ERR_NO_DATA          /* nothing to return (detached node / empty read) */
} elib_ft_err_t;

/* ================================================================
 * util -- shortcut macros
 * ================================================================ */

/* compiler compatibility */
#if defined(__ICCARM__)
    #define ELIB_FT_WEAK __weak
    #define ELIB_FT_UNUSED __attribute__((unused))
#elif defined(__CC_ARM)
    #define ELIB_FT_WEAK __weak
    #define ELIB_FT_UNUSED __attribute__((unused))
#elif defined(__GNUC__)
    #define ELIB_FT_WEAK __attribute__((weak))
    #define ELIB_FT_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
    #define ELIB_FT_WEAK
    #define ELIB_FT_UNUSED __pragma(warning(suppress:4100))
#else
    #define ELIB_FT_WEAK
    #define ELIB_FT_UNUSED
#endif

/**
 * @brief Get number of elements in an array
 * @param arr Array variable (not a pointer)
 * @return Element count as uint32_t
 */
#define ELIB_FT_ARRAY_SIZE(arr) ((uint32_t)(sizeof(arr) / sizeof((arr)[0])))

/** @brief Generate a single-bit mask at position n */
#define ELIB_FT_BIT(n) (1U << (n))

/**
 * @brief Get containing struct from a member pointer
 * @param ptr    Pointer to the member
 * @param type   Name of the containing struct
 * @param member Name of the member within the struct
 * @return Pointer to the containing struct
 */
#define ELIB_FT_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - (char *)(&((type *)0)->member)))

/** @brief Return the smaller of two values */
#define ELIB_FT_MIN(a, b)  ((a) < (b) ? (a) : (b))

/** @brief Return the larger of two values */
#define ELIB_FT_MAX(a, b)  ((a) > (b) ? (a) : (b))

/** @brief Clamp value to [lo, hi] range */
#define ELIB_FT_CLAMP(val, lo, hi)  ELIB_FT_MIN(ELIB_FT_MAX(val, lo), hi)

/**
 * @brief Integer linear mapping with rounding
 * @param val    Input value
 * @param in_lo  Input range lower bound
 * @param in_hi  Input range upper bound
 * @param out_lo Output range lower bound
 * @param out_hi Output range upper bound
 * @return Mapped value (rounded to nearest integer)
 *
 * Maps val from [in_lo, in_hi] to [out_lo, out_hi].
 * Uses rounding: +((in_hi - in_lo) / 2) before division.
 */
#define ELIB_FT_MAP(val, in_lo, in_hi, out_lo, out_hi) \
    ((out_lo) + (((val) - (in_lo)) * ((out_hi) - (out_lo)) + \
     ((in_hi) - (in_lo)) / 2) / ((in_hi) - (in_lo)))

/**
 * @brief Floating-point linear mapping
 * @param val    Input value
 * @param in_lo  Input range lower bound
 * @param in_hi  Input range upper bound
 * @param out_lo Output range lower bound
 * @param out_hi Output range upper bound
 * @return Mapped value (double precision)
 */
#define ELIB_FT_MAP_F(val, in_lo, in_hi, out_lo, out_hi) \
    ((out_lo) + (double)((val) - (in_lo)) * ((out_hi) - (out_lo)) / ((in_hi) - (in_lo)))

/* ================================================================
 * mem -- bounded memory copy / fill
 * ================================================================ */

/**
 * @brief Copy memory with max length limit
 * @param dst Destination buffer
 * @param src Source buffer
 * @param len Requested copy length in bytes
 * @param max_len Maximum allowed length in bytes; truncates if len exceeds
 * @return Actual number of bytes copied (0 on NULL or zero length)
 */
uint32_t elib_ft_memcpy(void *dst, const void *src, uint32_t len, uint32_t max_len);

/**
 * @brief Fill memory with a byte value, with max length limit
 * @param dst Destination buffer
 * @param val Byte value to fill
 * @param len Requested fill length in bytes
 * @param max_len Maximum allowed length in bytes; truncates if len exceeds
 * @return Actual number of bytes set (0 on NULL or zero length)
 */
uint32_t elib_ft_memset(void *dst, uint8_t val, uint32_t len, uint32_t max_len);

/* ================================================================
 * bitmap -- bit array operations
 *
 * buf is a user-provided uint8_t array of ceil(max_bits / 8) bytes.
 * Bits above max_bits in the last byte are always kept cleared.
 * ================================================================ */

/**
 * @brief Set a bit in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param bit Bit index to set
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 *         buf or bit >= max_bits
 */
elib_ft_err_t elib_ft_bitmap_set(uint8_t *buf, uint32_t max_bits, uint32_t bit);

/**
 * @brief Clear a bit in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param bit Bit index to clear
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 *         buf or bit >= max_bits
 */
elib_ft_err_t elib_ft_bitmap_clear(uint8_t *buf, uint32_t max_bits, uint32_t bit);

/**
 * @brief Get a bit value from bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param bit Bit index to query
 * @return 1 if set, 0 if clear or error (NULL buf, bit >= max_bits)
 */
uint32_t elib_ft_bitmap_get(const uint8_t *buf, uint32_t max_bits, uint32_t bit);

/**
 * @brief Set all bits in bitmap (bits above max_bits stay cleared)
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 */
void elib_ft_bitmap_set_all(uint8_t *buf, uint32_t max_bits);

/**
 * @brief Clear all bits in bitmap
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 */
void elib_ft_bitmap_clear_all(uint8_t *buf, uint32_t max_bits);

/**
 * @brief Left shift bitmap by n bits toward higher indexes (MSB side,
 *        bits shifted out of max_bits are lost)
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param n Number of bits to shift (n >= max_bits clears everything)
 */
void elib_ft_bitmap_shift_left(uint8_t *buf, uint32_t max_bits, uint32_t n);

/**
 * @brief Right shift bitmap by n bits toward lower indexes (LSB side,
 *        low bits are lost)
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param n Number of bits to shift (n >= max_bits clears everything)
 */
void elib_ft_bitmap_shift_right(uint8_t *buf, uint32_t max_bits, uint32_t n);

/**
 * @brief Find first set bit (1) from start index
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param start Bit index to start searching from
 * @return Index of first 1 bit, or max_bits if not found / NULL buf /
 *         start >= max_bits
 */
uint32_t elib_ft_bitmap_find_first_set(const uint8_t *buf, uint32_t max_bits, uint32_t start);

/**
 * @brief Find first clear bit (0) from start index
 * @param buf uint8_t array pointer
 * @param max_bits Maximum bit count
 * @param start Bit index to start searching from
 * @return Index of first 0 bit, or max_bits if not found / NULL buf /
 *         start >= max_bits
 */
uint32_t elib_ft_bitmap_find_first_clear(const uint8_t *buf, uint32_t max_bits, uint32_t start);

/* ================================================================
 * ringbuf -- cell-based ring buffer
 *
 * buf is a user-provided byte array of size * cell_size bytes.
 * One slot stays unused (size-1 cells usable) so that full and
 * empty states stay distinguishable by head/tail alone.
 * ================================================================ */

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
 * @brief Initialize ring buffer (validates and memset-clears ctx)
 * @param ctx User-allocated context
 * @param buf User-provided buffer
 * @param size Buffer capacity in cells (must be >= 2)
 * @param cell_size Size of each cell in bytes
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM otherwise
 */
elib_ft_err_t elib_ft_ringbuf_init(elib_ft_ringbuf_ctx_t *ctx, uint8_t *buf, uint32_t size, uint32_t cell_size);

/**
 * @brief Deinitialize ring buffer (clears the initialized guard)
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
 * @param max_len Maximum allowed cells per call
 * @return ELIB_FT_OK, or ELIB_FT_ERR_INVALID_PARAM /
 *         ELIB_FT_ERR_NOT_INITIALIZED / ELIB_FT_ERR_EXCEED_MAX /
 *         ELIB_FT_ERR_NO_SPACE
 */
elib_ft_err_t elib_ft_ringbuf_write(elib_ft_ringbuf_ctx_t *ctx, const void *data, uint32_t len, uint32_t max_len);

/**
 * @brief Read data from ring buffer
 * @param ctx Ring buffer context
 * @param data Buffer to store read data
 * @param len Number of cells to read
 * @param max_len Maximum allowed cells per call
 * @return ELIB_FT_OK, or ELIB_FT_ERR_INVALID_PARAM /
 *         ELIB_FT_ERR_NOT_INITIALIZED / ELIB_FT_ERR_EXCEED_MAX /
 *         ELIB_FT_ERR_NO_DATA
 */
elib_ft_err_t elib_ft_ringbuf_read(elib_ft_ringbuf_ctx_t *ctx, void *data, uint32_t len, uint32_t max_len);

/**
 * @brief Reset ring buffer (discard all data)
 * @param ctx Ring buffer context
 */
void elib_ft_ringbuf_reset(elib_ft_ringbuf_ctx_t *ctx);

/* ================================================================
 * list -- intrusive doubly-linked list
 *
 * Nodes carry an owner tag: the library knows which list a node is
 * linked into, double insertion and cross-list removal are rejected
 * instead of corrupting the chain. A cached tail pointer makes
 * push_back / pop_back O(1).
 * ================================================================ */

typedef struct elib_ft_list elib_ft_list_t;

typedef struct elib_ft_list_node {
    struct elib_ft_list_node *next;
    struct elib_ft_list_node *prev;
    elib_ft_list_t           *owner;   /* list the node is linked into, NULL when free */
} elib_ft_list_node_t;

struct elib_ft_list {
    elib_ft_list_node_t *head;
    elib_ft_list_node_t *tail;     /* cached last node, NULL when empty */
    uint32_t             count;
};

/**
 * @brief Get the containing struct from a list node pointer
 * @param node Pointer to the elib_ft_list_node_t member
 * @param type Name of the containing struct
 * @param member Name of the elib_ft_list_node_t member within the struct
 * @return Pointer to the containing struct
 */
#define ELIB_FT_LIST_ENTRY(node, type, member) \
    ELIB_FT_CONTAINER_OF(node, type, member)

/**
 * @brief Traverse a list from head to tail
 * @param cursor elib_ft_list_node_t * loop cursor
 * @param list  elib_ft_list_t * list to traverse
 */
#define ELIB_FT_LIST_FOR_EACH(cursor, list) \
    for ((cursor) = (list)->head; (cursor) != NULL; (cursor) = (cursor)->next)

/**
 * @brief Traverse a list while allowing the cursor node to be removed
 * @param cursor elib_ft_list_node_t * loop cursor
 * @param tmp    elib_ft_list_node_t * scratch pointer
 * @param list  elib_ft_list_t * list to traverse
 */
#define ELIB_FT_LIST_FOR_EACH_SAFE(cursor, tmp, list) \
    for ((cursor) = (list)->head, (tmp) = ((cursor) != NULL ? (cursor)->next : NULL); \
         (cursor) != NULL; \
         (cursor) = (tmp), (tmp) = ((cursor) != NULL ? (cursor)->next : NULL))

/**
 * @brief Initialize a list (validates and memset-clears)
 * @param list Pointer to the list
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 */
elib_ft_err_t elib_ft_list_init(elib_ft_list_t *list);

/**
 * @brief Check if a list is empty
 * @param list Pointer to the list
 * @return 1 if empty (or NULL), 0 if not
 */
int elib_ft_list_empty(const elib_ft_list_t *list);

/**
 * @brief Get the number of nodes in the list
 * @param list Pointer to the list
 * @return Node count (0 if NULL)
 */
uint32_t elib_ft_list_count(const elib_ft_list_t *list);

/**
 * @brief Insert node at head of list
 * @param list Pointer to the list
 * @param node Node to insert (must be detached: node->owner == NULL)
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM on NULL list/node or
 *         node already linked into some list
 */
elib_ft_err_t elib_ft_list_push_front(elib_ft_list_t *list, elib_ft_list_node_t *node);

/**
 * @brief Insert node at tail of list (O(1) via cached tail)
 * @param list Pointer to the list
 * @param node Node to insert (must be detached: node->owner == NULL)
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM on NULL list/node or
 *         node already linked into some list
 */
elib_ft_err_t elib_ft_list_push_back(elib_ft_list_t *list, elib_ft_list_node_t *node);

/**
 * @brief Remove the first node from the list
 * @param list Pointer to the list
 * @return Removed node (detached), or NULL if list empty / NULL
 */
elib_ft_list_node_t *elib_ft_list_pop_front(elib_ft_list_t *list);

/**
 * @brief Remove the last node from the list (O(1) via cached tail)
 * @param list Pointer to the list
 * @return Removed node (detached), or NULL if list empty / NULL
 */
elib_ft_list_node_t *elib_ft_list_pop_back(elib_ft_list_t *list);

/**
 * @brief Remove a specific node from the list
 * @param list Pointer to the list
 * @param node Node to remove
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM on NULL arguments,
 *         ELIB_FT_ERR_NO_DATA when node is detached, and
 *         ELIB_FT_ERR_INVALID_PARAM when node belongs to another list
 */
elib_ft_err_t elib_ft_list_remove(elib_ft_list_t *list, elib_ft_list_node_t *node);

/**
 * @brief Insert node after an existing node of the list
 * @param list Pointer to the list
 * @param after Existing node in the list (after->owner == list)
 * @param node Node to insert (must be detached)
 * @return ELIB_FT_OK, or ELIB_FT_ERR_INVALID_PARAM on NULL/detached
 *         after or already-linked node
 */
elib_ft_err_t elib_ft_list_insert_after(elib_ft_list_t *list, elib_ft_list_node_t *after, elib_ft_list_node_t *node);

/**
 * @brief Insert node before an existing node of the list
 * @param list Pointer to the list
 * @param before Existing node in the list (before->owner == list)
 * @param node Node to insert (must be detached)
 * @return ELIB_FT_OK, or ELIB_FT_ERR_INVALID_PARAM on NULL/detached
 *         before or already-linked node
 */
elib_ft_err_t elib_ft_list_insert_before(elib_ft_list_t *list, elib_ft_list_node_t *before, elib_ft_list_node_t *node);

/* ================================================================
 * checksum -- Sum8 / CRC8 / CRC16 / CRC32 / FNV-1a
 *
 * Streaming contexts are init-guarded: update() refuses to run on a
 * not-initialized (or zeroed) ctx with ELIB_FT_ERR_NOT_INITIALIZED.
 * XOR-out is configured once at init and applied automatically in
 * final(); no manual XOR needed.
 * ================================================================ */

typedef struct {
    uint32_t sum;
    struct {
        uint8_t initialized : 1;
    } bit_flags;
} elib_ft_sum8_ctx_t;

/**
 * @brief Initialize Sum8 context
 * @param ctx User-allocated context
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 */
elib_ft_err_t elib_ft_sum8_init(elib_ft_sum8_ctx_t *ctx);

/**
 * @brief Feed data into the Sum8 accumulator
 * @param ctx Initialized context
 * @param data Input data
 * @param len Data length in bytes
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM or
 *         ELIB_FT_ERR_NOT_INITIALIZED
 */
elib_ft_err_t elib_ft_sum8_update(elib_ft_sum8_ctx_t *ctx, const uint8_t *data, uint32_t len);

/**
 * @brief Get the final Sum8 value (ctx keeps its state until re-init)
 * @param ctx Initialized context
 * @return Checksum value, 0 on NULL or uninitialized ctx (documented
 *         sentinel -- a valid checksum of real data can also be 0,
 *         disambiguate by comparing with a known frame)
 */
uint8_t elib_ft_sum8_final(elib_ft_sum8_ctx_t *ctx);

/**
 * @brief One-shot Sum8 over a buffer
 * @param data Input data
 * @param len Data length in bytes
 * @return Checksum value
 */
uint8_t elib_ft_sum8(const uint8_t *data, uint32_t len);

typedef struct {
    uint8_t  crc;
    uint8_t  poly;
    uint8_t  xor_out;
    struct {
        uint8_t initialized : 1;
        uint8_t ref         : 1;    /* 0=MSB-first, 1=LSB-first */
    } bit_flags;
} elib_ft_crc8_ctx_t;

/**
 * @brief Initialize CRC8 context
 * @param ctx User-allocated context
 * @param poly Polynomial (already in the right bit order for `ref`)
 * @param init_val Initial CRC value
 * @param xor_out Final XOR value (0 for none)
 * @param ref 0 = MSB-first, 1 = LSB-first
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 */
elib_ft_err_t elib_ft_crc8_init(elib_ft_crc8_ctx_t *ctx, uint8_t poly, uint8_t init_val, uint8_t xor_out, int ref);

/**
 * @brief Feed data into the CRC8 accumulator
 * @param ctx Initialized context
 * @param data Input data
 * @param len Data length in bytes
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM or
 *         ELIB_FT_ERR_NOT_INITIALIZED
 */
elib_ft_err_t elib_ft_crc8_update(elib_ft_crc8_ctx_t *ctx, const uint8_t *data, uint32_t len);

/**
 * @brief Get the final CRC8 value (applies xor_out; state is kept)
 * @param ctx Initialized context
 * @return CRC value, 0 on NULL or uninitialized ctx (documented
 *         sentinel)
 */
uint8_t elib_ft_crc8_final(elib_ft_crc8_ctx_t *ctx);

/**
 * @brief One-shot CRC8 over a buffer
 * @param data Input data
 * @param len Data length in bytes
 * @param poly Polynomial
 * @param init_val Initial CRC value
 * @param xor_out Final XOR value
 * @param ref 0 = MSB-first, 1 = LSB-first
 * @return CRC value
 */
uint8_t elib_ft_crc8(const uint8_t *data, uint32_t len, uint8_t poly, uint8_t init_val, uint8_t xor_out, int ref);

#define ELIB_FT_CRC8_CCITT_INIT(ctx)    elib_ft_crc8_init(ctx, 0x07, 0x00, 0x00, 0)
#define ELIB_FT_CRC8_MAXIM_INIT(ctx)    elib_ft_crc8_init(ctx, 0x8C, 0x00, 0x00, 1)

typedef struct {
    uint16_t crc;
    uint16_t poly;
    uint16_t xor_out;
    struct {
        uint16_t initialized : 1;
        uint16_t ref         : 1;    /* 0=MSB-first, 1=LSB-first */
    } bit_flags;
} elib_ft_crc16_ctx_t;

/**
 * @brief Initialize CRC16 context (same parameter meaning as CRC8)
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 */
elib_ft_err_t elib_ft_crc16_init(elib_ft_crc16_ctx_t *ctx, uint16_t poly, uint16_t init_val, uint16_t xor_out, int ref);

/**
 * @brief Feed data into the CRC16 accumulator
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM or
 *         ELIB_FT_ERR_NOT_INITIALIZED
 */
elib_ft_err_t elib_ft_crc16_update(elib_ft_crc16_ctx_t *ctx, const uint8_t *data, uint32_t len);

/**
 * @brief Get the final CRC16 value (applies xor_out; state is kept)
 * @return CRC value, 0 on NULL or uninitialized ctx (documented
 *         sentinel)
 */
uint16_t elib_ft_crc16_final(elib_ft_crc16_ctx_t *ctx);

/**
 * @brief One-shot CRC16 over a buffer
 * @return CRC value
 */
uint16_t elib_ft_crc16(const uint8_t *data, uint32_t len, uint16_t poly, uint16_t init_val, uint16_t xor_out, int ref);

#define ELIB_FT_CRC16_XMODEM_INIT(ctx)  elib_ft_crc16_init(ctx, 0x1021, 0x0000, 0x0000, 0)
#define ELIB_FT_CRC16_CCITT_INIT(ctx)   elib_ft_crc16_init(ctx, 0x1021, 0xFFFF, 0x0000, 0)
#define ELIB_FT_CRC16_MODBUS_INIT(ctx)  elib_ft_crc16_init(ctx, 0xA001, 0xFFFF, 0x0000, 1)

typedef struct {
    uint32_t crc;
    uint32_t poly;
    uint32_t xor_out;
    struct {
        uint32_t initialized : 1;
        uint32_t ref         : 1;    /* 0=MSB-first, 1=LSB-first */
    } bit_flags;
} elib_ft_crc32_ctx_t;

/**
 * @brief Initialize CRC32 context (same parameter meaning as CRC8)
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL
 */
elib_ft_err_t elib_ft_crc32_init(elib_ft_crc32_ctx_t *ctx, uint32_t poly, uint32_t init_val, uint32_t xor_out, int ref);

/**
 * @brief Feed data into the CRC32 accumulator
 * @return ELIB_FT_OK, ELIB_FT_ERR_INVALID_PARAM or
 *         ELIB_FT_ERR_NOT_INITIALIZED
 */
elib_ft_err_t elib_ft_crc32_update(elib_ft_crc32_ctx_t *ctx, const uint8_t *data, uint32_t len);

/**
 * @brief Get the final CRC32 value (applies xor_out; state is kept)
 * @return CRC value, 0 on NULL or uninitialized ctx (documented
 *         sentinel)
 */
uint32_t elib_ft_crc32_final(elib_ft_crc32_ctx_t *ctx);

/**
 * @brief One-shot CRC32 over a buffer
 * @return CRC value
 */
uint32_t elib_ft_crc32(const uint8_t *data, uint32_t len, uint32_t poly, uint32_t init_val, uint32_t xor_out, int ref);

#define ELIB_FT_CRC32_ETHERNET_INIT(ctx) elib_ft_crc32_init(ctx, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1)

/**
 * @brief Compute FNV-1a 32-bit hash of a data block
 * @param data Input data
 * @param len  Data length in bytes
 * @return FNV-1a 32-bit hash (offset_basis=2166136261, prime=16777619)
 */
uint32_t elib_ft_hash_fnv1a32(const uint8_t *data, uint32_t len);

/**
 * @brief Compute FNV-1a 64-bit hash of a data block
 * @param data Input data
 * @param len  Data length in bytes
 * @return FNV-1a 64-bit hash (offset_basis=14695981039346656037, prime=1099511628211)
 */
uint64_t elib_ft_hash_fnv1a64(const uint8_t *data, uint32_t len);

/* ================================================================
 * str -- bounded string operations
 *
 * All parse functions are length-bounded by max_len and never read
 * past it. atoi32/atou32 report overflow with documented sentinels:
 * return value 0 AND *endptr = NULL, so a valid parse always leaves
 * endptr non-NULL.
 * ================================================================ */

/**
 * @brief Get string length with max limit
 * @param s String pointer
 * @param max_len Maximum allowed length
 * @return Actual string length (0 on NULL)
 */
uint32_t elib_ft_strlen(const char *s, uint32_t max_len);

/**
 * @brief Compare two strings
 * @param s1 First string
 * @param s2 Second string
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2;
 *         NULL-safe (NULL sorts before any string)
 */
int elib_ft_strcmp(const char *s1, const char *s2);

/**
 * @brief Compare two strings up to n characters
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum characters to compare
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2;
 *         NULL-safe
 */
int elib_ft_strncmp(const char *s1, const char *s2, uint32_t n);

/**
 * @brief Convert string to signed 32-bit integer
 * @param s String pointer
 * @param max_len Maximum characters to parse
 * @param endptr Output pointer for first unparsed character (can be
 *               NULL); set to NULL on parse error
 * @return Parsed value; 0 AND *endptr = NULL on NULL input, no digits,
 *         or value outside int32 range (documented error sentinels)
 */
int32_t elib_ft_atoi32(const char *s, uint32_t max_len, const char **endptr);

/**
 * @brief Convert string to unsigned 32-bit integer
 * @param s String pointer
 * @param max_len Maximum characters to parse
 * @param endptr Output pointer for first unparsed character (can be
 *               NULL); set to NULL on parse error
 * @return Parsed value; 0 AND *endptr = NULL on NULL input, no digits,
 *         or value outside uint32 range (documented error sentinels)
 */
uint32_t elib_ft_atou32(const char *s, uint32_t max_len, const char **endptr);

/**
 * @brief Convert string to double (leading sign and decimal fraction)
 * @param s String pointer
 * @param max_len Maximum characters to parse
 * @param endptr Output pointer for first unparsed character within
 *               the first 64 scanned characters (can be NULL); longer
 *               numerals are truncated to 64 characters before
 *               conversion -- endptr may then land mid-numeral
 * @return Parsed value, 0.0 on error
 */
double elib_ft_atof(const char *s, uint32_t max_len, const char **endptr);

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
 * @param endptr Output pointer for first character after token (can
 *               be NULL); NULL when token ends at buffer boundary
 * @return Pointer to token start, or NULL if not found / NULL input
 */
const char *elib_ft_strarg_get(const char *s, uint32_t max_len, uint32_t index, const char **endptr);

/**
 * @brief Quick string compare using FNV-1a hash pre-check
 * @param s1 First string
 * @param s2 Second string
 * @param max_len Maximum characters to compare
 * @return 0 if equal, negative if s1 sorts before s2, positive
 *         otherwise. NULL-safe.
 *
 * Algorithm:
 *   - If lengths differ -> -1 / +1 by length order
 *   - Compute FNV-1a 32-bit hash of both (same length)
 *   - If hashes differ  -> -1 / +1 by hash order (fast O(1) reject on
 *     most unequal strings)
 *   - If hashes equal   -> memcmp confirmation (hash collisions are
 *     possible, ordering within a collision group follows memcmp)
 *
 * memcmp result can be any negative/positive magnitude, only the sign
 * is meaningful.
 */
int elib_ft_strcmp_fast(const char *s1, const char *s2, uint32_t max_len);

/* ================================================================
 * endian -- byte-swap / host-network conversions
 * ================================================================ */

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

/* ================================================================
 * time -- calendar <-> Unix epoch conversion
 *
 * ELIB_FT_EPOCH_T selects the output type (default uint64_t):
 *   uint64_t  internal arithmetic is int64_t, calendar years are
 *             bounded to 1..65535 by elib_ft_time_t
 *   uint32_t  internal arithmetic stays 32-bit; valid epoch range is
 *             1970-01-01 00:00:00 .. 2106-02-07 06:28:15 UTC, anything
 *             beyond returns ELIB_FT_ERR_EXCEED_MAX
 * Redefine ELIB_FT_EPOCH_T before including this header.
 * ================================================================ */

/**
 * @def ELIB_FT_EPOCH_T
 * @brief Epoch second type, uint64_t by default
 */
#ifndef ELIB_FT_EPOCH_T
    #define ELIB_FT_EPOCH_T uint64_t
#endif

/* Auto-derive signed type for internal arithmetic. */
#if UINT32_MAX == (ELIB_FT_EPOCH_T)-1
    typedef int32_t  elib_ft_epoch_signed_t;
#elif UINT64_MAX == (ELIB_FT_EPOCH_T)-1
    typedef int64_t  elib_ft_epoch_signed_t;
#else
    #error "ELIB_FT_EPOCH_T must be uint32_t or uint64_t"
#endif

typedef struct {
    uint16_t year;    /* calendar year: 1..65535 (uint32 epoch config
                         valid window: 1970..2106) */
    uint8_t  month;   /* 1-12 */
    uint8_t  day;     /* 1-31 */
    uint8_t  hour;    /* 0-23 */
    uint8_t  minute;  /* 0-59 */
    uint8_t  second;  /* 0-59 */
} elib_ft_time_t;

/**
 * @brief Convert calendar time to Unix epoch seconds
 * @param t Calendar time
 * @param epoch Output epoch seconds
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL,
 *         year < 1970, month/day/h/m/s out of range, or
 *         ELIB_FT_ERR_EXCEED_MAX when the date exceeds the configured
 *         epoch type range (uint32 config: past 2106-02-07 06:28:15)
 *
 * Does not account for leap seconds or timezone (always UTC).
 */
elib_ft_err_t elib_ft_time_to_epoch(const elib_ft_time_t *t, ELIB_FT_EPOCH_T *epoch);

/**
 * @brief Convert Unix epoch seconds to calendar time
 * @param epoch Epoch seconds
 * @param t Output calendar time
 * @return ELIB_FT_OK on success, ELIB_FT_ERR_INVALID_PARAM on NULL t,
 *         or ELIB_FT_ERR_EXCEED_MAX when the result year exceeds
 *         65535 (uint64 config)
 *
 * Does not account for leap seconds or timezone (always UTC).
 */
elib_ft_err_t elib_ft_epoch_to_time(ELIB_FT_EPOCH_T epoch, elib_ft_time_t *t);

/**
 * @brief Get day of week from epoch seconds
 * @param epoch Epoch seconds
 * @return 0=Sunday, 1=Monday, ..., 6=Saturday
 */
uint8_t elib_ft_time_wday(ELIB_FT_EPOCH_T epoch);

/* ================================================================
 * qmath -- Qm.n fixed-point arithmetic
 *
 * Six families: Q16/UQ16 (16-bit), Q32/UQ32 (32-bit), Q64/UQ64
 * (64-bit). `n` is the fractional bit count, passed at runtime.
 *   - signed families:   n in [0, bitwidth-2]
 *   - unsigned families: n in [0, bitwidth-1]
 *
 * Each family provides:
 *   - fast variants: return a VALUE, no overflow detection. Invalid
 *     n is rejected as a defined fallback (returns 0 / unchanged q);
 *     arithmetic overflow WRAPS (undefined for C signed types is
 *     avoided where possible but the caller must guarantee by
 *     construction that add/sub/round inputs cannot overflow)
 *   - `_s` safe variants: full validation and overflow detection,
 *     return elib_ft_err_t, output via pointer
 *
 * from_double/from_float saturate to the type range instead of
 * invoking out-of-range cast UB.
 * ================================================================ */

/* Q_N domain: signed types [0, bits-2], unsigned types [0, bits-1] */

/* ---- Q16 - signed 16-bit ---- */

int16_t  elib_ft_q16_from_double(double val, int n);
double   elib_ft_q16_to_double(int16_t q, int n);
int16_t  elib_ft_q16_from_float(float val, int n);
float    elib_ft_q16_to_float(int16_t q, int n);
int16_t  elib_ft_q16_add(int16_t a, int16_t b);        /* caller guarantees no overflow */
int16_t  elib_ft_q16_sub(int16_t a, int16_t b);        /* caller guarantees no overflow */
int16_t  elib_ft_q16_mul(int16_t a, int16_t b, int n); /* wraps on overflow */
int16_t  elib_ft_q16_div(int16_t a, int16_t b, int n); /* b==0 -> 0; wraps on overflow */
elib_ft_err_t elib_ft_q16_add_s(int16_t a, int16_t b, int16_t *out);
elib_ft_err_t elib_ft_q16_sub_s(int16_t a, int16_t b, int16_t *out);
elib_ft_err_t elib_ft_q16_mul_s(int16_t a, int16_t b, int n, int16_t *out);
elib_ft_err_t elib_ft_q16_div_s(int16_t a, int16_t b, int n, int16_t *out);
int16_t  elib_ft_q16_convert(int16_t q, int n_from, int n_to); /* invalid n -> unchanged; up-shift drops high bits */
int16_t  elib_ft_q16_floor(int16_t q, int n);
int16_t  elib_ft_q16_ceil(int16_t q, int n);
int16_t  elib_ft_q16_round(int16_t q, int n);
int16_t  elib_ft_q16_abs(int16_t q);                   /* INT16_MIN -> INT16_MIN (documented) */

/* ---- UQ16 - unsigned 16-bit ---- */

uint16_t elib_ft_uq16_from_double(double val, int n);
double   elib_ft_uq16_to_double(uint16_t q, int n);
uint16_t elib_ft_uq16_add(uint16_t a, uint16_t b);
uint16_t elib_ft_uq16_sub(uint16_t a, uint16_t b);
uint16_t elib_ft_uq16_mul(uint16_t a, uint16_t b, int n);
uint16_t elib_ft_uq16_div(uint16_t a, uint16_t b, int n);
elib_ft_err_t elib_ft_uq16_add_s(uint16_t a, uint16_t b, uint16_t *out);
elib_ft_err_t elib_ft_uq16_sub_s(uint16_t a, uint16_t b, uint16_t *out);
elib_ft_err_t elib_ft_uq16_mul_s(uint16_t a, uint16_t b, int n, uint16_t *out);
elib_ft_err_t elib_ft_uq16_div_s(uint16_t a, uint16_t b, int n, uint16_t *out);
uint16_t elib_ft_uq16_convert(uint16_t q, int n_from, int n_to);

/* ---- Q32 - signed 32-bit ---- */

int32_t  elib_ft_q32_from_double(double val, int n);
double   elib_ft_q32_to_double(int32_t q, int n);
int32_t  elib_ft_q32_from_float(float val, int n);
float    elib_ft_q32_to_float(int32_t q, int n);
int32_t  elib_ft_q32_add(int32_t a, int32_t b);        /* caller guarantees no overflow */
int32_t  elib_ft_q32_sub(int32_t a, int32_t b);        /* caller guarantees no overflow */
int32_t  elib_ft_q32_mul(int32_t a, int32_t b, int n); /* wraps on overflow */
int32_t  elib_ft_q32_div(int32_t a, int32_t b, int n); /* b==0 -> 0; wraps on overflow */
elib_ft_err_t elib_ft_q32_add_s(int32_t a, int32_t b, int32_t *out);
elib_ft_err_t elib_ft_q32_sub_s(int32_t a, int32_t b, int32_t *out);
elib_ft_err_t elib_ft_q32_mul_s(int32_t a, int32_t b, int n, int32_t *out);
elib_ft_err_t elib_ft_q32_div_s(int32_t a, int32_t b, int n, int32_t *out);
int32_t  elib_ft_q32_convert(int32_t q, int n_from, int n_to);
int32_t  elib_ft_q32_floor(int32_t q, int n);
int32_t  elib_ft_q32_ceil(int32_t q, int n);
int32_t  elib_ft_q32_round(int32_t q, int n);
int32_t  elib_ft_q32_abs(int32_t q);                   /* INT32_MIN -> INT32_MIN (documented) */

/* ---- UQ32 - unsigned 32-bit ---- */

uint32_t elib_ft_uq32_from_double(double val, int n);
double   elib_ft_uq32_to_double(uint32_t q, int n);
uint32_t elib_ft_uq32_add(uint32_t a, uint32_t b);
uint32_t elib_ft_uq32_sub(uint32_t a, uint32_t b);
uint32_t elib_ft_uq32_mul(uint32_t a, uint32_t b, int n);
uint32_t elib_ft_uq32_div(uint32_t a, uint32_t b, int n);
elib_ft_err_t elib_ft_uq32_add_s(uint32_t a, uint32_t b, uint32_t *out);
elib_ft_err_t elib_ft_uq32_sub_s(uint32_t a, uint32_t b, uint32_t *out);
elib_ft_err_t elib_ft_uq32_mul_s(uint32_t a, uint32_t b, int n, uint32_t *out);
elib_ft_err_t elib_ft_uq32_div_s(uint32_t a, uint32_t b, int n, uint32_t *out);
uint32_t elib_ft_uq32_convert(uint32_t q, int n_from, int n_to);

/* ---- Q64 - signed 64-bit ---- */

int64_t  elib_ft_q64_from_double(double val, int n);
double   elib_ft_q64_to_double(int64_t q, int n);
int64_t  elib_ft_q64_add(int64_t a, int64_t b);        /* caller guarantees no overflow */
int64_t  elib_ft_q64_sub(int64_t a, int64_t b);        /* caller guarantees no overflow */
int64_t  elib_ft_q64_mul(int64_t a, int64_t b, int n); /* wraps on overflow */
int64_t  elib_ft_q64_div(int64_t a, int64_t b, int n); /* b==0 -> 0; wraps on overflow */
elib_ft_err_t elib_ft_q64_add_s(int64_t a, int64_t b, int64_t *out);
elib_ft_err_t elib_ft_q64_sub_s(int64_t a, int64_t b, int64_t *out);
elib_ft_err_t elib_ft_q64_mul_s(int64_t a, int64_t b, int n, int64_t *out);
elib_ft_err_t elib_ft_q64_div_s(int64_t a, int64_t b, int n, int64_t *out);
int64_t  elib_ft_q64_convert(int64_t q, int n_from, int n_to);
int64_t  elib_ft_q64_floor(int64_t q, int n);
int64_t  elib_ft_q64_ceil(int64_t q, int n);
int64_t  elib_ft_q64_round(int64_t q, int n);
int64_t  elib_ft_q64_abs(int64_t q);                   /* INT64_MIN -> INT64_MIN (documented) */

/* ---- UQ64 - unsigned 64-bit ---- */

uint64_t elib_ft_uq64_from_double(double val, int n);
double   elib_ft_uq64_to_double(uint64_t q, int n);
uint64_t elib_ft_uq64_add(uint64_t a, uint64_t b);
uint64_t elib_ft_uq64_sub(uint64_t a, uint64_t b);
uint64_t elib_ft_uq64_mul(uint64_t a, uint64_t b, int n);
uint64_t elib_ft_uq64_div(uint64_t a, uint64_t b, int n);
elib_ft_err_t elib_ft_uq64_add_s(uint64_t a, uint64_t b, uint64_t *out);
elib_ft_err_t elib_ft_uq64_sub_s(uint64_t a, uint64_t b, uint64_t *out);
elib_ft_err_t elib_ft_uq64_mul_s(uint64_t a, uint64_t b, int n, uint64_t *out);
elib_ft_err_t elib_ft_uq64_div_s(uint64_t a, uint64_t b, int n, uint64_t *out);
uint64_t elib_ft_uq64_convert(uint64_t q, int n_from, int n_to);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_H */
