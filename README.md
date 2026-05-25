# elib-functiontool

零动态内存分配的嵌入式 C 工具库，纯 C99，无外部依赖。

## 用法文档

| 模块 | 文档 |
|------|------|
| mem | [docs/usage_mem.md](docs/usage_mem.md) |
| ringbuf | [docs/usage_ringbuf.md](docs/usage_ringbuf.md) |
| list | [docs/usage_list.md](docs/usage_list.md) |

## 模块列表

| 模块 | 头文件 | 说明 |
|------|--------|------|
| mem | `elib_ft_mem.h` | 内存拷贝/填充/位图操作/数组宏 |
| ringbuf | `elib_ft_ringbuf.h` | 基于 cell 的环形缓冲区 |
| list | `elib_ft_list.h` | 侵入式双向链表 |
| err | `elib_ft_err.h` | 统一错误码 |
| 兼容宏 | `elib_ft.h` | `ELIB_FT_WEAK` / `ELIB_FT_ARRAY_SIZE` 等 |

用户只需 `#include "elib_ft.h"` 即可引入全部模块，也可单独引用子模块头文件。

## 功能列表

### mem — 内存操作

| 函数 / 宏 | 说明 |
|-----------|------|
| `ELIB_FT_ARRAY_SIZE(arr)` | 计算数组元素个数 |
| `elib_ft_memcpy(dst, src, len, max_len)` | 内存拷贝，超出 max_len 截断，返回实际字节数 |
| `elib_ft_memset(dst, val, len, max_len)` | 内存填充，超出 max_len 截断，返回实际字节数 |
| `elib_ft_bitmap_set(buf, max_bits, bit)` | 置位 |
| `elib_ft_bitmap_clear(buf, max_bits, bit)` | 清位 |
| `elib_ft_bitmap_get(buf, max_bits, bit)` | 获取位值（0 或 1） |
| `elib_ft_bitmap_set_all(buf, max_bits)` | 全部置位 |
| `elib_ft_bitmap_clear_all(buf, max_bits)` | 全部清零 |
| `elib_ft_bitmap_shift_left(buf, max_bits, n)` | 左移 n 位（高位丢弃） |
| `elib_ft_bitmap_shift_right(buf, max_bits, n)` | 右移 n 位（低位丢弃） |
| `elib_ft_bitmap_find_first_set(buf, max_bits, start)` | 查找最近的 1 |
| `elib_ft_bitmap_find_first_clear(buf, max_bits, start)` | 查找最近的 0 |

### ringbuf — 环形缓冲区

| 函数 | 说明 |
|------|------|
| `elib_ft_ringbuf_init(ctx, buf, size, cell_size)` | 初始化 |
| `elib_ft_ringbuf_deinit(ctx)` | 反初始化 |
| `elib_ft_ringbuf_used(ctx)` | 已用 cell 数 |
| `elib_ft_ringbuf_free(ctx)` | 空闲 cell 数 |
| `elib_ft_ringbuf_write(ctx, data, len, max_len)` | 写入 |
| `elib_ft_ringbuf_read(ctx, data, len, max_len)` | 读取 |
| `elib_ft_ringbuf_reset(ctx)` | 重置 |

### list — 侵入式双向链表

| 函数 / 宏 | 说明 |
|-----------|------|
| `elib_ft_list_init(list)` | 初始化 |
| `elib_ft_list_empty(list)` | 判空 |
| `elib_ft_list_count(list)` | 节点数 |
| `elib_ft_list_push_front(list, node)` | 头部插入 |
| `elib_ft_list_push_back(list, node)` | 尾部插入 |
| `elib_ft_list_pop_front(list)` | 弹出首节点 |
| `elib_ft_list_pop_back(list)` | 弹出尾节点 |
| `elib_ft_list_remove(list, node)` | 移除节点 |
| `elib_ft_list_insert_after(list, after, node)` | 在 after 后插入 |
| `elib_ft_list_insert_before(list, before, node)` | 在 before 前插入 |
| `ELIB_FT_LIST_ENTRY(node, type, member)` | 获取宿主结构体指针 |
| `ELIB_FT_LIST_FOR_EACH(cursor, list)` | 遍历 |
| `ELIB_FT_LIST_FOR_EACH_SAFE(cursor, tmp, list)` | 安全遍历 |

### 错误码

| 错误码 | 说明 |
|--------|------|
| `ELIB_FT_OK` | 成功 |
| `ELIB_FT_ERR_INVALID_PARAM` | 无效参数 |
| `ELIB_FT_ERR_NOT_INITIALIZED` | 未初始化 |
| `ELIB_FT_ERR_EXCEED_MAX` | 超过 max_len |
| `ELIB_FT_ERR_NO_SPACE` | 缓冲区空间不足 |
| `ELIB_FT_ERR_NO_DATA` | 无数据可读 |

## 目录结构

```
elib-functiontool/
├── include/
│   ├── elib_ft.h                  # 伞形头文件
│   ├── elib_ft_err.h              # 错误码
│   ├── elib_ft_mem.h              # 内存操作
│   ├── elib_ft_ringbuf.h          # 环形缓冲区
│   └── elib_ft_list.h             # 侵入式链表
├── src/
│   ├── elib_ft_core.c             # mem 实现
│   ├── elib_ft_ringbuf_core.c     # ringbuf 实现
│   └── elib_ft_list_core.c        # list 实现
├── test/
│   └── test_elib_ft.c             # 单元测试
├── docs/
│   ├── usage_mem.md               # mem 用法
│   ├── usage_ringbuf.md           # ringbuf 用法
│   └── usage_list.md              # list 用法
├── scripts/
├── LICENSE
└── README.md
```

## 构建与测试

```bash
gcc -std=c99 -Wall -Wextra -Iinclude -o test_elib_ft test/test_elib_ft.c src/elib_ft_core.c src/elib_ft_ringbuf_core.c src/elib_ft_list_core.c && ./test_elib_ft
```

## 许可证

MIT License
