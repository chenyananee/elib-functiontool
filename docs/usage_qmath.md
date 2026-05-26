# qmath 模块用法

头文件：`elib_ft_qmath.h`（通过 `elib_ft.h` 自动引入）

Qm.n 定点数运算换算库，支持 16/32/64 位宽、有符号/无符号共六套接口。

---

## 基本概念

Qm.n 格式中，`m` 为整数位数，`n` 为小数位数。底层为整型运算，`n` 运行时传入，`m` 自动推导：

- 有符号：m = 位宽 - n - 1（1 位符号位）
- 无符号：m = 位宽 - n

| 接口前缀 | 底层类型 | 范围（n=16） | 符号 |
|----------|----------|-------------|------|
| `q16` | `int16_t` | -2.0 ~ 1.999... | 有 |
| `uq16` | `uint16_t` | 0 ~ 63.999... | 无 |
| `q32` | `int32_t` | -32768.0 ~ 32767.999... | 有 |
| `uq32` | `uint32_t` | 0 ~ 65535.999... | 无 |
| `q64` | `int64_t` | ±2^47 量级 | 有 |
| `uq64` | `uint64_t` | 0 ~ 2^63 量级 | 无 |

---

## 浮点换算

```c
/* double → Q32 */
int32_t q = elib_ft_q32_from_double(1.5, 16);   /* q = 98304 (1.5 * 65536) */

/* Q32 → double */
double v = elib_ft_q32_to_double(q, 16);          /* v = 1.5 */

/* float 接口同理 */
int32_t qf = elib_ft_q32_from_float(1.5f, 16);
float  vf  = elib_ft_q32_to_float(qf, 16);
```

---

## 四则运算

### 快速版（无溢出检测）

```c
int32_t a = elib_ft_q32_from_double(2.0, 16);
int32_t b = elib_ft_q32_from_double(3.0, 16);

int32_t r_add = elib_ft_q32_add(a, b);            /* 5.0 */
int32_t r_sub = elib_ft_q32_sub(a, b);            /* -1.0 */
int32_t r_mul = elib_ft_q32_mul(a, b, 16);        /* 6.0 */
int32_t r_div = elib_ft_q32_div(b, a, 16);        /* 1.5 */
```

> `add`/`sub` 不需要 `n` 参数（同 Q 格式直接加减）。

### 安全版（溢出检测）

```c
int32_t out;
int ret = elib_ft_q32_add_s(INT32_MAX, 1, &out);
/* ret == ELIB_FT_ERR_EXCEED_MAX */

ret = elib_ft_q32_div_s(100, 0, 16, &out);
/* ret == ELIB_FT_ERR_INVALID_PARAM (除零) */

ret = elib_ft_q32_mul_s(a, b, 16, &out);
if (ret == ELIB_FT_OK) {
    /* 使用 out */
}
```

---

## Q 格式转换

```c
/* Q8 → Q12（增加精度） */
int32_t q8 = 1 << 8;       /* 1.0 in Q8 */
int32_t q12 = elib_ft_q32_convert(q8, 8, 12);
/* q12 = 1 << 12 = 4096 */

/* Q12 → Q8（降低精度） */
int32_t back = elib_ft_q32_convert(q12, 12, 8);
/* back = 1 << 8 = 256 */
```

---

## 取整与绝对值

```c
int32_t q = elib_ft_q32_from_double(1.75, 16);

elib_ft_q32_floor(q, 16);   /* 1.0 */
elib_ft_q32_ceil(q, 16);    /* 2.0 */
elib_ft_q32_round(q, 16);   /* 2.0 */

elib_ft_q32_abs(-q);         /* |q| */
```

> 负数 `floor` 向负无穷方向：`-1.25 → -2.0`

---

## 其他位宽

Q16/UQ16/Q64/UQ64 接口完全对称，仅前缀和类型不同：

```c
/* 16 位 */
int16_t q = elib_ft_q16_from_double(0.5, 14);
int16_t r = elib_ft_q16_mul(q, q, 14);

/* 64 位 */
int64_t q = elib_ft_q64_from_double(1.5, 16);
int64_t r = elib_ft_q64_div(q, elib_ft_q64_from_double(2.0, 16), 16);
```

---

## 注意事项

- 快速版溢出行为遵循 C 语言自然溢出环绕，不检测
- 安全版返回 `ELIB_FT_ERR_EXCEED_MAX`（溢出）或 `ELIB_FT_ERR_INVALID_PARAM`（参数无效/除零）
- 64 位乘法在 GCC/Clang 上使用 `__int128` 加速，其他编译器使用分步实现
- `n` 范围：有符号 [0, 位宽-2]，无符号 [0, 位宽-1]
