# elib-functiontool

零动态内存分配的嵌入式 C 工具库，纯 C99，无外部依赖。十个独立模块共享一套 `elib_ft_err_t` 错误码。单一公共头 `elib_ft.h`，`-Werror` 一等公民。当前版本 `2.0.0`（`ELIB_FT_VERSION`）。

## 用法文档

| 模块 | 文档 |
|------|------|
| util | [docs/usage_util.md](docs/usage_util.md) |
| mem | [docs/usage_mem.md](docs/usage_mem.md) |
| bitmap | [docs/usage_bitmap.md](docs/usage_bitmap.md) |
| ringbuf | [docs/usage_ringbuf.md](docs/usage_ringbuf.md) |
| list | [docs/usage_list.md](docs/usage_list.md) |
| checksum | [docs/usage_checksum.md](docs/usage_checksum.md) |
| str | [docs/usage_str.md](docs/usage_str.md) |
| endian | [docs/usage_endian.md](docs/usage_endian.md) |
| time | [docs/usage_time.md](docs/usage_time.md) |
| qmath | [docs/usage_qmath.md](docs/usage_qmath.md) |

## 模块列表

唯一公共头：`#include "elib_ft.h"` 引入全部模块（v2 起不再提供子模块头文件）。

```c
#include "elib_ft.h"

/* 计算帧校验 */
assert(elib_ft_crc16_init(&ctx, 0xA001, 0xFFFF, 0x0000, 1) == ELIB_FT_OK);
elib_ft_crc16_update(&ctx, frame, len);
uint16_t crc = elib_ft_crc16_final(&ctx);

/* 解析带界字符串（溢出 -> 0 且 *endptr = NULL） */
int32_t v = elib_ft_atoi32(buf, max_len, &end);

/* Q32 定点: 1.5 * 2.0 = 3.0, 满界安全 */
int32_t a = elib_ft_q32_from_double(1.5, 16);
int32_t b = elib_ft_q32_from_double(2.0, 16);
int32_t c = elib_ft_q32_mul(a, b, 16);
```

## 游戏规则

- **零动态内存**：所有 ctx 用户分配、自包含，库内零全局状态
- **配置 init 一次拒收**：非法配置（NULL / 越界 / size<2 / 非法 n 等）在 init 拒收，非法状态不进运行期；init 一律 `memset` 清零 ctx 后再填配置，数组链满不变量
- **`_s` 变体与 ctx API 返回 `elib_ft_err_t`**；fast 变体走快路径返回值，其未做运行期校验的域在伞头逐一文档钉死（调用方构造保证 / 定义化 fallback）

## 错误码

| 错误码 | 说明 |
|--------|------|
| `ELIB_FT_OK` | 成功 |
| `ELIB_FT_ERR_INVALID_PARAM` | 无效参数（NULL、越界、跨列表节点、非法配置） |
| `ELIB_FT_ERR_NOT_INITIALIZED` | 未初始化调用 |
| `ELIB_FT_ERR_EXCEED_MAX` | 值或范围超上限（qmath 溢出 / time 年界 / 解析溢出） |
| `ELIB_FT_ERR_NO_SPACE` | 目的无空间（ringbuf 满） |
| `ELIB_FT_ERR_NO_DATA` | 无数据可取（空读 / 游离节点） |

## v1 -> v2 迁移对照

