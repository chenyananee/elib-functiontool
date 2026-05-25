# time 模块用法

日历时间与 Unix 世纪秒（自 1970-01-01 00:00:00 UTC 起的秒数）相互转换，零动态分配，无 OS 依赖。支持年份范围 **1970-2038**，纯 32 位实现（适用于 8051 等 8/16/32 位平台）。

## Calendar time structure

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
uint64_t epoch = 0;
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
uint64_t epoch = 0;
elib_ft_time_to_epoch(&t_in, &epoch);

elib_ft_time_t t_out;
elib_ft_epoch_to_time(epoch, &t_out);
/* t_out == t_in */
```

## 注意事项

- 所有函数均对 `NULL` 参数做安全处理
- 返回 `ELIB_FT_ERR_INVALID_PARAM` 表示参数越界（年份超出 1970-2038）
- 不考虑闰秒，始终 UTC 时区
- epoch 使用 `uint32_t`，可表示到 **2106-02-07 06:28:15**（32 位溢出前）