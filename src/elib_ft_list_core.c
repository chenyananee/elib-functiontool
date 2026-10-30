/* elib_ft_list_core.c - FunctionTool Intrusive List Core Implementation */

#include "elib_ft.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* Unlink a node whose owner is confirmed to be `list`, fix head/tail
 * and count. Used by remove and pop. */
static void list_unlink(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    node->next = NULL;
    node->prev = NULL;
    node->owner = NULL;
    list->count--;
}

/* Link a detached node after `after` (after == NULL means at head). */
static void list_link(elib_ft_list_t *list, elib_ft_list_node_t *after, elib_ft_list_node_t *node)
{
    if (after == NULL) {
        node->prev = NULL;
        node->next = list->head;
        if (list->head != NULL) {
            list->head->prev = node;
        }
        list->head = node;
        if (list->tail == NULL) {
            list->tail = node;
        }
    } else {
        node->prev = after;
        node->next = after->next;
        if (after->next != NULL) {
            after->next->prev = node;
        } else {
            list->tail = node;
        }
        after->next = node;
    }
    node->owner = list;
    list->count++;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

elib_ft_err_t elib_ft_list_init(elib_ft_list_t *list)
{
    if (list == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    memset(list, 0, sizeof(*list));
    return ELIB_FT_OK;
}

int elib_ft_list_empty(const elib_ft_list_t *list)
{
    if (list == NULL) {
        return 1;
    }
    return list->head == NULL;
}

uint32_t elib_ft_list_count(const elib_ft_list_t *list)
{
    if (list == NULL) {
        return 0;
    }
    return list->count;
}

elib_ft_err_t elib_ft_list_push_front(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (list == NULL || node == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (node->owner != NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;   /* already linked into some list */
    }

    list_link(list, NULL, node);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_list_push_back(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (list == NULL || node == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (node->owner != NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    list_link(list, list->tail, node);
    return ELIB_FT_OK;
}

elib_ft_list_node_t *elib_ft_list_pop_front(elib_ft_list_t *list)
{
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    elib_ft_list_node_t *node = list->head;
    list_unlink(list, node);
    return node;
}

elib_ft_list_node_t *elib_ft_list_pop_back(elib_ft_list_t *list)
{
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    elib_ft_list_node_t *node = list->tail;
    list_unlink(list, node);
    return node;
}

elib_ft_err_t elib_ft_list_remove(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (list == NULL || node == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (node->owner == NULL) {
        return ELIB_FT_ERR_NO_DATA;         /* node not linked anywhere */
    }
    if (node->owner != list) {
        return ELIB_FT_ERR_INVALID_PARAM;   /* cross-list removal */
    }

    list_unlink(list, node);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_list_insert_after(elib_ft_list_t *list, elib_ft_list_node_t *after, elib_ft_list_node_t *node)
{
    if (list == NULL || after == NULL || node == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (after->owner != list || node->owner != NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    list_link(list, after, node);
    return ELIB_FT_OK;
}

elib_ft_err_t elib_ft_list_insert_before(elib_ft_list_t *list, elib_ft_list_node_t *before, elib_ft_list_node_t *node)
{
    if (list == NULL || before == NULL || node == NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }
    if (before->owner != list || node->owner != NULL) {
        return ELIB_FT_ERR_INVALID_PARAM;
    }

    list_link(list, before->prev, node);
    return ELIB_FT_OK;
}
