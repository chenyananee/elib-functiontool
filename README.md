# elib-functiontool

零动态内存分配的嵌入式内存操作工具库，提供带最大长度限制的安全 memcpy/memset、环形缓冲区、侵入式双向链表。

## 特性

- 零动态内存分配，无本地数组
- 纯 C99 实现，无外部依赖
- `elib_ft_memcpy`：带最大长度限制的内存拷贝，超出截断
- `elib_ft_memset`：带最大长度限制的内存填充，超出截断
- `elib_ft_ringbuf`：基于 cell 的环形缓冲区，支持 max_len 安全限制
- `elib_ft_list`：侵入式双向链表，零额外内存开销
- `ELIB_FT_WEAK`：跨编译器 weak 定义宏（IAR/GCC/MDK）
- NULL 指针安全检查

## 目录结构

```
elib-functiontool/
├── include/
│   ├── elib_ft.h                  # 伞形头文件（包含所有子模块）
│   ├── elib_ft_err.h              # 错误码定义
│   ├── elib_ft_mem.h              # 内存操作模块
│   ├── elib_ft_ringbuf.h          # 环形缓冲区模块
│   └── elib_ft_list.h             # 侵入式链表模块
├── src/
│   ├── elib_ft_core.h             # memcpy 内部头文件
│   ├── elib_ft_core.c             # memcpy/memset 实现
│   ├── elib_ft_ringbuf_core.h     # ringbuf 内部头文件
│   ├── elib_ft_ringbuf_core.c     # ringbuf 实现
│   ├── elib_ft_list_core.h        # list 内部头文件
│   └── elib_ft_list_core.c        # list 实现
├── test/
│   └── test_elib_ft.c             # 单元测试
├── scripts/
│   ├── setup-push-remote.sh
│   └── setup-push-remote.bat
├── LICENSE
└── README.md
```

## API 参考

### 编译器兼容宏

| 宏 | 说明 |
|----|------|
| `ELIB_FT_WEAK` | 弱定义宏，自动适配 IAR / GCC / MDK |

### 内存操作

| 函数 / 宏 | 说明 |
|-----------|------|
| `ELIB_FT_ARRAY_SIZE(arr)` | 计算数组元素个数 |
| `elib_ft_memcpy(dst, src, len, max_len)` | 内存拷贝，超出 max_len 截断，返回实际拷贝字节数 |
| `elib_ft_memset(dst, val, len, max_len)` | 内存填充，超出 max_len 截断，返回实际填充字节数 |

### 位图操作

| 函数 | 说明 |
|------|------|
| `elib_ft_bitmap_set(buf, max_bits, bit)` | 置位 |
| `elib_ft_bitmap_clear(buf, max_bits, bit)` | 清位 |
| `elib_ft_bitmap_get(buf, max_bits, bit)` | 获取位值（返回 0 或 1） |
| `elib_ft_bitmap_set_all(buf, max_bits)` | 全部置位 |
| `elib_ft_bitmap_clear_all(buf, max_bits)` | 全部清零 |
| `elib_ft_bitmap_shift_left(buf, max_bits, n)` | 左移 n 位（高位溢出丢弃） |
| `elib_ft_bitmap_shift_right(buf, max_bits, n)` | 右移 n 位（低位溢出丢弃） |
| `elib_ft_bitmap_find_first_set(buf, max_bits, start)` | 从 start 起查找最近的 1，返回位索引，未找到返回 max_bits |
| `elib_ft_bitmap_find_first_clear(buf, max_bits, start)` | 从 start 起查找最近的 0，返回位索引，未找到返回 max_bits |

### 环形缓冲区

| 函数 | 说明 |
|------|------|
| `elib_ft_ringbuf_init(ctx, buf, size, cell_size)` | 初始化环形缓冲区 |
| `elib_ft_ringbuf_deinit(ctx)` | 反初始化 |
| `elib_ft_ringbuf_used(ctx)` | 获取已用 cell 数 |
| `elib_ft_ringbuf_free(ctx)` | 获取空闲 cell 数 |
| `elib_ft_ringbuf_write(ctx, data, len, max_len)` | 写入数据，len > max_len 返回错误 |
| `elib_ft_ringbuf_read(ctx, data, len, max_len)` | 读取数据，len > max_len 返回错误 |
| `elib_ft_ringbuf_reset(ctx)` | 重置缓冲区（丢弃所有数据） |

