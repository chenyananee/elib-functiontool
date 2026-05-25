/* test_elib_ft.c - FunctionTool Unit Tests */

#include "elib_ft.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Mock / Helper                                                      */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/*  elib_ft_ringbuf tests                                              */
/* ------------------------------------------------------------------ */

#define RB_SIZE       4
#define RB_CELL_SIZE  sizeof(uint32_t)

static uint8_t          rb_buf[RB_SIZE * RB_CELL_SIZE];
static elib_ft_ringbuf_ctx_t rb_ctx;

static void mock_reset(void)
{
    memset(rb_buf, 0, sizeof(rb_buf));
    memset(&rb_ctx, 0, sizeof(rb_ctx));
}

#define RUN_TEST(fn) do { \
    mock_reset(); \
    fn(); \
    printf("  PASS: %s\n", #fn); \
} while(0)

/* ------------------------------------------------------------------ */
/*  elib_ft_memcpy tests                                               */
/* ------------------------------------------------------------------ */

static void test_memcpy_normal(void)
{
    uint8_t src[] = {1, 2, 3, 4, 5};
    uint8_t dst[5] = {0};

    uint32_t ret = elib_ft_memcpy(dst, src, 5, 10);
    assert(ret == 5);
    assert(memcmp(dst, src, 5) == 0);
}

static void test_memcpy_truncate(void)
{
    uint8_t src[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t dst[8];
    memset(dst, 0xAA, sizeof(dst));

    uint32_t ret = elib_ft_memcpy(dst, src, 8, 4);
    assert(ret == 4);
    assert(memcmp(dst, src, 4) == 0);
    assert(dst[4] == 0xAA);
    assert(dst[5] == 0xAA);
}

static void test_memcpy_len_eq_max(void)
{
    uint8_t src[] = {10, 20, 30};
    uint8_t dst[3] = {0};

    uint32_t ret = elib_ft_memcpy(dst, src, 3, 3);
    assert(ret == 3);
    assert(memcmp(dst, src, 3) == 0);
}

static void test_memcpy_zero_len(void)
{
    uint8_t src[] = {1, 2, 3};
    uint8_t dst[3] = {0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memcpy(dst, src, 0, 10);
    assert(ret == 0);
    assert(dst[0] == 0xFF);
}

static void test_memcpy_zero_max(void)
{
    uint8_t src[] = {1, 2, 3};
    uint8_t dst[3] = {0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memcpy(dst, src, 3, 0);
    assert(ret == 0);
    assert(dst[0] == 0xFF);
}

static void test_memcpy_null_dst(void)
{
    uint8_t src[] = {1, 2, 3};

    uint32_t ret = elib_ft_memcpy(NULL, src, 3, 10);
    assert(ret == 0);
}

static void test_memcpy_null_src(void)
{
    uint8_t dst[3] = {0};

    uint32_t ret = elib_ft_memcpy(dst, NULL, 3, 10);
    assert(ret == 0);
}

static void test_memcpy_single_byte(void)
{
    uint8_t src = 0x42;
    uint8_t dst = 0;

    uint32_t ret = elib_ft_memcpy(&dst, &src, 1, 1);
    assert(ret == 1);
    assert(dst == 0x42);
}

/* ------------------------------------------------------------------ */
/*  elib_ft_memset tests                                               */
/* ------------------------------------------------------------------ */

static void test_memset_normal(void)
{
    uint8_t buf[8] = {0};

    uint32_t ret = elib_ft_memset(buf, 0xAB, 6, 10);
    assert(ret == 6);
    for (uint32_t i = 0; i < 6; i++) {
        assert(buf[i] == 0xAB);
    }
    assert(buf[6] == 0);
    assert(buf[7] == 0);
}

static void test_memset_truncate(void)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));

    uint32_t ret = elib_ft_memset(buf, 0xFF, 8, 3);
    assert(ret == 3);
    for (uint32_t i = 0; i < 3; i++) {
        assert(buf[i] == 0xFF);
    }
    assert(buf[3] == 0);
    assert(buf[4] == 0);
}

static void test_memset_len_eq_max(void)
{
    uint8_t buf[4] = {0};

    uint32_t ret = elib_ft_memset(buf, 0x55, 4, 4);
    assert(ret == 4);
    for (uint32_t i = 0; i < 4; i++) {
        assert(buf[i] == 0x55);
    }
}

static void test_memset_zero_len(void)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memset(buf, 0x00, 0, 10);
    assert(ret == 0);
    assert(buf[0] == 0xFF);
}

static void test_memset_zero_max(void)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memset(buf, 0x00, 4, 0);
    assert(ret == 0);
    assert(buf[0] == 0xFF);
}

static void test_memset_null_dst(void)
{
    uint32_t ret = elib_ft_memset(NULL, 0x00, 4, 10);
    assert(ret == 0);
}

static void test_memset_zero_value(void)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    uint32_t ret = elib_ft_memset(buf, 0x00, 4, 10);
    assert(ret == 4);
    for (uint32_t i = 0; i < 4; i++) {
        assert(buf[i] == 0x00);
    }
}

