# str 模块用法

字符串操作模块，提供安全的有界字符串处理函数，适用于嵌入式环境。

## strlen — 带长度限制的字符串长度

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
cmp = elib_ft_strcmp("abd", "abc");          /* >0，s1 > s2 */
cmp = elib_ft_strcmp(NULL, "abc");           /* <0，NULL 最小 */

/* 比较前 n 个字符 */
cmp = elib_ft_strncmp("hello", "helloworld", 5);  /* 0，前 5 字符相等 */
cmp = elib_ft_strncmp("abc", "abd", 3);           /* <0 */
cmp = elib_ft_strncmp("abc", "xyz", 0);           /* 0，n=0 安全 */
```

## atoi — 字符串转无符号整数

```c
const char *end;
uint32_t val = elib_ft_atoi("12345", 10, &end);  /* 12345，*end='\0' */
val = elib_ft_atoi("12345", 3, &end);             /* 12，限制只解析前 3 字符 */
val = elib_ft_atoi("  42abc", 10, &end);          /* 42，跳过前导空格 */
val = elib_ft_atoi("abc", 10, &end);              /* 0，无效输入 */

/* endptr 可以为 NULL */
val = elib_ft_atoi("999", 10, NULL);              /* 999 */
```

## 注意事项

- 所有函数均对 `NULL` 参数做安全处理
- `strcmp` / `strncmp` 严格区分大小写
- `atoi` 只识别十进制数字，无溢出保护（`uint32_t` 范围）