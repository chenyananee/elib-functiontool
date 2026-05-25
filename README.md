# elib-functiontool

零动态内存分配的嵌入式 C 工具库，纯 C99，无外部依赖。

## 用法文档

| 模块 | 文档 |
|------|------|
| mem | [docs/usage_mem.md](docs/usage_mem.md) |
| ringbuf | [docs/usage_ringbuf.md](docs/usage_ringbuf.md) |
| list | [docs/usage_list.md](docs/usage_list.md) |
| checksum | [docs/usage_checksum.md](docs/usage_checksum.md) |
| str | [docs/usage_str.md](docs/usage_str.md) |
| endian | [docs/usage_endian.md](docs/usage_endian.md) |

## 模块列表

| 模块 | 头文件 | 说明 |
|------|--------|------|
| mem | `elib_ft_mem.h` | 内存拷贝/填充/位图操作/数组宏 |
| ringbuf | `elib_ft_ringbuf.h` | 基于 cell 的环形缓冲区 |
| list | `elib_ft_list.h` | 侵入式双向链表 |
| checksum | `elib_ft_checksum.h` | Sum8/CRC8/CRC16/CRC32 校验算法 |
| str | `elib_ft_str.h` | 字符串操作 |
| endian | `elib_ft_endian.h` | 字节序转换 |
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

### checksum — 校验算法

> `init` 时指定 `xor_out`，`final` 自动完成异或，无需手动处理。

| 函数 / 宏 | 说明 |
|-----------|------|
| `elib_ft_sum8_init/update/final` | 和校验（多帧） |
| `elib_ft_sum8(data, len)` | 和校验（单次） |
| `elib_ft_crc8_init/update/final` | CRC8（多帧） |
| `elib_ft_crc8(data, len, poly, init, ref)` | CRC8（单次） |
| `ELIB_FT_CRC8_CCITT_INIT(ctx)` | CRC8-CCITT 快捷初始化 |
| `ELIB_FT_CRC8_MAXIM_INIT(ctx)` | CRC8-MAXIM 快捷初始化 |
| `elib_ft_crc16_init/update/final` | CRC16（多帧） |
| `elib_ft_crc16(data, len, poly, init, ref)` | CRC16（单次） |
| `ELIB_FT_CRC16_XMODEM_INIT(ctx)` | CRC16-XMODEM 快捷初始化 |
| `ELIB_FT_CRC16_CCITT_INIT(ctx)` | CRC16-CCITT 快捷初始化 |
| `ELIB_FT_CRC16_MODBUS_INIT(ctx)` | CRC16-MODBUS 快捷初始化 |
| `elib_ft_crc32_init/update/final` | CRC32（多帧） |
| `elib_ft_crc32(data, len, poly, init, ref)` | CRC32（单次） |
| `ELIB_FT_CRC32_ETHERNET_INIT(ctx)` | CRC32-Ethernet 快捷初始化 |

### str — 字符串操作

| 函数 | 说明 |
|------|------|
| `elib_ft_strlen(s, max_len)` | 获取字符串长度（受 max_len 限制） |
| `elib_ft_strcmp(s1, s2)` | 比较两个字符串 |
| `elib_ft_strncmp(s1, s2, n)` | 比较前 n 个字符 |
| `elib_ft_atoi(s, max_len, endptr)` | 字符串转无符号整数 |

### endian — 字节序转换

| 函数 | 说明 |
|------|------|
| `elib_ft_swap16(val)` | 交换 16 位高低字节 |
| `elib_ft_swap32(val)` | 交换 32 位字节序 |
| `elib_ft_hton16(val)` | 主机序转网络序（16 位） |
| `elib_ft_ntoh16(val)` | 网络序转主机序（16 位） |
| `elib_ft_hton32(val)` | 主机序转网络序（32 位） |
| `elib_ft_ntoh32(val)` | 网络序转主机序（32 位） |

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
│   ├── elib_ft_list.h             # 侵入式链表
│   ├── elib_ft_checksum.h         # 校验算法
│   ├── elib_ft_str.h              # 字符串操作
│   ├── elib_ft_endian.h            # 字节序转换
│   └── elib_ft_err.h               # 错误码
├── src/
│   ├── elib_ft_mem_core.c            # mem 实现
│   ├── elib_ft_bitmap_core.c         # bitmap 实现
│   ├── elib_ft_ringbuf_core.c     # ringbuf 实现
│   ├── elib_ft_list_core.c        # list 实现
│   ├── elib_ft_checksum_core.c    # checksum 实现
│   ├── elib_ft_str_core.c         # str 实现
│   ├── elib_ft_endian_core.c       # endian 实现
├── test/
│   └── test_elib_ft.c             # 单元测试
├── docs/
│   ├── usage_mem.md               # mem 用法
│   ├── usage_ringbuf.md           # ringbuf 用法
│   ├── usage_list.md              # list 用法
│   ├── usage_checksum.md         # checksum 用法
│   ├── usage_str.md              # str 用法
│   ├── usage_endian.md           # endian 用法
├── scripts/
├── LICENSE
└── README.md
```

## 构建与测试

```bash
gcc -std=c99 -Wall -Wextra -Iinclude -o test_elib_ft test/test_elib_ft.c \
  src/elib_ft_mem_core.c src/elib_ft_bitmap_core.c src/elib_ft_ringbuf_core.c \
  src/elib_ft_list_core.c src/elib_ft_checksum_core.c src/elib_ft_str_core.c \
  src/elib_ft_endian_core.c && ./test_elib_ft
```

## 许可证

MIT License