static void test_memset_single_byte(void)
{
    uint8_t buf = 0;

    uint32_t ret = elib_ft_memset(&buf, 0x99, 1, 1);
    assert(ret == 1);
    assert(buf == 0x99);
}

/* ------------------------------------------------------------------ */
/*  elib_ft_ringbuf tests                                              */
/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */
/*  elib_ft_list tests                                                 */
/* ------------------------------------------------------------------ */

typedef struct {
    int                value;
    elib_ft_list_node_t node;
} test_item_t;

static void test_list_init(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);
    assert(elib_ft_list_empty(&list) == 1);
    assert(elib_ft_list_count(&list) == 0);
}

static void test_list_init_null(void)
{
    elib_ft_list_init(NULL);
    assert(elib_ft_list_empty(NULL) == 1);
    assert(elib_ft_list_count(NULL) == 0);
}

static void test_list_push_front(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_front(&list, &a.node);
    assert(elib_ft_list_count(&list) == 1);
    assert(list.head == &a.node);

    elib_ft_list_push_front(&list, &b.node);
    assert(elib_ft_list_count(&list) == 2);
    assert(list.head == &b.node);
    assert(b.node.next == &a.node);
    assert(a.node.prev == &b.node);
}

static void test_list_push_back(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);

    assert(list.head == &a.node);
    assert(a.node.next == &b.node);
    assert(b.node.prev == &a.node);
    assert(b.node.next == NULL);
    assert(elib_ft_list_count(&list) == 2);
}

static void test_list_pop_front(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);

    elib_ft_list_node_t *n = elib_ft_list_pop_front(&list);
    assert(n == &a.node);
    assert(a.node.next == NULL);
    assert(a.node.prev == NULL);
    assert(list.head == &b.node);
    assert(b.node.prev == NULL);
    assert(elib_ft_list_count(&list) == 1);

    n = elib_ft_list_pop_front(&list);
    assert(n == &b.node);
    assert(elib_ft_list_empty(&list) == 1);
    assert(elib_ft_list_count(&list) == 0);
}

static void test_list_pop_back(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);

    elib_ft_list_node_t *n = elib_ft_list_pop_back(&list);
    assert(n == &b.node);
    assert(elib_ft_list_count(&list) == 1);

    n = elib_ft_list_pop_back(&list);
    assert(n == &a.node);
    assert(elib_ft_list_empty(&list) == 1);
}

static void test_list_pop_empty(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    assert(elib_ft_list_pop_front(&list) == NULL);
    assert(elib_ft_list_pop_back(&list) == NULL);
}

static void test_list_remove(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};
    test_item_t c = {3, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);
    elib_ft_list_push_back(&list, &c.node);

    /* remove middle */
    elib_ft_list_remove(&list, &b.node);
    assert(b.node.next == NULL);
    assert(b.node.prev == NULL);
    assert(a.node.next == &c.node);
    assert(c.node.prev == &a.node);
    assert(elib_ft_list_count(&list) == 2);
}

static void test_list_remove_head(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);

    elib_ft_list_remove(&list, &a.node);
    assert(list.head == &b.node);
    assert(b.node.prev == NULL);
    assert(elib_ft_list_count(&list) == 1);
}

static void test_list_remove_tail(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);

    elib_ft_list_remove(&list, &b.node);
    assert(a.node.next == NULL);
    assert(elib_ft_list_count(&list) == 1);
}

static void test_list_insert_after(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};
    test_item_t c = {3, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &c.node);

    elib_ft_list_insert_after(&list, &a.node, &b.node);
    assert(a.node.next == &b.node);
    assert(b.node.prev == &a.node);
    assert(b.node.next == &c.node);
    assert(c.node.prev == &b.node);
    assert(elib_ft_list_count(&list) == 3);
}

static void test_list_insert_before(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};
    test_item_t c = {3, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &c.node);

    elib_ft_list_insert_before(&list, &c.node, &b.node);
    assert(a.node.next == &b.node);
    assert(b.node.prev == &a.node);
    assert(b.node.next == &c.node);
    assert(c.node.prev == &b.node);
    assert(elib_ft_list_count(&list) == 3);
}

static void test_list_insert_before_head(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {1, {NULL, NULL}};
    test_item_t b = {2, {NULL, NULL}};

    elib_ft_list_push_back(&list, &b.node);
    elib_ft_list_insert_before(&list, &b.node, &a.node);

    assert(list.head == &a.node);
    assert(a.node.next == &b.node);
    assert(b.node.prev == &a.node);
    assert(elib_ft_list_count(&list) == 2);
}

static void test_list_entry_macro(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t items[3] = {
        {10, {NULL, NULL}},
        {20, {NULL, NULL}},
        {30, {NULL, NULL}},
    };

    elib_ft_list_push_back(&list, &items[0].node);
    elib_ft_list_push_back(&list, &items[1].node);
    elib_ft_list_push_back(&list, &items[2].node);

    elib_ft_list_node_t *cursor;
    int idx = 0;
    ELIB_FT_LIST_FOR_EACH(cursor, &list) {
        test_item_t *item = ELIB_FT_LIST_ENTRY(cursor, test_item_t, node);
        assert(item->value == items[idx].value);
        idx++;
    }
    assert(idx == 3);
}

