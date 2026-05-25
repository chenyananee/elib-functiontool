# list 模块用法

头文件：`elib_ft_list.h`（通过 `elib_ft.h` 自动引入）

侵入式双向链表，零额外内存开销，节点嵌入用户结构体中。

---

## 基本概念

侵入式链表的节点 `elib_ft_list_node_t` 直接嵌入用户结构体，而非独立分配：

```c
typedef struct {
    int                id;
    elib_ft_list_node_t node;   /* 嵌入链表节点 */
} my_item_t;
```

同一结构体可嵌入多个节点，从而同时属于多个链表：

```c
typedef struct {
    int                  priority;
    int                  timestamp;
    elib_ft_list_node_t  by_priority;  /* 按优先级排列 */
    elib_ft_list_node_t  by_time;      /* 按时间排列 */
} task_t;
```

---

## 初始化

```c
elib_ft_list_t list;
elib_ft_list_init(&list);
/* list.head = NULL, list.count = 0 */
```

---

## 插入节点

```c
my_item_t a = {1, {NULL, NULL}};
my_item_t b = {2, {NULL, NULL}};
my_item_t c = {3, {NULL, NULL}};

/* 尾部插入：1 → 2 → 3 */
elib_ft_list_push_back(&list, &a.node);
elib_ft_list_push_back(&list, &b.node);
elib_ft_list_push_back(&list, &b.node);

/* 头部插入：0 → 1 → 2 → 3 */
my_item_t z = {0, {NULL, NULL}};
elib_ft_list_push_front(&list, &z.node);
```

---

## 在指定位置插入

```c
/* 在 a 之后插入 b */
elib_ft_list_insert_after(&list, &a.node, &b.node);

/* 在 c 之前插入 b */
elib_ft_list_insert_before(&list, &c.node, &b.node);

/* 在头部之前插入（等效于 push_front） */
elib_ft_list_insert_before(&list, list.head, &z.node);
```

---

## 弹出节点

```c
/* 弹出首节点 */
elib_ft_list_node_t *n = elib_ft_list_pop_front(&list);
my_item_t *item = ELIB_FT_LIST_ENTRY(n, my_item_t, node);

/* 弹出尾节点 */
n = elib_ft_list_pop_back(&list);

/* 空链表弹出返回 NULL */
elib_ft_list_pop_front(&list);  /* NULL */
```

弹出后节点的 `next`/`prev` 被置为 NULL，可安全重新插入。

---

## 移除节点

```c
elib_ft_list_remove(&list, &b.node);
/* b.node.next = NULL, b.node.prev = NULL */
```

移除后节点可重新插入同一链表或其他链表。

---

## 遍历

### 基本遍历

```c
elib_ft_list_node_t *cursor;
ELIB_FT_LIST_FOR_EACH(cursor, &list) {
    my_item_t *item = ELIB_FT_LIST_ENTRY(cursor, my_item_t, node);
    printf("id = %d\n", item->id);
}
```

### 安全遍历（支持遍历中删除）

```c
elib_ft_list_node_t *cursor;
elib_ft_list_node_t *tmp;
ELIB_FT_LIST_FOR_EACH_SAFE(cursor, tmp, &list) {
    my_item_t *item = ELIB_FT_LIST_ENTRY(cursor, my_item_t, node);
    if (item->id == 2) {
        elib_ft_list_remove(&list, cursor);
    }
}
```

---

## ELIB_FT_LIST_ENTRY

从节点指针反推宿主结构体指针，类似 Linux 内核的 `container_of`：

```c
my_item_t *item = ELIB_FT_LIST_ENTRY(node_ptr, my_item_t, node);
/*              ^要获取的类型    ^节点成员名  */
```

原理：`结构体地址 = 节点地址 - 节点在结构体内的偏移`

---

## 查询状态

```c
int empty = elib_ft_list_empty(&list);      /* 1=空, 0=非空 */
uint32_t count = elib_ft_list_count(&list);  /* 节点数 */
```

---

## 完整示例

```c
#include "elib_ft.h"
#include <stdio.h>

typedef struct {
    int                id;
    elib_ft_list_node_t node;
} my_item_t;

int main(void)
{
    elib_ft_list_t list;
    elib_ft_list_init(&list);

    my_item_t a = {1, {NULL, NULL}};
    my_item_t b = {2, {NULL, NULL}};
    my_item_t c = {3, {NULL, NULL}};

    elib_ft_list_push_back(&list, &a.node);
    elib_ft_list_push_back(&list, &b.node);
    elib_ft_list_push_back(&list, &c.node);

    /* 遍历：1 2 3 */
    elib_ft_list_node_t *cursor;
    ELIB_FT_LIST_FOR_EACH(cursor, &list) {
        my_item_t *item = ELIB_FT_LIST_ENTRY(cursor, my_item_t, node);
        printf("id = %d\n", item->id);
    }

    /* 移除中间节点 */
    elib_ft_list_remove(&list, &b.node);

    /* 再次遍历：1 3 */
    ELIB_FT_LIST_FOR_EACH(cursor, &list) {
        my_item_t *item = ELIB_FT_LIST_ENTRY(cursor, my_item_t, node);
        printf("id = %d\n", item->id);
    }

    return 0;
}
```
