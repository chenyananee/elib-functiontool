# checksum 模块用法

头文件：`elib_ft_checksum.h`（通过 `elib_ft.h` 自动引入）

校验算法支持多帧计算：`init` → `update`(可多次) → `final`，也提供一次性计算函数。

> `final` 自动对结果执行 `crc ^ xor_out`，`xor_out` 在 `init` 时指定。无需手动异或。

---

## Sum8 — 和校验

累加所有字节，取低 8 位。

### 一次性计算

```c
const uint8_t data[] = {0x01, 0x02, 0x03};
uint8_t sum = elib_ft_sum8(data, 3);
/* sum = 0x06 */
```

### 多帧计算

```c
elib_ft_sum8_ctx_t ctx;
elib_ft_sum8_init(&ctx);

/* 第一帧 */
elib_ft_sum8_update(&ctx, frame1, frame1_len);
/* 第二帧 */
elib_ft_sum8_update(&ctx, frame2, frame2_len);

uint8_t sum = elib_ft_sum8_final(&ctx);
```

---

## CRC8

通用 CRC8 计算，支持自定义多项式、初始值、输出异或值和位序。

### 参数说明

| 参数 | 说明 |
|------|------|
| `poly` | 生成多项式（8 位） |
| `init_val` | 初始值 |
| `xor_out` | 最终结果异或值（不需要异或时传 0） |
| `ref` | 0=MSB-first，1=LSB-first（反射输入输出） |

### 预定义快捷初始化

| 宏 | 算法 | poly | init | xor_out | ref |
|----|------|------|------|---------|-----|
| `ELIB_FT_CRC8_CCITT_INIT(ctx)` | CRC8-CCITT | 0x07 | 0x00 | 0x00 | 0 |
| `ELIB_FT_CRC8_MAXIM_INIT(ctx)` | CRC8-MAXIM | 0x8C | 0x00 | 0x00 | 1 |

### 一次性计算

```c
const uint8_t data[] = "123456789";

/* CRC8-CCITT */
uint8_t crc = elib_ft_crc8(data, 9, 0x07, 0x00, 0x00, 0);
/* crc = 0xF4 */
```

### 多帧计算

```c
elib_ft_crc8_ctx_t ctx;
ELIB_FT_CRC8_CCITT_INIT(&ctx);

elib_ft_crc8_update(&ctx, frame1, frame1_len);
elib_ft_crc8_update(&ctx, frame2, frame2_len);

uint8_t crc = elib_ft_crc8_final(&ctx);
```

---

## CRC16

通用 CRC16 计算，支持自定义多项式、初始值、输出异或值和位序。

### 预定义快捷初始化

| 宏 | 算法 | poly | init | xor_out | ref |
|----|------|------|------|---------|-----|
| `ELIB_FT_CRC16_XMODEM_INIT(ctx)` | CRC16-XMODEM | 0x1021 | 0x0000 | 0x0000 | 0 |
| `ELIB_FT_CRC16_CCITT_INIT(ctx)` | CRC16-CCITT | 0x1021 | 0xFFFF | 0x0000 | 0 |
| `ELIB_FT_CRC16_MODBUS_INIT(ctx)` | CRC16-MODBUS | 0xA001 | 0xFFFF | 0x0000 | 1 |

### 一次性计算

```c
const uint8_t data[] = "123456789";

/* CRC16-XMODEM */
uint16_t crc = elib_ft_crc16(data, 9, 0x1021, 0x0000, 0x0000, 0);
/* crc = 0x31C3 */

/* CRC16-MODBUS */
crc = elib_ft_crc16(data, 9, 0xA001, 0xFFFF, 0x0000, 1);
/* crc = 0x4B37 */
```

### 多帧计算

```c
elib_ft_crc16_ctx_t ctx;
ELIB_FT_CRC16_MODBUS_INIT(&ctx);

elib_ft_crc16_update(&ctx, frame1, frame1_len);
elib_ft_crc16_update(&ctx, frame2, frame2_len);

uint16_t crc = elib_ft_crc16_final(&ctx);
```