static void test_list_for_each_safe(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t items[3] = {
        {1, {NULL, NULL}},
        {2, {NULL, NULL}},
        {3, {NULL, NULL}},
    };

    elib_ft_list_push_back(&list, &items[0].node);
    elib_ft_list_push_back(&list, &items[1].node);
    elib_ft_list_push_back(&list, &items[2].node);

    /* remove all during iteration */
    elib_ft_list_node_t *cursor;
    elib_ft_list_node_t *tmp;
    int removed = 0;
    ELIB_FT_LIST_FOR_EACH_SAFE(cursor, tmp, &list) {
        elib_ft_list_remove(&list, cursor);
        removed++;
    }
    assert(removed == 3);
    assert(elib_ft_list_empty(&list) == 1);
}

static void test_list_single_node(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    test_item_t a = {42, {NULL, NULL}};
    elib_ft_list_push_back(&list, &a.node);

    assert(elib_ft_list_count(&list) == 1);
    assert(list.head == &a.node);
    assert(a.node.next == NULL);
    assert(a.node.prev == NULL);

    elib_ft_list_node_t *n = elib_ft_list_pop_front(&list);
    assert(n == &a.node);
    assert(elib_ft_list_empty(&list) == 1);
}

static void test_list_null_params(void)
{
    test_item_t a = {1, {NULL, NULL}};
    elib_ft_list_push_front(NULL, &a.node);
    elib_ft_list_push_back(NULL, &a.node);
    elib_ft_list_push_front((elib_ft_list_t *)1, NULL);
    elib_ft_list_push_back((elib_ft_list_t *)1, NULL);
    elib_ft_list_remove(NULL, &a.node);
    elib_ft_list_remove((elib_ft_list_t *)1, NULL);
    elib_ft_list_insert_after(NULL, &a.node, &a.node);
    elib_ft_list_insert_after((elib_ft_list_t *)1, NULL, &a.node);
    elib_ft_list_insert_before(NULL, &a.node, &a.node);
    elib_ft_list_insert_before((elib_ft_list_t *)1, NULL, &a.node);
}

/* ------------------------------------------------------------------ */
/*  elib_ft_bitmap tests                                               */
/* ------------------------------------------------------------------ */

static void test_bitmap_set_get(void)
{
    uint8_t buf[2] = {0};
    assert(elib_ft_bitmap_set(buf, 16, 0) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 16, 0) == 1);
    assert(elib_ft_bitmap_set(buf, 16, 7) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 16, 7) == 1);
    assert(elib_ft_bitmap_set(buf, 16, 8) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 16, 8) == 1);
    assert(elib_ft_bitmap_get(buf, 16, 1) == 0);
    assert(buf[0] == 0x81);
    assert(buf[1] == 0x01);
}

static void test_bitmap_clear(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_clear(buf, 8, 3) == ELIB_FT_OK);
    assert(elib_ft_bitmap_get(buf, 8, 3) == 0);
    assert(buf[0] == 0xF7);
}

static void test_bitmap_set_out_of_range(void)
{
    uint8_t buf[1] = {0};
    assert(elib_ft_bitmap_set(buf, 8, 8) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_bitmap_get(buf, 8, 8) == 0);
}

static void test_bitmap_clear_out_of_range(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_clear(buf, 8, 8) == ELIB_FT_ERR_INVALID_PARAM);
}

static void test_bitmap_null_buf(void)
{
    assert(elib_ft_bitmap_set(NULL, 8, 0) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_bitmap_clear(NULL, 8, 0) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_bitmap_get(NULL, 8, 0) == 0);
}

static void test_bitmap_set_all(void)
{
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set_all(buf, 10);
    /* 10 bits: 0b11_1111_1111 => buf[0]=0xFF, buf[1]=0x03 */
    assert(buf[0] == 0xFF);
    assert(buf[1] == 0x03);
}

static void test_bitmap_set_all_byte_aligned(void)
{
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set_all(buf, 16);
    assert(buf[0] == 0xFF);
    assert(buf[1] == 0xFF);
}

static void test_bitmap_clear_all(void)
{
    uint8_t buf[2] = {0xFF, 0xFF};
    elib_ft_bitmap_clear_all(buf, 16);
    assert(buf[0] == 0x00);
    assert(buf[1] == 0x00);
}

static void test_bitmap_shift_left(void)
{
    /* bits: 0,1,2 set -> shift left 1 -> bits: 1,2,3 set */
    uint8_t buf[1] = {0};
    elib_ft_bitmap_set(buf, 8, 0);
    elib_ft_bitmap_set(buf, 8, 1);
    elib_ft_bitmap_set(buf, 8, 2);
    elib_ft_bitmap_shift_left(buf, 8, 1);
    assert(elib_ft_bitmap_get(buf, 8, 0) == 0);
    assert(elib_ft_bitmap_get(buf, 8, 1) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 2) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 3) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 4) == 0);
}

