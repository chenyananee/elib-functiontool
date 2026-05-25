/* elib_ft_list.h - FunctionTool Intrusive Doubly-Linked List */

#ifndef ELIB_FT_LIST_H
#define ELIB_FT_LIST_H

#include <stdint.h>
#include "elib_ft_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct elib_ft_list_node {
    struct elib_ft_list_node *next;
    struct elib_ft_list_node *prev;
} elib_ft_list_node_t;

typedef struct {
    elib_ft_list_node_t *head;
    uint32_t             count;
} elib_ft_list_t;

/* ------------------------------------------------------------------ */
/*  Macros                                                             */
/* ------------------------------------------------------------------ */

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
 * @brief Iterate over a list
 * @param cursor elib_ft_list_node_t * iterator
 * @param list Pointer to the list
 */
#define ELIB_FT_LIST_FOR_EACH(cursor, list) \
    for ((cursor) = (list)->head; (cursor) != NULL; (cursor) = (cursor)->next)

/**
 * @brief Iterate over a list safe against removal of current node
 * @param cursor elib_ft_list_node_t * iterator
 * @param tmp Temporary elib_ft_list_node_t * for safe removal
 * @param list Pointer to the list
 */
#define ELIB_FT_LIST_FOR_EACH_SAFE(cursor, tmp, list) \
    for ((cursor) = (list)->head; \
         (cursor) != NULL && ((tmp) = (cursor)->next, 1); \
         (cursor) = (tmp))

/* ------------------------------------------------------------------ */
/*  Functions                                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Initialize a list
 * @param list Pointer to the list
 */
void elib_ft_list_init(elib_ft_list_t *list);

/**
 * @brief Check if a list is empty
 * @param list Pointer to the list
 * @return 1 if empty, 0 if not
 */
int elib_ft_list_empty(const elib_ft_list_t *list);

/**
 * @brief Get the number of nodes in the list
 * @param list Pointer to the list
 * @return Node count
 */
uint32_t elib_ft_list_count(const elib_ft_list_t *list);

/**
 * @brief Insert node at head of list
 * @param list Pointer to the list
 * @param node Node to insert
 */
void elib_ft_list_push_front(elib_ft_list_t *list, elib_ft_list_node_t *node);

/**
 * @brief Insert node at tail of list
 * @param list Pointer to the list
 * @param node Node to insert
 */
void elib_ft_list_push_back(elib_ft_list_t *list, elib_ft_list_node_t *node);

/**
 * @brief Remove the first node from the list
 * @param list Pointer to the list
 * @return Removed node, or NULL if list is empty
 */
elib_ft_list_node_t *elib_ft_list_pop_front(elib_ft_list_t *list);

/**
 * @brief Remove the last node from the list
 * @param list Pointer to the list
 * @return Removed node, or NULL if list is empty
 */
elib_ft_list_node_t *elib_ft_list_pop_back(elib_ft_list_t *list);

/**
 * @brief Remove a specific node from its list
 * @param list Pointer to the list
 * @param node Node to remove
 */
void elib_ft_list_remove(elib_ft_list_t *list, elib_ft_list_node_t *node);

/**
 * @brief Insert node after an existing node
 * @param list Pointer to the list
 * @param after Existing node in the list
 * @param node Node to insert
 */
void elib_ft_list_insert_after(elib_ft_list_t *list, elib_ft_list_node_t *after, elib_ft_list_node_t *node);

/**
 * @brief Insert node before an existing node
 * @param list Pointer to the list
 * @param before Existing node in the list
 * @param node Node to insert
 */
void elib_ft_list_insert_before(elib_ft_list_t *list, elib_ft_list_node_t *before, elib_ft_list_node_t *node);

#ifdef __cplusplus
}
#endif

#endif /* ELIB_FT_LIST_H */
