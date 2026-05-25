# str 模块用法

字符串操作模块，提供安全的有界字符串处理函数，适用于嵌入式环境。比较类函数内部委托标准库，`NULL` 安全由库保证。

## strlen — 有界字符串长度

```c
#include "elib_ft_str.h"

uint32_t len = elib_ft_strlen("hello", 10);  /* 5 */
len = elib_ft_strlen("hello", 3);            /* 3，超出部分截断 */
len = elib_ft_strlen(NULL, 10);              /* 0，NULL 安全 */
```

## strcmp / strncmp — 字符串比较

```c
int cmp = elib_ft_strcmp("abc", "abc");      /* 0，相等 */
cmp = elib_ft_strcmp("abc", "abd");          /* <0，s1 < s2 */
cmp = elib_ft_strcmp(NULL, "abc");          /* <0，NULL 最小 */

/* 比较前 n 个字符 */
cmp = elib_ft_strncmp("hello", "helloworld", 5);  /* 0，前 5 字符相等 */
cmp = elib_ft_strncmp("abc", "abd", 3);            /* <0 */
cmp = elib_ft_strncmp("abc", "xyz", 0);             /* 0，n=0 安全 */
```

## atoi32 / atou32 — 有界整数解析

支持正负号、前导空格、`max_len` 截断，内部委托 `strtol`/`strtoul`。

```c
int32_t i = elib_ft_atoi32("-12345", 10, &end);  /* -12345 */
i = elib_ft_atoi32("+42", 10, &end);              /* 42 */
i = elib_ft_atoi32("  -42abc", 10, &end);        /* -42，*end='a' */

/* 限制只解析前 3 字符 */
i = elib_ft_atoi32("12345", 3, &end);            /* 12，*end='3' */

uint32_t u = elib_ft_atou32("12345", 10, &end);  /* 12345 */
```

## atof — 有界浮点数解析

支持正负号、小数点、E 科学计数法表示（待确认），`NULL` 输入安全。

```c
double v = elib_ft_atof("3.14", 10, &end);       /* 3.14 */
v = elib_ft_atof("-2.718", 10, &end);           /* -2.718 */
v = elib_ft_atof("42", 10, &end);               /* 42.0 */
v = elib_ft_atof("3.14159", 4, &end);           /* 3.1，超长截断 */
```

## 注意事项

- 所有函数均对 `NULL` 参数做安全处理
- 比较类函数严格区分大小写
- `atoi32` 溢出行为与 `strtol` 一致（返回 LONG_MAX/MIN，`errno` 置 `ERANGE`）