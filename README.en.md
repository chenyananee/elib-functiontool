[简体中文](README.md) | **English**

# elib-functiontool

An embedded C utility library with zero dynamic allocation, pure C99, no external dependencies. Ten independent modules share one `elib_ft_err_t` error code set. A single public header, `elib_ft.h`, and `-Werror` as a first-class citizen. Current version `2.0.0` (`ELIB_FT_VERSION`).

## Usage Docs

| Module | Documentation |
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

## Modules

Single public header: `#include "elib_ft.h"` pulls in every module (since v2 there are no per-module headers).

```c
#include "elib_ft.h"

/* compute the frame checksum */
assert(elib_ft_crc16_init(&ctx, 0xA001, 0xFFFF, 0x0000, 1) == ELIB_FT_OK);
elib_ft_crc16_update(&ctx, frame, len);
uint16_t crc = elib_ft_crc16_final(&ctx);

/* parse a bounded string (overflow -> 0 and *endptr = NULL) */
int32_t v = elib_ft_atoi32(buf, max_len, &end);

/* Q32 fixed point: 1.5 * 2.0 = 3.0, full-range safe */
int32_t a = elib_ft_q32_from_double(1.5, 16);
int32_t b = elib_ft_q32_from_double(2.0, 16);
int32_t c = elib_ft_q32_mul(a, b, 16);
```

## Ground Rules

- **Zero dynamic memory**: every ctx is user-allocated and self-contained, with zero global state in the library
- **Configurations rejected once, at init**: illegal configurations (NULL / out of range / size<2 / illegal n, etc.) are rejected at init, so no illegal state reaches run time; init always `memset`s the ctx to zero before filling in the configuration, keeping the array/list fullness invariants
- **The `_s` variants and the ctx API return `elib_ft_err_t`**; the fast variants take the fast path and return a value, and every domain they leave unvalidated at run time is pinned down line by line in the umbrella header (caller-construction guarantees / defined fallback)

## Error Codes

| Error code | Description |
|--------|------|
| `ELIB_FT_OK` | Success |
| `ELIB_FT_ERR_INVALID_PARAM` | Invalid parameter (NULL, out of range, node from another list, illegal configuration) |
| `ELIB_FT_ERR_NOT_INITIALIZED` | Called before initialization |
| `ELIB_FT_ERR_EXCEED_MAX` | Value or range above the limit (qmath overflow / time year bound / parse overflow) |
| `ELIB_FT_ERR_NO_SPACE` | No space at the destination (ringbuf full) |
| `ELIB_FT_ERR_NO_DATA` | No data available (read on empty / detached node) |

## v1 -> v2 Migration Mapping

| v1 | v2 | Notes |
|----|----|------|
| 11 public sub-headers + 6 internal shell headers | A single `elib_ft.h` | Sub-headers deleted; each `.c` includes the umbrella header directly (`-Iinclude`, no relative paths) |
| Standalone err header | Error codes merged into the umbrella header | One `elib_ft_err_t` across the whole library |
| Some modules had no init validation | init rejection matrix | ringbuf `size<2`, list NULL, checksum `init=NULL`, all time fields, the whole qmath `n` domain |
| init did not clear the ctx | init memsets the ctx | Full-ctx invariants |
| `list` push/insert/remove returned void | Returns `elib_ft_err_t` | Nodes carry an `owner`: **cross-list / duplicate insertion is rejected**; a cached `tail` makes `push_back`/`pop_back` O(1); `remove` on a detached node returns `NO_DATA` |
| `checksum` ctx had no guard | initialized guard | `update` before initialization returns `NOT_INITIALIZED`; `final` applies `xor_out` automatically |
| `atoi32`/`atou32` based on `strtol` | Hand-written parsing | No dependence on `long` width differences; **overflow returns 0 with `*endptr=NULL`** (sentinel) |
| `strcmp_fast` contract not pinned down | Contract documented | length order -> FNV-1a hash order -> memcmp as the fallback |
| time depended on 64-bit intermediates | Two configuration pipelines | The `uint32` configuration is a pure 32-bit safe pipeline (exact through 2106-02-07 06:28:15); the `uint64` configuration has a year ceiling of 65535 |
| `civil_from_days` had sign errors in the formula | Fixed | yoe sign + negative-era adjustment (Hinnant's original) |
| `q64` multiply/divide produced wrong values without `__int128` | 128-bit fallback rewritten | umul/udiv/scale fully rewritten, bit-for-bit identical to the `__int128` path |
| `from_double` cast out of range with UB | Saturating conversion | Clamped to the type extremes (including the 2^63 / 2^64 double-precision bounds) |
| fast variants did not validate `n` | Defined fallback | For an illegal `n`: mul/div/from_double return 0; convert/floor/ceil/round return the original q; **no UB is triggered** |
| `_s` returned `int` | Returns `elib_ft_err_t` | All output goes through pointers |

## Testing

`test/` holds 11 files (one per module, each with its own `main`, gated by a `-std=c99 -Wall -Wextra -Werror` compile). Where no host compiler is available, behavior is verified point by point against a Python big-integer reference implementation (the `scripts` pattern: a bit-exact C model vs. native big-integer arithmetic, covering the 128-bit multiply/divide fallback, saturation boundaries, the calendar formulas, and the FNV/CRC standard vectors).

## Directory Layout

```
elib-functiontool/
├── include/
│   └── elib_ft.h                 # the only public header (error codes + ten-module API)
├── src/
│   ├── elib_ft_mem_core.c        # bounded memcpy/memset
│   ├── elib_ft_bitmap_core.c     # bitmap (set/clear/shift/find, tail-bit safe)
│   ├── elib_ft_ringbuf_core.c    # cell ring buffer (initialized guard)
│   ├── elib_ft_list_core.c       # intrusive doubly linked list (owner + O(1) tail)
│   ├── elib_ft_checksum_core.c   # Sum8/CRC8/CRC16/CRC32/FNV-1a
│   ├── elib_ft_str_core.c        # bounded strings + hand-written parsing
│   ├── elib_ft_endian_core.c     # byte swap / network order
│   ├── elib_ft_time_core.c       # calendar <-> Unix epoch seconds (two configurations)
│   └── elib_ft_qmath_core.c      # Qm.n six families (with the 128-bit fallback)
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
│   └── usage_*.md                # ten module usage docs
├── LICENSE
└── README.md
```

## Building

No build system, just one compile command (`arm-none-eabi-gcc` or any C99 compiler):

```bash
arm-none-eabi-gcc -std=c99 -Wall -Wextra -Werror -Iinclude -c src/*.c
```

Each translation unit includes `elib_ft.h` directly (`-Iinclude`, no relative-path insertion), so a multi-file project can link any combination of the `.o` files. `test/` is likewise compiled file by file with `-Iinclude`.

## License

MIT License
