/* test_elib_ft_list.c - FunctionTool Intrusive List Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};
    test_item_t c = {3, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};
    test_item_t c = {3, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};
    test_item_t c = {3, {NULL, NULL, NULL}};

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

    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

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
        {10, {NULL, NULL, NULL}},
        {20, {NULL, NULL, NULL}},
        {30, {NULL, NULL, NULL}},
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
        {1, {NULL, NULL, NULL}},
        {2, {NULL, NULL, NULL}},
        {3, {NULL, NULL, NULL}},
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

    test_item_t a = {42, {NULL, NULL, NULL}};
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
    test_item_t a = {1, {NULL, NULL, NULL}};
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

static void test_list_cross_list_reject(void)
{
    elib_ft_list_t l1, l2;
    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

    assert(elib_ft_list_init(&l1) == ELIB_FT_OK);
    assert(elib_ft_list_init(&l2) == ELIB_FT_OK);
    assert(elib_ft_list_push_back(&l1, &a.node) == ELIB_FT_OK);

    /* a is owned by l1: joining l2 is rejected, neither list corrupts */
    assert(elib_ft_list_push_back(&l2, &a.node) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_list_count(&l1) == 1);
    assert(elib_ft_list_count(&l2) == 0);

    /* cross-list removal: owner knows better than the caller */
    assert(elib_ft_list_push_back(&l2, &b.node) == ELIB_FT_OK);
    assert(elib_ft_list_remove(&l1, &b.node) == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_list_remove(&l2, &b.node) == ELIB_FT_OK);
    assert(elib_ft_list_remove(&l2, &b.node) == ELIB_FT_ERR_NO_DATA);
}

static void test_list_tail_order(void)
{
    elib_ft_list_t l;
    test_item_t x[4] = {
        {1, {NULL, NULL, NULL}}, {2, {NULL, NULL, NULL}},
        {3, {NULL, NULL, NULL}}, {4, {NULL, NULL, NULL}}
    };

    assert(elib_ft_list_init(&l) == ELIB_FT_OK);
    assert(elib_ft_list_push_front(&l, &x[0].node) == ELIB_FT_OK);
    assert(elib_ft_list_pop_front(&l) == &x[0].node);
    assert(elib_ft_list_pop_front(&l) == NULL);

    assert(elib_ft_list_push_back(&l, &x[1].node) == ELIB_FT_OK);
    assert(elib_ft_list_push_front(&l, &x[2].node) == ELIB_FT_OK);
    assert(elib_ft_list_push_back(&l, &x[3].node) == ELIB_FT_OK);

    assert(elib_ft_list_pop_front(&l) == &x[2].node);
    assert(elib_ft_list_pop_back(&l) == &x[3].node);
    assert(elib_ft_list_pop_back(&l) == &x[1].node);
    assert(elib_ft_list_count(&l) == 0);
}

static void test_list_insert_into_detached(void)
{
    elib_ft_list_t l;
    test_item_t a = {1, {NULL, NULL, NULL}};
    test_item_t b = {2, {NULL, NULL, NULL}};

    assert(elib_ft_list_init(&l) == ELIB_FT_OK);
    /* anchors must be inside the list, nodes must be detached */
    assert(elib_ft_list_insert_after(&l, &a.node, &b.node)
           == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_list_insert_before(&l, &a.node, &b.node)
           == ELIB_FT_ERR_INVALID_PARAM);
    assert(elib_ft_list_count(&l) == 0);

    assert(elib_ft_list_push_back(&l, &a.node) == ELIB_FT_OK);
    assert(elib_ft_list_insert_before(&l, &a.node, &b.node) == ELIB_FT_OK);
    assert(elib_ft_list_pop_front(&l) == &b.node);
}

int main(void)
{
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

    RUN_TEST(test_list_cross_list_reject);
    RUN_TEST(test_list_tail_order);
    RUN_TEST(test_list_insert_into_detached);
    printf("\nAll %d tests passed.\n", 20);
    return 0;
}
