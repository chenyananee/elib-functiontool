# endian 模块用法

字节序转换模块，提供 16 位和 32 位整数的字节交换和网络序/主机序转换，适用于网络通信和跨平台数据交换。

## swap — 字节交换

```c
#include "elib_ft_endian.h"

uint16_t hi = elib_ft_swap16(0x1234);  /* 0x3412 */
uint32_t hi32 = elib_ft_swap32(0x12345678);  /* 0x78563412 */
```

## hton / ntoh — 网络序/主机序转换

网络序（大端序）与主机序相互转换。在大端主机上 `hton` 为空操作；在小端主机上执行字节交换。

```c
/* 16 位 */
uint16_t net16 = elib_ft_hton16(0x1234);  /* 转网络序 */
uint16_t host16 = elib_ft_ntoh16(net16);  /* 转主机序 */

/* 32 位 */
uint32_t net32 = elib_ft_hton32(0x12345678);  /* 转网络序 */
uint32_t host32 = elib_ft_ntoh32(net32);       /* 转主机序 */
```

## 注意事项

- `hton` 和 `ntoh` 对 16 位和 32 位分别互为逆操作
- 内部使用 `__BYTE_ORDER__` 宏判断主机字节序，若编译器不支持该宏则按小端处理