static void test_bitmap_shift_left_overflow(void)
{
    uint8_t buf[1] = {0};
    elib_ft_bitmap_set(buf, 8, 7);
    elib_ft_bitmap_shift_left(buf, 8, 1);
    /* bit 7 shifted out, lost */
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_left_cross_byte(void)
{
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set(buf, 16, 0);
    elib_ft_bitmap_shift_left(buf, 16, 8);
    assert(elib_ft_bitmap_get(buf, 16, 0) == 0);
    assert(elib_ft_bitmap_get(buf, 16, 8) == 1);
    assert(buf[0] == 0x00);
    assert(buf[1] == 0x01);
}

static void test_bitmap_shift_left_all(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_left(buf, 8, 8);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_left_large(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_left(buf, 8, 100);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_right(void)
{
    /* bits: 5,6,7 set -> shift right 1 -> bits: 4,5,6 set */
    uint8_t buf[1] = {0xE0};
    elib_ft_bitmap_shift_right(buf, 8, 1);
    assert(buf[0] == 0x70);
    assert(elib_ft_bitmap_get(buf, 8, 6) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 5) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 4) == 1);
    assert(elib_ft_bitmap_get(buf, 8, 7) == 0);
}

static void test_bitmap_shift_right_overflow(void)
{
    uint8_t buf[1] = {0x01};
    elib_ft_bitmap_shift_right(buf, 8, 1);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_right_cross_byte(void)
{
    uint8_t buf[2] = {0x00, 0x01};
    elib_ft_bitmap_shift_right(buf, 16, 8);
    assert(elib_ft_bitmap_get(buf, 16, 0) == 1);
    assert(buf[0] == 0x01);
    assert(buf[1] == 0x00);
}

static void test_bitmap_shift_right_all(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_right(buf, 8, 8);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_right_large(void)
{
    uint8_t buf[1] = {0xFF};
    elib_ft_bitmap_shift_right(buf, 8, 100);
    assert(buf[0] == 0x00);
}

static void test_bitmap_shift_zero(void)
{
    uint8_t buf[1] = {0xAA};
    elib_ft_bitmap_shift_left(buf, 8, 0);
    assert(buf[0] == 0xAA);
    elib_ft_bitmap_shift_right(buf, 8, 0);
    assert(buf[0] == 0xAA);
}

static void test_bitmap_non_byte_aligned(void)
{
    /* 12 bits across 2 bytes */
    uint8_t buf[2] = {0};
    elib_ft_bitmap_set(buf, 12, 9);
    assert(elib_ft_bitmap_get(buf, 12, 9) == 1);
    assert(buf[1] == 0x02);

    elib_ft_bitmap_shift_right(buf, 12, 1);
    assert(elib_ft_bitmap_get(buf, 12, 8) == 1);
    assert(elib_ft_bitmap_get(buf, 12, 9) == 0);
}

static void test_bitmap_null_shift(void)
{
    elib_ft_bitmap_shift_left(NULL, 8, 1);
    elib_ft_bitmap_shift_right(NULL, 8, 1);
}

static void test_bitmap_null_set_clear_all(void)
{
    elib_ft_bitmap_set_all(NULL, 8);
    elib_ft_bitmap_clear_all(NULL, 8);
    elib_ft_bitmap_set_all((uint8_t *)1, 0);
    elib_ft_bitmap_clear_all((uint8_t *)1, 0);
}

static void test_bitmap_find_first_set_basic(void)
{
    /* buf = 0b00100100, bits 2 and 5 are set */
    uint8_t buf[1] = {0x24};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 0) == 2);
    assert(elib_ft_bitmap_find_first_set(buf, 8, 3) == 5);
    assert(elib_ft_bitmap_find_first_set(buf, 8, 6) == 8);
}

static void test_bitmap_find_first_set_none(void)
{
    uint8_t buf[1] = {0x00};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 0) == 8);
}

static void test_bitmap_find_first_set_all_set(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 0) == 0);
    assert(elib_ft_bitmap_find_first_set(buf, 8, 7) == 7);
}

static void test_bitmap_find_first_set_cross_byte(void)
{
    /* bit 9 set */
    uint8_t buf[2] = {0x00, 0x02};
    assert(elib_ft_bitmap_find_first_set(buf, 16, 0) == 9);
}

static void test_bitmap_find_first_clear_basic(void)
{
    /* buf = 0b11011011, bits 2 and 5 are clear */
    uint8_t buf[1] = {0xDB};
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 0) == 2);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 3) == 5);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 6) == 8);
}

static void test_bitmap_find_first_clear_none(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 0) == 8);
}

static void test_bitmap_find_first_clear_all_clear(void)
{
    uint8_t buf[1] = {0x00};
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 0) == 0);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 7) == 7);
}

static void test_bitmap_find_first_clear_cross_byte(void)
{
    /* bit 10 clear */
    uint8_t buf[2] = {0xFF, 0xFB};
    assert(elib_ft_bitmap_find_first_clear(buf, 16, 0) == 10);
}

static void test_bitmap_find_null_buf(void)
{
    assert(elib_ft_bitmap_find_first_set(NULL, 8, 0) == 8);
    assert(elib_ft_bitmap_find_first_clear(NULL, 8, 0) == 8);
}

