# mem 模块用法

头文件：`elib_ft_mem.h`（通过 `elib_ft.h` 自动引入）

---

## elib_ft_memcpy

带最大长度限制的内存拷贝，超出 `max_len` 截断，返回实际拷贝字节数。

```c
uint8_t src[] = {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t dst[8] = {0};

/* 正常拷贝 */
uint32_t n = elib_ft_memcpy(dst, src, 5, 10);
/* n = 5, dst = {1,2,3,4,5,0,0,0} */

/* 截断拷贝：len > max_len */
n = elib_ft_memcpy(dst, src, 8, 3);
/* n = 3, dst = {1,2,3,...} 仅拷贝前 3 字节 */

/* NULL 安全 */
n = elib_ft_memcpy(NULL, src, 5, 10);
/* n = 0，不会崩溃 */
```

---

## elib_ft_memset

带最大长度限制的内存填充，超出 `max_len` 截断，返回实际填充字节数。

```c
uint8_t buf[8] = {0};

uint32_t n = elib_ft_memset(buf, 0xAA, 6, 10);
/* n = 6, buf 前 6 字节均为 0xAA */

/* 截断 */
n = elib_ft_memset(buf, 0xFF, 8, 3);
/* n = 3, 仅填充前 3 字节 */
```
