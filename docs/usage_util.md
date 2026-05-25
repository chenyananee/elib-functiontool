# util 模块用法

通用快捷宏集合，零动态分配，纯宏实现，无 OS 依赖。

## MIN / MAX — 大小比较

```c
int a = ELIB_FT_MIN(3, 7);    /* a = 3 */
int b = ELIB_FT_MAX(3, 7);    /* b = 7 */
int c = ELIB_FT_MIN(-1, 2);   /* c = -1 */
```

## CLAMP — 限幅

将值限制在 [lo, hi] 范围内：

```c
int v = ELIB_FT_CLAMP(5, 0, 10);    /* v = 5  (范围内) */
int v = ELIB_FT_CLAMP(-1, 0, 10);   /* v = 0  (低于下限) */
int v = ELIB_FT_CLAMP(15, 0, 10);   /* v = 10 (高于上限) */
```

## MAP — 整数线性映射

将值从 [in_lo, in_hi] 映射到 [out_lo, out_hi]，四舍五入：

```c
/* 0-100 → 0-1000 */
int v = ELIB_FT_MAP(50, 0, 100, 0, 1000);   /* v = 500 */

/* 0-100 → 200-300 */
int v = ELIB_FT_MAP(50, 0, 100, 200, 300);  /* v = 250 */

/* 反向映射 0-100 → 1000-0 */
int v = ELIB_FT_MAP(0, 0, 100, 1000, 0);    /* v = 1000 */
```

## MAP_F — 浮点线性映射

```c
/* ADC 0-4095 → 电压 0.0-3.3V */
double v = ELIB_FT_MAP_F(2048, 0, 4095, 0.0, 3.3);  /* v ≈ 1.65 */
```

## 其他宏

| 宏 | 说明 |
|----|------|
| `ELIB_FT_ARRAY_SIZE(arr)` | 计算数组元素个数 |
| `ELIB_FT_WEAK` | 编译器弱符号修饰（ICCARM/CC_ARM/GNUC 自适应） |

## 注意事项

- 所有宏参数仅求值一次时不保证安全（标准 C 宏限制），避免 `ELIB_FT_MIN(i++, j)` 等写法
- `ELIB_FT_MAP` 使用整数运算，四舍五入避免截断偏差
- `ELIB_FT_MAP_F` 使用 `double` 精度，需要浮点支持
- `in_hi == in_lo` 时除零行为未定义，调用者应确保输入范围非零