static void test_bitmap_find_start_out_of_range(void)
{
    uint8_t buf[1] = {0xFF};
    assert(elib_ft_bitmap_find_first_set(buf, 8, 8) == 8);
    assert(elib_ft_bitmap_find_first_clear(buf, 8, 8) == 8);
}

static void test_bitmap_find_non_byte_aligned(void)
{
    /* 10 bits, bit 9 set */
    uint8_t buf[2] = {0x00, 0x02};
    assert(elib_ft_bitmap_find_first_set(buf, 10, 0) == 9);
    assert(elib_ft_bitmap_find_first_set(buf, 10, 10) == 10);
}

/* ------------------------------------------------------------------ */
/*  elib_ft_checksum tests                                             */
/* ------------------------------------------------------------------ */

/* "123456789" — standard CRC test vector */
static const uint8_t chk_test_data[] = {
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};

static void test_sum8_basic(void)
{
    elib_ft_sum8_ctx_t ctx;
    elib_ft_sum8_init(&ctx);
    elib_ft_sum8_update(&ctx, chk_test_data, 9);
    uint8_t r = elib_ft_sum8_final(&ctx);
    /* 0x31+0x32+...+0x39 = 477 = 0x1DD, mod 256 = 0xDD */
    assert(r == 0xDD);
}

static void test_sum8_multiframe(void)
{
    elib_ft_sum8_ctx_t ctx;
    elib_ft_sum8_init(&ctx);
    elib_ft_sum8_update(&ctx, chk_test_data, 4);
    elib_ft_sum8_update(&ctx, chk_test_data + 4, 5);
    uint8_t r = elib_ft_sum8_final(&ctx);
    assert(r == 0xDD);
}

static void test_sum8_oneshot(void)
{
    assert(elib_ft_sum8(chk_test_data, 9) == 0xDD);
}

static void test_sum8_null(void)
{
    elib_ft_sum8_init(NULL);
    elib_ft_sum8_update(NULL, chk_test_data, 9);
    elib_ft_sum8_update((elib_ft_sum8_ctx_t *)1, NULL, 9);
    assert(elib_ft_sum8_final(NULL) == 0);
    assert(elib_ft_sum8(NULL, 9) == 0);
    assert(elib_ft_sum8(chk_test_data, 0) == 0);
}

static void test_crc8_ccitt(void)
{
    /* CRC8-CCITT: poly=0x07, init=0x00, ref=0 → "123456789" = 0xF4 */
    elib_ft_crc8_ctx_t ctx;
    ELIB_FT_CRC8_CCITT_INIT(&ctx);
    elib_ft_crc8_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc8_final(&ctx) == 0xF4);
}

static void test_crc8_multiframe(void)
{
    elib_ft_crc8_ctx_t ctx;
    ELIB_FT_CRC8_CCITT_INIT(&ctx);
    elib_ft_crc8_update(&ctx, chk_test_data, 4);
    elib_ft_crc8_update(&ctx, chk_test_data + 4, 5);
    assert(elib_ft_crc8_final(&ctx) == 0xF4);
}

static void test_crc8_oneshot(void)
{
    assert(elib_ft_crc8(chk_test_data, 9, 0x07, 0x00, 0x00, 0) == 0xF4);
}

static void test_crc8_null(void)
{
    elib_ft_crc8_init(NULL, 0x07, 0x00, 0x00, 0);
    elib_ft_crc8_update(NULL, chk_test_data, 9);
    elib_ft_crc8_ctx_t ctx;
    elib_ft_crc8_update(&ctx, NULL, 9);
    assert(elib_ft_crc8_final(NULL) == 0);
    assert(elib_ft_crc8(NULL, 9, 0x07, 0x00, 0x00, 0) == 0);
}

static void test_crc16_xmodem(void)
{
    /* CRC16-XMODEM: poly=0x1021, init=0x0000, ref=0 → "123456789" = 0x31C3 */
    elib_ft_crc16_ctx_t ctx;
    ELIB_FT_CRC16_XMODEM_INIT(&ctx);
    elib_ft_crc16_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc16_final(&ctx) == 0x31C3);
}

static void test_crc16_modbus(void)
{
    /* CRC16-MODBUS: poly=0xA001, init=0xFFFF, ref=1 → "123456789" = 0x4B37 */
    elib_ft_crc16_ctx_t ctx;
    ELIB_FT_CRC16_MODBUS_INIT(&ctx);
    elib_ft_crc16_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc16_final(&ctx) == 0x4B37);
}

static void test_crc16_multiframe(void)
{
    elib_ft_crc16_ctx_t ctx;
    ELIB_FT_CRC16_XMODEM_INIT(&ctx);
    elib_ft_crc16_update(&ctx, chk_test_data, 4);
    elib_ft_crc16_update(&ctx, chk_test_data + 4, 5);
    assert(elib_ft_crc16_final(&ctx) == 0x31C3);
}

static void test_crc16_oneshot(void)
{
    assert(elib_ft_crc16(chk_test_data, 9, 0x1021, 0x0000, 0x0000, 0) == 0x31C3);
    assert(elib_ft_crc16(chk_test_data, 9, 0xA001, 0xFFFF, 0x0000, 1) == 0x4B37);
}