### 侵入式双向链表

| 函数 / 宏 | 说明 |
|-----------|------|
| `elib_ft_list_init(list)` | 初始化链表 |
| `elib_ft_list_empty(list)` | 判空 |
| `elib_ft_list_count(list)` | 获取节点数 |
| `elib_ft_list_push_front(list, node)` | 头部插入 |
| `elib_ft_list_push_back(list, node)` | 尾部插入 |
| `elib_ft_list_pop_front(list)` | 弹出首节点 |
| `elib_ft_list_pop_back(list)` | 弹出尾节点 |
| `elib_ft_list_remove(list, node)` | 移除指定节点 |
| `elib_ft_list_insert_after(list, after, node)` | 在 after 之后插入 |
| `elib_ft_list_insert_before(list, before, node)` | 在 before 之前插入 |
| `ELIB_FT_LIST_ENTRY(node, type, member)` | 从节点指针获取所在结构体指针 |
| `ELIB_FT_LIST_FOR_EACH(cursor, list)` | 遍历链表 |
| `ELIB_FT_LIST_FOR_EACH_SAFE(cursor, tmp, list)` | 安全遍历（支持遍历中删除） |

### 错误码

| 错误码 | 说明 |
|--------|------|
| `ELIB_FT_OK` | 成功 |
| `ELIB_FT_ERR_INVALID_PARAM` | 无效参数（NULL 指针等） |
| `ELIB_FT_ERR_NOT_INITIALIZED` | 未初始化 |
| `ELIB_FT_ERR_EXCEED_MAX` | len 超过 max_len |
| `ELIB_FT_ERR_NO_SPACE` | 缓冲区空间不足 |
| `ELIB_FT_ERR_NO_DATA` | 缓冲区无数据可读 |

## 使用示例

### memcpy / memset

```c
#include "elib_ft.h"
#include <stdio.h>

int main(void)
{
    uint8_t src[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t dst[8] = {0};

    /* 正常拷贝：len < max_len */
    uint32_t n = elib_ft_memcpy(dst, src, 5, 8);
    /* n = 5, dst = {1,2,3,4,5,0,0,0} */

    /* 截断拷贝：len > max_len */
    n = elib_ft_memcpy(dst, src, 8, 3);
    /* n = 3, dst = {1,2,3,...} 仅拷贝前 3 字节 */

    /* 内存填充 */
    n = elib_ft_memset(dst, 0xAA, 6, 8);
    /* n = 6, dst 前 6 字节均为 0xAA */

    return 0;
}
```

### ringbuf

```c
#include "elib_ft.h"
#include <stdio.h>

int main(void)
{
    /* 用户分配缓冲区 */
    uint8_t buf[8 * sizeof(uint32_t)];
    elib_ft_ringbuf_ctx_t ctx;

    elib_ft_ringbuf_init(&ctx, buf, 8, sizeof(uint32_t));

    /* 写入数据 */
    uint32_t vals[] = {100, 200, 300};
    elib_ft_ringbuf_write(&ctx, vals, 3, 3);

    /* 读取数据 */
    uint32_t out[3] = {0};
    elib_ft_ringbuf_read(&ctx, out, 3, 3);
    /* out = {100, 200, 300} */

    elib_ft_ringbuf_deinit(&ctx);
    return 0;
}
```

### 侵入式链表

```c
#include "elib_ft.h"
#include <stdio.h>

typedef struct {
    int                id;
    elib_ft_list_node_t node;   /* 嵌入链表节点 */
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

    /* 遍历 */
    elib_ft_list_node_t *cursor;
    ELIB_FT_LIST_FOR_EACH(cursor, &list) {
        my_item_t *item = ELIB_FT_LIST_ENTRY(cursor, my_item_t, node);
        printf("id = %d\n", item->id);
    }

    /* 移除节点 */
    elib_ft_list_remove(&list, &b.node);

    return 0;
}
```

## 构建与测试

```bash
gcc -std=c99 -Wall -Wextra -Iinclude -o test_elib_ft test/test_elib_ft.c src/elib_ft_core.c src/elib_ft_ringbuf_core.c src/elib_ft_list_core.c && ./test_elib_ft
```

## 许可证

MIT License
