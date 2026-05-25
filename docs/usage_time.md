# time 模块用法

日历时间与 Unix 世纪秒（自 1970-01-01 00:00:00 UTC 起的秒数）相互转换，零动态分配，无 OS 依赖。

## epoch 类型配置

`ELIB_FT_EPOCH_T` 控制 epoch 秒类型，`elib_ft_epoch_signed_t` 自动配对有符号类型用于内部算术：

| `ELIB_FT_EPOCH_T` | `elib_ft_epoch_signed_t` | 年份范围 | 适用平台 |
|---|---|---|---|
| `uint64_t`（默认） | `int64_t` | 1970 ~ ~5883516 | 32/64 位 MCU |
| `uint32_t` | `int32_t` | 1970 ~ 2106-02-07 06:28:15 | 8/16/32 位 MCU（如 8051） |

32 位平台重定义方式：

```c
#define ELIB_FT_EPOCH_T uint32_t
#include "elib_ft_time.h"
```

## Calendar time structure

```c
#include "elib_ft_time.h"

elib_ft_time_t t = {
    .year   = 2025,
    .month  = 6,
    .day    = 15,
    .hour   = 8,
    .minute = 30,
    .second = 45
};
```

## epoch 互转

```c
ELIB_FT_EPOCH_T epoch = 0;
int ret = elib_ft_time_to_epoch(&t, &epoch);  /* t -> epoch */

/* 反向转换 */
elib_ft_time_t t2;
ret = elib_ft_epoch_to_time(epoch, &t2);      /* epoch -> t */
```

## 星期几

```c
/* 1970-01-01 = Thursday = 4 */
assert(elib_ft_time_wday(0) == 4);

/* 2000-01-01 = Saturday = 6 */
assert(elib_ft_time_wday(946684800ULL) == 6);
```

## 往返测试

```c
elib_ft_time_t t_in = {2025, 6, 15, 8, 30, 45};
ELIB_FT_EPOCH_T epoch = 0;
elib_ft_time_to_epoch(&t_in, &epoch);

elib_ft_time_t t_out;
elib_ft_epoch_to_time(epoch, &t_out);
/* t_out == t_in */
```

## 注意事项

- 所有函数均对 `NULL` 参数做安全处理
- 返回 `ELIB_FT_ERR_INVALID_PARAM` 表示参数越界（年份小于 1970）
- 不考虑闰秒，始终 UTC 时区
- 默认 `ELIB_FT_EPOCH_T = uint64_t`，`elib_ft_epoch_signed_t` 自动配对为 `int64_t`
- 32 位平台：`#define ELIB_FT_EPOCH_T uint32_t`，自动配对 `int32_t`，零 64 位运算
- 年份上限取决于 `elib_ft_epoch_signed_t`：`int32_t` 约 5883516 年，`uint32_t` epoch 限制为 2106 年