static void test_crc16_null(void)
{
    elib_ft_crc16_init(NULL, 0x1021, 0x0000, 0x0000, 0);
    elib_ft_crc16_update(NULL, chk_test_data, 9);
    elib_ft_crc16_ctx_t ctx;
    elib_ft_crc16_update(&ctx, NULL, 9);
    assert(elib_ft_crc16_final(NULL) == 0);
    assert(elib_ft_crc16(NULL, 9, 0x1021, 0x0000, 0x0000, 0) == 0);
}

static void test_crc32_ethernet(void)
{
    /* CRC32-Ethernet: poly=0xEDB88320, init=0xFFFFFFFF, xor_out=0xFFFFFFFF, ref=1
     * "123456789" → 0xCBF43926 */
    elib_ft_crc32_ctx_t ctx;
    ELIB_FT_CRC32_ETHERNET_INIT(&ctx);
    elib_ft_crc32_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc32_final(&ctx) == 0xCBF43926u);
}

static void test_crc32_multiframe(void)
{
    elib_ft_crc32_ctx_t ctx;
    ELIB_FT_CRC32_ETHERNET_INIT(&ctx);
    elib_ft_crc32_update(&ctx, chk_test_data, 4);
    elib_ft_crc32_update(&ctx, chk_test_data + 4, 5);
    assert(elib_ft_crc32_final(&ctx) == 0xCBF43926u);
}

static void test_crc32_oneshot(void)
{
    assert(elib_ft_crc32(chk_test_data, 9, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1) == 0xCBF43926u);
}

static void test_crc32_null(void)
{
    elib_ft_crc32_init(NULL, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1);
    elib_ft_crc32_update(NULL, chk_test_data, 9);
    elib_ft_crc32_ctx_t ctx;
    elib_ft_crc32_update(&ctx, NULL, 9);
    assert(elib_ft_crc32_final(NULL) == 0);
    assert(elib_ft_crc32(NULL, 9, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1) == 0);
}

static void test_crc32_msb_first(void)
{
    /* CRC32-ISO-HDLC MSB-first: poly=0x04C11DB7, init=0xFFFFFFFF, xor_out=0xFFFFFFFF, ref=0
     * "123456789" → 0xCBF43926 */
    elib_ft_crc32_ctx_t ctx;
    elib_ft_crc32_init(&ctx, 0x04C11DB7u, 0xFFFFFFFFu, 0xFFFFFFFFu, 0);
    elib_ft_crc32_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc32_final(&ctx) == 0xCBF43926u);
}

/* ------------------------------------------------------------------ */
/*  elib_ft_str tests                                                   */
/* ------------------------------------------------------------------ */

static void test_strlen_normal(void)
{
    assert(elib_ft_strlen("hello", 10) == 5);
}

static void test_strlen_with_limit(void)
{
    assert(elib_ft_strlen("hello", 3) == 3);
}

static void test_strlen_null(void)
{
    assert(elib_ft_strlen(NULL, 10) == 0);
}

static void test_strlen_empty(void)
{
    assert(elib_ft_strlen("", 10) == 0);
}

static void test_strcmp_equal(void)
{
    assert(elib_ft_strcmp("hello", "hello") == 0);
}

static void test_strcmp_less(void)
{
    assert(elib_ft_strcmp("abc", "abd") < 0);
}

static void test_strcmp_greater(void)
{
    assert(elib_ft_strcmp("abd", "abc") > 0);
}

static void test_strcmp_null_s1(void)
{
    assert(elib_ft_strcmp(NULL, "abc") < 0);
}

static void test_strcmp_null_s2(void)
{
    assert(elib_ft_strcmp("abc", NULL) > 0);
}

static void test_strcmp_both_null(void)
{
    assert(elib_ft_strcmp(NULL, NULL) == 0);
}

static void test_strncmp_equal(void)
{
    assert(elib_ft_strncmp("hello", "hello", 5) == 0);
}

static void test_strncmp_prefix(void)
{
    assert(elib_ft_strncmp("hello", "helloworld", 5) == 0);
}

static void test_strncmp_less(void)
{
    assert(elib_ft_strncmp("abc", "abd", 3) < 0);
}

static void test_strncmp_greater(void)
{
    assert(elib_ft_strncmp("abd", "abc", 3) > 0);
}

static void test_strncmp_zero_n(void)
{
    assert(elib_ft_strncmp("abc", "xyz", 0) == 0);
}

static void test_strncmp_null_s1(void)
{
    assert(elib_ft_strncmp(NULL, "abc", 3) < 0);
}

static void test_strncmp_null_s2(void)
{
    assert(elib_ft_strncmp("abc", NULL, 3) > 0);
}

static void test_strncmp_null_both(void)
{
    assert(elib_ft_strncmp(NULL, NULL, 3) == 0);
}

static void test_strncmp_null_n(void)
{
    assert(elib_ft_strncmp("abc", "abc", 0) == 0);
}

static void test_atoi_normal(void)
{
    const char *end;
    assert(elib_ft_atoi("12345", 10, &end) == 12345);
    assert(*end == '\0');
}

static void test_atoi_with_limit(void)
{
    const char *end;
    assert(elib_ft_atoi("12345", 3, &end) == 12);
    assert(*end == '3');
}

