# bit 模块用法

位操作模块，提供 popcount（Population Count）计算和位图查找功能，适用于资源受限的嵌入式环境。

## popcount — 统计 1 的个数

使用位运算并行算法（Harley-Seal 技法），无循环、无查表，单指令流完成。

```c
#include "elib_ft_bit.h"

uint32_t n = elib_ft_popcount8(0xFF);    /* 8 */
n = elib_ft_popcount8(0x55);             /* 4 */
n = elib_ft_popcount8(0x00);             /* 0 */

uint32_t n16 = elib_ft_popcount16(0xFFFF);  /* 16 */
uint32_t n32 = elib_ft_popcount32(0xFFFFFFFFU);  /* 32 */
```

## find_next — 查找下一个 1/0 位

在位图中查找从 `start`（不含）之后最近的 1 位或 0 位位置。使用字节跳过 + ctz8 二分查找算法，复杂度 O(bytes)。

```c
/* 位图: bit2=1, bit5=1 */
uint8_t buf[1] = {0x24};  /* 0b00100100 */

uint32_t pos = elib_ft_bit_find_next_set(buf, 8, 0);   /* 2，从头开始找 */
pos = elib_ft_bit_find_next_set(buf, 8, 3);            /* 5，跳过 bit3 继续找 */
pos = elib_ft_bit_find_next_set(buf, 8, 6);            /* 8，没找到返回 max_bits */

/* 查找 0 位 */
uint8_t buf2[1] = {0xDB};  /* 0b11011011, bit2=0, bit5=0 */
pos = elib_ft_bit_find_next_clear(buf2, 8, 0);   /* 2 */
pos = elib_ft_bit_find_next_clear(buf2, 8, 3);  /* 5 */

/* 跨字节查找（bit9=1） */
uint8_t buf3[2] = {0x00, 0x02};
pos = elib_ft_bit_find_next_set(buf3, 16, 0);   /* 9 */
```

## 注意事项

- `find_next` 若未找到目标位，返回 `max_bits`
- 查找范围受 `max_bits` 限制，不受数组实际大小限制
- NULL 缓冲返回 `max_bits`