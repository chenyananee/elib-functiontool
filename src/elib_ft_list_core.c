/* elib_ft_list_core.c - FunctionTool Intrusive List Core Implementation */

#include "elib_ft_list_core.h"

void elib_ft_list_init(elib_ft_list_t *list)
{
    if (list == NULL) {
        return;
    }
    list->head = NULL;
    list->count = 0;
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

void elib_ft_list_push_front(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (list == NULL || node == NULL) {
        return;
    }

    node->prev = NULL;
    node->next = list->head;

    if (list->head != NULL) {
        list->head->prev = node;
    }
    list->head = node;
    list->count++;
}

void elib_ft_list_push_back(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (list == NULL || node == NULL) {
        return;
    }

    node->next = NULL;

    if (list->head == NULL) {
        node->prev = NULL;
        list->head = node;
    } else {
        elib_ft_list_node_t *tail = list->head;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        node->prev = tail;
        tail->next = node;
    }
    list->count++;
}

elib_ft_list_node_t *elib_ft_list_pop_front(elib_ft_list_t *list)
{
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    elib_ft_list_node_t *node = list->head;
    list->head = node->next;

    if (list->head != NULL) {
        list->head->prev = NULL;
    }
    node->next = NULL;
    node->prev = NULL;
    list->count--;

    return node;
}

elib_ft_list_node_t *elib_ft_list_pop_back(elib_ft_list_t *list)
{
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    elib_ft_list_node_t *tail = list->head;
    while (tail->next != NULL) {
        tail = tail->next;
    }

    if (tail->prev != NULL) {
        tail->prev->next = NULL;
    } else {
        list->head = NULL;
    }

    tail->next = NULL;
    tail->prev = NULL;
    list->count--;

    return tail;
}

void elib_ft_list_remove(elib_ft_list_t *list, elib_ft_list_node_t *node)
{
    if (list == NULL || node == NULL) {
        return;
    }

    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    node->next = NULL;
    node->prev = NULL;
    list->count--;
}

void elib_ft_list_insert_after(elib_ft_list_t *list, elib_ft_list_node_t *after, elib_ft_list_node_t *node)
{
    if (list == NULL || after == NULL || node == NULL) {
        return;
    }

    node->prev = after;
    node->next = after->next;

    if (after->next != NULL) {
        after->next->prev = node;
    }
    after->next = node;
    list->count++;
}

void elib_ft_list_insert_before(elib_ft_list_t *list, elib_ft_list_node_t *before, elib_ft_list_node_t *node)
{
    if (list == NULL || before == NULL || node == NULL) {
        return;
    }

    node->prev = before->prev;
    node->next = before;

    if (before->prev != NULL) {
        before->prev->next = node;
    } else {
        list->head = node;
    }
    before->prev = node;
    list->count++;
}
