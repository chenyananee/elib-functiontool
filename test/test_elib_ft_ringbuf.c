/* test_elib_ft_ringbuf.c - FunctionTool Ring Buffer Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RB_SIZE       4
#define RB_CELL_SIZE  sizeof(uint32_t)

static uint8_t               rb_buf[RB_SIZE * RB_CELL_SIZE];
static elib_ft_ringbuf_ctx_t  rb_ctx;

static void mock_reset(void)
{
    memset(rb_buf, 0, sizeof(rb_buf));
    memset(&rb_ctx, 0, sizeof(rb_ctx));
}

#define RUN_TEST(fn) do { mock_reset(); fn(); printf("  PASS: %s\n", #fn); } while (0)

static void test_ringbuf_init_valid(void)
{
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE) == ELIB_FT_OK);
    assert(rb_ctx.bit_flags.initialized == 1);
    assert(rb_ctx.head == 0);
    assert(rb_ctx.tail == 0);
    assert(rb_ctx.buf == rb_buf);
    assert(rb_ctx.size == RB_SIZE);
    assert(rb_ctx.cell_size == RB_CELL_SIZE);
}

static void test_ringbuf_init_null_ctx(void)
{
    assert(elib_ft_ringbuf_init(NULL, rb_buf, RB_SIZE, RB_CELL_SIZE) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_ringbuf_init_null_buf(void)
{
    assert(elib_ft_ringbuf_init(&rb_ctx, NULL, RB_SIZE, RB_CELL_SIZE) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, 0, RB_CELL_SIZE) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, 0) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_ringbuf_deinit(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);
    elib_ft_ringbuf_deinit(&rb_ctx);
    assert(rb_ctx.bit_flags.initialized == 0);

    /* deinit NULL is safe */
    elib_ft_ringbuf_deinit(NULL);
}

static void test_ringbuf_not_initialized(void)
{
    uint32_t val = 1;
    assert(elib_ft_ringbuf_write(&rb_ctx, &val, 1, 1) == ELIB_FT_ERR_NOT_INITIALIZED);
    assert(elib_ft_ringbuf_read(&rb_ctx, &val, 1, 1) == ELIB_FT_ERR_NOT_INITIALIZED);
    assert(elib_ft_ringbuf_used(&rb_ctx) == 0);
    assert(elib_ft_ringbuf_free(&rb_ctx) == 0);
}

static void test_ringbuf_write_read_normal(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    uint32_t in_vals[3] = {100, 200, 300};
    assert(elib_ft_ringbuf_write(&rb_ctx, in_vals, 3, 3) == ELIB_FT_OK);

    uint32_t out_vals[3] = {0};
    assert(elib_ft_ringbuf_read(&rb_ctx, out_vals, 3, 3) == ELIB_FT_OK);
    assert(out_vals[0] == 100);
    assert(out_vals[1] == 200);
    assert(out_vals[2] == 300);
}

static void test_ringbuf_write_exceed_max(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    uint32_t val = 1;
    assert(elib_ft_ringbuf_write(&rb_ctx, &val, 2, 1) == ELIB_FT_ERR_EXCEED_MAX);
}

static void test_ringbuf_write_no_space(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    uint32_t vals[4] = {1, 2, 3, 4};
    /* capacity is size - 1 = 3 cells */
    assert(elib_ft_ringbuf_write(&rb_ctx, vals, 4, 4) == ELIB_FT_ERR_NO_SPACE);
}

static void test_ringbuf_read_exceed_max(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    uint32_t val = 42;
    elib_ft_ringbuf_write(&rb_ctx, &val, 1, 1);

    uint32_t out = 0;
    assert(elib_ft_ringbuf_read(&rb_ctx, &out, 1, 0) == ELIB_FT_ERR_EXCEED_MAX);
}

static void test_ringbuf_read_no_data(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    uint32_t out = 0;
    assert(elib_ft_ringbuf_read(&rb_ctx, &out, 1, 1) == ELIB_FT_ERR_NO_DATA);
}

static void test_ringbuf_wrap_around(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    /* fill 3 cells (max free), read 3, write 3 more -> wraps around */
    uint32_t in1[3] = {10, 20, 30};
    assert(elib_ft_ringbuf_write(&rb_ctx, in1, 3, 3) == ELIB_FT_OK);

    uint32_t out1[3] = {0};
    assert(elib_ft_ringbuf_read(&rb_ctx, out1, 3, 3) == ELIB_FT_OK);

    uint32_t in2[3] = {40, 50, 60};
    assert(elib_ft_ringbuf_write(&rb_ctx, in2, 3, 3) == ELIB_FT_OK);

    uint32_t out2[3] = {0};
    assert(elib_ft_ringbuf_read(&rb_ctx, out2, 3, 3) == ELIB_FT_OK);
    assert(out2[0] == 40);
    assert(out2[1] == 50);
    assert(out2[2] == 60);
}