| v1 | v2 | 说明 |
|----|----|------|
| 11 个公共子头 + 6 个内部壳头 | 唯一 `elib_ft.h` | 子头删除，`.c` 直连伞头（`-Iinclude`，无相对路径） |
| err 独立头 | 错误码并入伞头 | `elib_ft_err_t` 全库统一 |
| 部分模块无初始化校验 | init 拒收矩阵 | ringbuf `size<2`、list NULL、checksum `init=NULL`、time 全字段、qmath `n` 全域 |
| init 不清 ctx | init memset ctx | ctx 全量不变量 |
| `list` push/insert/remove 返 void | 返 `elib_ft_err_t` | 节点带 `owner`：**跨列表/重复插入拒收**；缓存 `tail` 使 `push_back`/`pop_back` O(1)；`remove` 游离节点返 `NO_DATA` |
| `checksum` ctx 无守门 | initialized 守门 | `update` 未初始化返 `NOT_INITIALIZED`；`final` 自动 `xor_out` |
| `atoi32`/`atou32` 基于 `strtol` | 手工解析 | 不依赖 long 位宽差异；**溢出返 0 且 `*endptr=NULL`**（哨兵） |
| `strcmp_fast` 契约未钉死 | 契约文档化 | 长度序 -> FNV-1a 哈希序 -> memcmp 兜底 |
| time 64 位中间量依赖 | 双配置管线 | `uint32` 配置纯 32 位安全管线（精确到 2106-02-07 06:28:15）；`uint64` 配置年上限 65535 |
| `civil_from_days` 公式符号错 | 修复 | yoe 符号 + era 负端调整（Hinnant 原版） |
| `q64` 无 `__int128` fallback 乘除错值 | 128 位 fallback 重写 | umul/udiv/scale 全量重写，与 `__int128` 路径逐位一致 |
| `from_double` 越界 UB 强转 | 饱和换算 | 钳制到类型极值（含 2^63 / 2^64 双精度上界） |
| fast 变体不校验 `n` | 定义化 fallback | 非法 `n`：mul/div/from_double 返 0；convert/floor/ceil/round 原 q 返回；**不发 UB** |
| `_s` 返回 `int` | 返回 `elib_ft_err_t` | 输出统一走指针 |

## 测试与验证

`test/` 下 11 件套（每模块一个，独立 `main`，`-std=c99 -Wall -Wextra -Werror` 编译关卡）。无宿主编译器的环境下行为验证用 Python 大整数参考实现逐点交叉验证（`scripts` 范式：位精确 C 模型 vs 原生大整数算术，覆盖 128 位乘除 fallback、饱和边界、日历公式、FNV/CRC 标准向量）。

## 目录结构

```
elib-functiontool/
├── include/
│   └── elib_ft.h                 # 唯一公共头（错误码 + 十模块 API）
├── src/
│   ├── elib_ft_mem_core.c        # 有界 memcpy/memset
│   ├── elib_ft_bitmap_core.c     # 位图（置/清/移位/查找，尾位安全）
│   ├── elib_ft_ringbuf_core.c    # cell 环形缓冲区（initialized 守门）
│   ├── elib_ft_list_core.c       # 侵入式双向链表（owner + O(1) tail）
│   ├── elib_ft_checksum_core.c   # Sum8/CRC8/CRC16/CRC32/FNV-1a
│   ├── elib_ft_str_core.c        # 有界字符串 + 手工解析
│   ├── elib_ft_endian_core.c     # 字节交换 / 网络序
│   ├── elib_ft_time_core.c       # 日历 <-> 世纪秒（双配置）
│   └── elib_ft_qmath_core.c      # Qm.n 六族（含 128 位 fallback）
├── test/
│   ├── test_elib_ft_err.c
│   ├── test_elib_ft_util.c
│   ├── test_elib_ft_mem.c
│   ├── test_elib_ft_bitmap.c
│   ├── test_elib_ft_ringbuf.c
│   ├── test_elib_ft_list.c
│   ├── test_elib_ft_checksum.c
│   ├── test_elib_ft_str.c
│   ├── test_elib_ft_endian.c
│   ├── test_elib_ft_time.c
│   └── test_elib_ft_qmath.c
├── docs/
│   └── usage_*.md                # 十篇模块用法
├── LICENSE
└── README.md
```

## 构建

无构建系统，单一编译命令（arm-none-eabi-gcc 或任意 C99 编译器）：

```bash
arm-none-eabi-gcc -std=c99 -Wall -Wextra -Werror -Iinclude -c src/*.c
```

编译单元各自直连 `elib_ft.h`（`-Iinclude`，无相对路径插入），多文件工程按任意组合链接 `.o` 即可。`test/` 同样以 `-Iinclude` 逐文件编译。

## 许可证

MIT License
