# ringbuf 模块用法

头文件：`elib_ft_ringbuf.h`（通过 `elib_ft.h` 自动引入）

基于 cell 的环形缓冲区，零动态分配，支持 `max_len` 安全限制。

---

## 初始化

用户分配缓冲区和上下文，调用 init 完成初始化。

```c
#include "elib_ft.h"

uint8_t buf[8 * sizeof(uint32_t)];     /* 用户分配缓冲区 */
elib_ft_ringbuf_ctx_t ctx;              /* 用户分配上下文 */

elib_ft_ringbuf_init(&ctx, buf, 8, sizeof(uint32_t));
/* 参数：ctx      上下文指针
 *       buf      缓冲区指针
 *       8        容量（cell 数）
 *       sizeof   每个 cell 的字节数
 *
 * 有效容量 = size - 1 = 7 个 cell（环形缓冲区保留一个空位）
 */
```

### 结构体 cell

cell 大小不限于基本类型，可以是任意结构体：

```c
typedef struct {
    uint16_t id;
    uint8_t  data[4];
} msg_t;

uint8_t buf[10 * sizeof(msg_t)];
elib_ft_ringbuf_ctx_t ctx;

elib_ft_ringbuf_init(&ctx, buf, 10, sizeof(msg_t));
```

---

## 写入 / 读取

### 基本用法

```c
uint32_t vals[] = {100, 200, 300};

/* 写入 3 个 cell，max_len = 3 */
elib_ft_err_t err = elib_ft_ringbuf_write(&ctx, vals, 3, 3);
/* err == ELIB_FT_OK */

uint32_t out[3] = {0};
err = elib_ft_ringbuf_read(&ctx, out, 3, 3);
/* out = {100, 200, 300} */
```

### max_len 安全限制

`len > max_len` 时返回 `ELIB_FT_ERR_EXCEED_MAX`，不会写入任何数据：

```c
uint32_t val = 42;
err = elib_ft_ringbuf_write(&ctx, &val, 2, 1);
/* err == ELIB_FT_ERR_EXCEED_MAX，未写入 */
```

### 缓冲区满

写入超过空闲容量时返回 `ELIB_FT_ERR_NO_SPACE`：

```c
uint32_t vals[8] = {0};
err = elib_ft_ringbuf_write(&ctx, vals, 8, 8);
/* err == ELIB_FT_ERR_NO_SPACE，有效容量仅 7 */
```

### 缓冲区空

无数据可读时返回 `ELIB_FT_ERR_NO_DATA`：

```c
uint32_t out = 0;
err = elib_ft_ringbuf_read(&ctx, &out, 1, 1);
/* err == ELIB_FT_ERR_NO_DATA */
```

---

## 查询状态

```c
uint32_t used = elib_ft_ringbuf_used(&ctx);  /* 已用 cell 数 */
uint32_t free = elib_ft_ringbuf_free(&ctx);  /* 空闲 cell 数 */
```

---

## 重置 / 反初始化

```c
/* 重置：丢弃所有数据，缓冲区可重新使用 */
elib_ft_ringbuf_reset(&ctx);

/* 反初始化：标记为未初始化，后续操作返回 ERR_NOT_INITIALIZED */
elib_ft_ringbuf_deinit(&ctx);
```

---

## 环绕测试

环形缓冲区首尾相连，写入和读取后会自动环绕：

```c
uint8_t buf[4 * sizeof(uint32_t)];
elib_ft_ringbuf_ctx_t ctx;
elib_ft_ringbuf_init(&ctx, buf, 4, sizeof(uint32_t));

/* 写满 3 个 cell（有效容量 = 4 - 1 = 3） */
uint32_t in1[3] = {10, 20, 30};
elib_ft_ringbuf_write(&ctx, in1, 3, 3);

/* 读出 3 个 cell */
uint32_t out1[3] = {0};
elib_ft_ringbuf_read(&ctx, out1, 3, 3);

/* 再写入 3 个 cell → 环绕 */
uint32_t in2[3] = {40, 50, 60};
elib_ft_ringbuf_write(&ctx, in2, 3, 3);

uint32_t out2[3] = {0};
elib_ft_ringbuf_read(&ctx, out2, 3, 3);
/* out2 = {40, 50, 60} */
```