### 典型应用：Modbus RTU

Modbus RTU 协议中，CRC16 低字节在前：

```c
elib_ft_crc16_ctx_t ctx;
ELIB_FT_CRC16_MODBUS_INIT(&ctx);
elib_ft_crc16_update(&ctx, pdu, pdu_len);
uint16_t crc = elib_ft_crc16_final(&ctx);

/* 发送时低字节在前 */
tx_buf[len]     = (uint8_t)(crc & 0xFF);       /* CRC_L */
tx_buf[len + 1] = (uint8_t)((crc >> 8) & 0xFF); /* CRC_H */
```

---

## CRC32

通用 CRC32 计算，支持自定义多项式、初始值、输出异或值和位序。

### 预定义快捷初始化

| 宏 | 算法 | poly | init | xor_out | ref |
|----|------|------|------|---------|-----|
| `ELIB_FT_CRC32_ETHERNET_INIT(ctx)` | CRC32-Ethernet | 0xEDB88320 | 0xFFFFFFFF | 0xFFFFFFFF | 1 |

> `final` 自动执行 `crc ^ xor_out`，CRC32-Ethernet 的 `xor_out=0xFFFFFFFF` 已内置在快捷宏中，无需手动异或。

### 一次性计算

```c
const uint8_t data[] = "123456789";

/* CRC32-Ethernet */
uint32_t crc = elib_ft_crc32(data, 9, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1);
/* crc = 0xCBF43926 */
```

### 多帧计算

```c
elib_ft_crc32_ctx_t ctx;
ELIB_FT_CRC32_ETHERNET_INIT(&ctx);

elib_ft_crc32_update(&ctx, frame1, frame1_len);
elib_ft_crc32_update(&ctx, frame2, frame2_len);

uint32_t crc = elib_ft_crc32_final(&ctx);
/* 自动完成 xor_out，无需手动异或 */
```

### MSB-first 多项式

库同时支持 MSB-first 多项式（`ref=0`），等效结果相同：

```c
/* MSB-first: poly=0x04C11DB7, 等效于 LSB-first 0xEDB88320 */
elib_ft_crc32_ctx_t ctx;
elib_ft_crc32_init(&ctx, 0x04C11DB7u, 0xFFFFFFFFu, 0xFFFFFFFFu, 0);
elib_ft_crc32_update(&ctx, data, len);
uint32_t crc = elib_ft_crc32_final(&ctx);
/* crc = 0xCBF43926，结果一致 */
```

---

## xor_out 说明

`xor_out` 是对**最终结果**的异或值，在 `final` 中自动执行 `crc ^ xor_out`。与 `init_val` 作用不同：

- `init_val`：影响寄存器起始值（处理数据前）
- `xor_out`：影响最终输出值（处理完所有数据后）

两者不可互相替代。例如 CRC32-Ethernet 中 `init=0xFFFFFFFF` 保证前导零参与计算，`xor_out=0xFFFFFFFF` 保证全零输入的结果不为零。

不需要异或的算法传 0 即可：`xor_out` 为 0 时 `crc ^ 0 = crc`，无额外开销。

---

## 常见 CRC 算法参数速查

| 算法 | 宽度 | poly | init | xor_out | ref | "123456789" |
|------|------|------|------|---------|-----|-------------|
| CRC8-CCITT | 8 | 0x07 | 0x00 | 0x00 | 0 | 0xF4 |
| CRC8-MAXIM | 8 | 0x8C | 0x00 | 0x00 | 1 | 0xA1 |
| CRC16-XMODEM | 16 | 0x1021 | 0x0000 | 0x0000 | 0 | 0x31C3 |
| CRC16-CCITT | 16 | 0x1021 | 0xFFFF | 0x0000 | 0 | 0x29B1 |
| CRC16-MODBUS | 16 | 0xA001 | 0xFFFF | 0x0000 | 1 | 0x4B37 |
| CRC32-Ethernet | 32 | 0xEDB88320 | 0xFFFFFFFF | 0xFFFFFFFF | 1 | 0xCBF43926 |