static void test_ringbuf_used_free(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);
    assert(elib_ft_ringbuf_free(&rb_ctx) == 3);

    uint32_t val = 1;
    elib_ft_ringbuf_write(&rb_ctx, &val, 1, 1);
    assert(elib_ft_ringbuf_used(&rb_ctx) == 1);
    assert(elib_ft_ringbuf_free(&rb_ctx) == 2);
}

static void test_ringbuf_reset(void)
{
    elib_ft_ringbuf_init(&rb_ctx, rb_buf, RB_SIZE, RB_CELL_SIZE);

    uint32_t val = 42;
    elib_ft_ringbuf_write(&rb_ctx, &val, 1, 1);
    assert(elib_ft_ringbuf_used(&rb_ctx) == 1);

    elib_ft_ringbuf_reset(&rb_ctx);
    assert(elib_ft_ringbuf_used(&rb_ctx) == 0);
    assert(elib_ft_ringbuf_free(&rb_ctx) == 3);
}

static void test_ringbuf_cell_struct(void)
{
    typedef struct { uint16_t a; uint8_t b; } item_t;

    uint8_t buf[3 * sizeof(item_t)];
    elib_ft_ringbuf_ctx_t ctx;

    elib_ft_ringbuf_init(&ctx, buf, 3, sizeof(item_t));

    item_t in = {0x1234, 0xAB};
    assert(elib_ft_ringbuf_write(&ctx, &in, 1, 1) == ELIB_FT_OK);

    item_t out = {0};
    assert(elib_ft_ringbuf_read(&ctx, &out, 1, 1) == ELIB_FT_OK);
    assert(out.a == 0x1234);
    assert(out.b == 0xAB);
}

static void test_ringbuf_init_undersized(void)
{
    /* size < 2 cannot distinguish full from empty: reject at init */
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, 0, RB_CELL_SIZE)
           == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, 1, RB_CELL_SIZE)
           == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, 2, 0)
           == ELIB_FT_ERR_INVALID_PARAM);
    assert(rb_ctx.bit_flags.initialized == 0);

    /* size == 2 keeps exactly one usable cell */
    assert(elib_ft_ringbuf_init(&rb_ctx, rb_buf, 2, RB_CELL_SIZE)
           == ELIB_FT_OK);
    assert(elib_ft_ringbuf_free(&rb_ctx) == 1);
    assert(elib_ft_ringbuf_used(&rb_ctx) == 0);

    uint32_t v = 0x11223344u, r = 0;
    assert(elib_ft_ringbuf_write(&rb_ctx, &v, 1, 1) == ELIB_FT_OK);
    assert(elib_ft_ringbuf_used(&rb_ctx) == 1);
    assert(elib_ft_ringbuf_free(&rb_ctx) == 0);
    v = 2;
    assert(elib_ft_ringbuf_write(&rb_ctx, &v, 1, 1) == ELIB_FT_ERR_NO_SPACE);
    assert(elib_ft_ringbuf_read(&rb_ctx, &r, 1, 1) == ELIB_FT_OK);
    assert(r == 0x11223344u);
}

int main(void)
{
    RUN_TEST(test_ringbuf_init_valid);
    RUN_TEST(test_ringbuf_init_null_ctx);
    RUN_TEST(test_ringbuf_init_null_buf);
    RUN_TEST(test_ringbuf_deinit);
    RUN_TEST(test_ringbuf_not_initialized);
    RUN_TEST(test_ringbuf_write_read_normal);
    RUN_TEST(test_ringbuf_write_exceed_max);
    RUN_TEST(test_ringbuf_write_no_space);
    RUN_TEST(test_ringbuf_read_exceed_max);
    RUN_TEST(test_ringbuf_read_no_data);
    RUN_TEST(test_ringbuf_wrap_around);
    RUN_TEST(test_ringbuf_used_free);
    RUN_TEST(test_ringbuf_reset);
    RUN_TEST(test_ringbuf_cell_struct);

    RUN_TEST(test_ringbuf_init_undersized);
    printf("\nAll %d tests passed.\n", 15);
    return 0;
}