static void test_atoi_multiframe(void)
{
    const char *end;
    assert(elib_ft_atoi("123", 10, &end) == 123);
    assert(*end == '\0');
}

static void test_atoi_zero(void)
{
    const char *end;
    assert(elib_ft_atoi("0", 10, &end) == 0);
    assert(*end == '\0');
}

static void test_atoi_leading_spaces(void)
{
    const char *end;
    assert(elib_ft_atoi("  42abc", 10, &end) == 42);
    assert(*end == 'a');
}

static void test_atoi_invalid(void)
{
    const char *end;
    assert(elib_ft_atoi("abc", 10, &end) == 0);
}

static void test_atoi_null(void)
{
    const char *end;
    assert(elib_ft_atoi(NULL, 10, &end) == 0);
    assert(end == NULL);
}

static void test_atoi_null_endptr(void)
{
    assert(elib_ft_atoi("123", 10, NULL) == 123);
}

static void test_atoi_empty(void)
{
    const char *end;
    assert(elib_ft_atoi("", 10, &end) == 0);
}

static void test_atoi_overflow(void)
{
    const char *end;
    /* 4000000000 > 2^32-1 */
    assert(elib_ft_atoi("4000000000", 10, &end) == 4000000000U);
}

/* ------------------------------------------------------------------ */
/*  elib_ft_endian tests                                               */
/* ------------------------------------------------------------------ */

static void test_swap16(void)
{
    assert(elib_ft_swap16(0x1234) == 0x3412);
    assert(elib_ft_swap16(0x00FF) == 0xFF00);
    assert(elib_ft_swap16(0xABCD) == 0xCDAB);
}

static void test_swap32(void)
{
    assert(elib_ft_swap32(0x12345678) == 0x78563412);
    assert(elib_ft_swap32(0x11223344) == 0x44332211);
    assert(elib_ft_swap32(0xAABBCCDD) == 0xDDCCBBAABB);
}

static void test_swap32_zero(void)
{
    assert(elib_ft_swap32(0) == 0);
}

static void test_swap32_pattern(void)
{
    /* 0x01020304 -> 0x04030201 */
    assert(elib_ft_swap32(0x01020304) == 0x04030201);
}

