# bitmap 模块用法

头文件：`elib_ft_mem.h`（通过 `elib_ft.h` 自动引入）

位图基于用户提供的 `uint8_t` 数组，无需初始化，通过 `max_bits` 约束有效位范围。

---

## 置位 / 清位 / 取值

```c
uint8_t buf[4] = {0};  /* 32 位位图 */

elib_ft_bitmap_set(buf, 32, 5);          /* 置位 bit 5 */
elib_ft_bitmap_get(buf, 32, 5);          /* 返回 1 */
elib_ft_bitmap_clear(buf, 32, 5);        /* 清位 bit 5 */
elib_ft_bitmap_get(buf, 32, 5);          /* 返回 0 */

/* 越界安全 */
elib_ft_bitmap_set(buf, 32, 32);         /* 返回 ELIB_FT_ERR_INVALID_PARAM */
elib_ft_bitmap_get(buf, 32, 32);         /* 返回 0 */
```

---

## 全部置位 / 全部清零

```c
uint8_t buf[2] = {0};

elib_ft_bitmap_set_all(buf, 10);
/* 10 位: buf[0] = 0xFF, buf[1] = 0x03 */
/* 末尾字节自动清理多余位 */

elib_ft_bitmap_clear_all(buf, 10);
/* buf[0] = 0x00, buf[1] = 0x00 */
```

---

## 左移 / 右移

移位方向：左移 = 向高位方向移动，高位溢出丢弃，低位补 0。

```c
uint8_t buf[2] = {0x01, 0x00};  /* bit 0 置位 */

elib_ft_bitmap_shift_left(buf, 16, 4);
/* bit 0 → bit 4: buf = {0x10, 0x00} */

elib_ft_bitmap_shift_right(buf, 16, 4);
/* bit 4 → bit 0: buf = {0x01, 0x00} */

/* 跨字节移位 */
uint8_t buf2[2] = {0x00, 0x01};  /* bit 8 置位 */
elib_ft_bitmap_shift_right(buf2, 16, 8);
/* bit 8 → bit 0: buf2 = {0x01, 0x00} */

/* 移位量 >= max_bits 时全部清零 */
elib_ft_bitmap_shift_left(buf, 16, 16);
/* buf = {0x00, 0x00} */
```

---

## 查找最近的 1 / 0

从 `start` 位起向后查找，返回位索引，未找到返回 `max_bits`。

```c
uint8_t buf[2] = {0x24, 0x02};  /* bit 2,5,9 置位 */

/* 查找最近的 1 */
elib_ft_bitmap_find_first_set(buf, 16, 0);   /* 返回 2 */
elib_ft_bitmap_find_first_set(buf, 16, 3);   /* 返回 5 */
elib_ft_bitmap_find_first_set(buf, 16, 6);   /* 返回 9 */
elib_ft_bitmap_find_first_set(buf, 16, 10);  /* 返回 16（未找到） */

/* 查找最近的 0 */
uint8_t full[1] = {0xFF};
elib_ft_bitmap_find_first_clear(full, 8, 0);  /* 返回 8（未找到） */

/* 典型用法：分配第一个空闲位 */
uint32_t bit = elib_ft_bitmap_find_first_clear(buf, 16, 0);
if (bit < 16) {
    elib_ft_bitmap_set(buf, 16, bit);
    /* 使用 bit ... */
}
```