/* ------------------------------------------------------------------ */
/*  Main                                                               */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("elib-functiontool test suite\n");

    /* memcpy tests */
    RUN_TEST(test_memcpy_normal);
    RUN_TEST(test_memcpy_truncate);
    RUN_TEST(test_memcpy_len_eq_max);
    RUN_TEST(test_memcpy_zero_len);
    RUN_TEST(test_memcpy_zero_max);
    RUN_TEST(test_memcpy_null_dst);
    RUN_TEST(test_memcpy_null_src);
    RUN_TEST(test_memcpy_single_byte);

    /* memset tests */
    RUN_TEST(test_memset_normal);
    RUN_TEST(test_memset_truncate);
    RUN_TEST(test_memset_len_eq_max);
    RUN_TEST(test_memset_zero_len);
    RUN_TEST(test_memset_zero_max);
    RUN_TEST(test_memset_null_dst);
    RUN_TEST(test_memset_zero_value);
    RUN_TEST(test_memset_single_byte);

    printf("\nAll 16 tests passed.\n");

    /* ringbuf tests */
    printf("\n");
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

    /* list tests */
    printf("\n");
    RUN_TEST(test_list_init);
    RUN_TEST(test_list_init_null);
    RUN_TEST(test_list_push_front);
    RUN_TEST(test_list_push_back);
    RUN_TEST(test_list_pop_front);
    RUN_TEST(test_list_pop_back);
    RUN_TEST(test_list_pop_empty);
    RUN_TEST(test_list_remove);
    RUN_TEST(test_list_remove_head);
    RUN_TEST(test_list_remove_tail);
    RUN_TEST(test_list_insert_after);
    RUN_TEST(test_list_insert_before);
    RUN_TEST(test_list_insert_before_head);
    RUN_TEST(test_list_entry_macro);
    RUN_TEST(test_list_for_each_safe);
    RUN_TEST(test_list_single_node);
    RUN_TEST(test_list_null_params);

    /* bitmap tests */
    printf("\n");
    RUN_TEST(test_bitmap_set_get);
    RUN_TEST(test_bitmap_clear);
    RUN_TEST(test_bitmap_set_out_of_range);
    RUN_TEST(test_bitmap_clear_out_of_range);
    RUN_TEST(test_bitmap_null_buf);
    RUN_TEST(test_bitmap_set_all);
    RUN_TEST(test_bitmap_set_all_byte_aligned);
    RUN_TEST(test_bitmap_clear_all);
    RUN_TEST(test_bitmap_shift_left);
    RUN_TEST(test_bitmap_shift_left_overflow);
    RUN_TEST(test_bitmap_shift_left_cross_byte);
    RUN_TEST(test_bitmap_shift_left_all);
    RUN_TEST(test_bitmap_shift_left_large);
    RUN_TEST(test_bitmap_shift_right);
    RUN_TEST(test_bitmap_shift_right_overflow);
    RUN_TEST(test_bitmap_shift_right_cross_byte);
    RUN_TEST(test_bitmap_shift_right_all);
    RUN_TEST(test_bitmap_shift_right_large);
    RUN_TEST(test_bitmap_shift_zero);
    RUN_TEST(test_bitmap_non_byte_aligned);
    RUN_TEST(test_bitmap_null_shift);
    RUN_TEST(test_bitmap_null_set_clear_all);

    /* bitmap find tests */
    RUN_TEST(test_bitmap_find_first_set_basic);
    RUN_TEST(test_bitmap_find_first_set_none);
    RUN_TEST(test_bitmap_find_first_set_all_set);
    RUN_TEST(test_bitmap_find_first_set_cross_byte);
    RUN_TEST(test_bitmap_find_first_clear_basic);
    RUN_TEST(test_bitmap_find_first_clear_none);
    RUN_TEST(test_bitmap_find_first_clear_all_clear);
    RUN_TEST(test_bitmap_find_first_clear_cross_byte);
    RUN_TEST(test_bitmap_find_null_buf);
    RUN_TEST(test_bitmap_find_start_out_of_range);
    RUN_TEST(test_bitmap_find_non_byte_aligned);

    /* checksum tests */
    printf("\n");
    RUN_TEST(test_sum8_basic);
    RUN_TEST(test_sum8_multiframe);
    RUN_TEST(test_sum8_oneshot);
    RUN_TEST(test_sum8_null);
    RUN_TEST(test_crc8_ccitt);
    RUN_TEST(test_crc8_multiframe);
    RUN_TEST(test_crc8_oneshot);
    RUN_TEST(test_crc8_null);
    RUN_TEST(test_crc16_xmodem);
    RUN_TEST(test_crc16_modbus);
    RUN_TEST(test_crc16_multiframe);
    RUN_TEST(test_crc16_oneshot);
    RUN_TEST(test_crc16_null);
    RUN_TEST(test_crc32_ethernet);
    RUN_TEST(test_crc32_multiframe);
    RUN_TEST(test_crc32_oneshot);
    RUN_TEST(test_crc32_null);
    RUN_TEST(test_crc32_msb_first);

    /* str tests */
    printf("\n");
    RUN_TEST(test_strlen_normal);
    RUN_TEST(test_strlen_with_limit);
    RUN_TEST(test_strlen_null);
    RUN_TEST(test_strlen_empty);
    RUN_TEST(test_strcmp_equal);
    RUN_TEST(test_strcmp_less);
    RUN_TEST(test_strcmp_greater);
    RUN_TEST(test_strcmp_null_s1);
    RUN_TEST(test_strcmp_null_s2);
    RUN_TEST(test_strcmp_both_null);
    RUN_TEST(test_strncmp_equal);
    RUN_TEST(test_strncmp_prefix);
    RUN_TEST(test_strncmp_less);
    RUN_TEST(test_strncmp_greater);
    RUN_TEST(test_strncmp_zero_n);
    RUN_TEST(test_strncmp_null_s1);
    RUN_TEST(test_strncmp_null_s2);
    RUN_TEST(test_strncmp_null_both);
    RUN_TEST(test_strncmp_null_n);
    RUN_TEST(test_atoi_normal);
    RUN_TEST(test_atoi_with_limit);
    RUN_TEST(test_atoi_multiframe);
    RUN_TEST(test_atoi_zero);
    RUN_TEST(test_atoi_leading_spaces);
    RUN_TEST(test_atoi_invalid);
    RUN_TEST(test_atoi_null);
    RUN_TEST(test_atoi_null_endptr);
    RUN_TEST(test_atoi_empty);
    RUN_TEST(test_atoi_overflow);

    /* endian tests */
    printf("\n");
    RUN_TEST(test_swap16);
    RUN_TEST(test_swap32);
    RUN_TEST(test_swap32_zero);
    RUN_TEST(test_swap32_pattern);

    /* bit tests */
    printf("\n");
    RUN_TEST(test_popcount8_all_set);
    RUN_TEST(test_popcount8_none_set);
    RUN_TEST(test_popcount8_mixed);
    RUN_TEST(test_popcount16_all_set);
    RUN_TEST(test_popcount16_mixed);
    RUN_TEST(test_popcount32_all_set);
    RUN_TEST(test_popcount32_mixed);
    RUN_TEST(test_find_next_set_basic);
    RUN_TEST(test_find_next_set_none);
    RUN_TEST(test_find_next_set_all);
    RUN_TEST(test_find_next_set_cross_byte);
    RUN_TEST(test_find_next_set_out_of_range);
    RUN_TEST(test_find_next_set_null);
    RUN_TEST(test_find_next_set_non_byte_aligned);
    RUN_TEST(test_find_next_clear_basic);
    RUN_TEST(test_find_next_clear_none);
    RUN_TEST(test_find_next_clear_all);
    RUN_TEST(test_find_next_clear_cross_byte);
    RUN_TEST(test_find_next_clear_out_of_range);
    RUN_TEST(test_find_next_clear_null);

    printf("\nAll %d tests passed.\n", 98 + 30);
    return 0;
}
