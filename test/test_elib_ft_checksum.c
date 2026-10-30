/* test_elib_ft_checksum.c - FunctionTool Checksum / Hash Unit Tests (v2)
 *
 * Split from the v1 single-file suite; migrated to the v2 umbrella
 * header (elib_ft.h, ELIB_FT_VERSION "2.0.0").
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "elib_ft.h"

#define RUN_TEST(fn) do { fn(); printf("  PASS: %s\n", #fn); } while (0)

/* "123456789" - standard CRC test vector */
static const uint8_t chk_test_data[] = {
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};

static void test_sum8_basic(void)
{
    elib_ft_sum8_ctx_t ctx;
    elib_ft_sum8_init(&ctx);
    elib_ft_sum8_update(&ctx, chk_test_data, 9);
    uint8_t r = elib_ft_sum8_final(&ctx);
    /* 0x31+0x32+...+0x39 = 477 = 0x1DD, mod 256 = 0xDD */
    assert(r == 0xDD);
}

static void test_sum8_multiframe(void)
{
    elib_ft_sum8_ctx_t ctx;
    elib_ft_sum8_init(&ctx);
    elib_ft_sum8_update(&ctx, chk_test_data, 4);
    elib_ft_sum8_update(&ctx, chk_test_data + 4, 5);
    uint8_t r = elib_ft_sum8_final(&ctx);
    assert(r == 0xDD);
}

static void test_sum8_oneshot(void)
{
    assert(elib_ft_sum8(chk_test_data, 9) == 0xDD);
}

static void test_sum8_null(void)
{
    elib_ft_sum8_init(NULL);
    elib_ft_sum8_update(NULL, chk_test_data, 9);
    elib_ft_sum8_update((elib_ft_sum8_ctx_t *)1, NULL, 9);
    assert(elib_ft_sum8_final(NULL) == 0);
    assert(elib_ft_sum8(NULL, 9) == 0);
    assert(elib_ft_sum8(chk_test_data, 0) == 0);
}

static void test_crc8_ccitt(void)
{
    /* CRC8-CCITT: poly=0x07, init=0x00, ref=0 鈫?"123456789" = 0xF4 */
    elib_ft_crc8_ctx_t ctx;
    ELIB_FT_CRC8_CCITT_INIT(&ctx);
    elib_ft_crc8_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc8_final(&ctx) == 0xF4);
}

static void test_crc8_multiframe(void)
{
    elib_ft_crc8_ctx_t ctx;
    ELIB_FT_CRC8_CCITT_INIT(&ctx);
    elib_ft_crc8_update(&ctx, chk_test_data, 4);
    elib_ft_crc8_update(&ctx, chk_test_data + 4, 5);
    assert(elib_ft_crc8_final(&ctx) == 0xF4);
}

static void test_crc8_oneshot(void)
{
    assert(elib_ft_crc8(chk_test_data, 9, 0x07, 0x00, 0x00, 0) == 0xF4);
}

static void test_crc8_null(void)
{
    elib_ft_crc8_init(NULL, 0x07, 0x00, 0x00, 0);
    elib_ft_crc8_update(NULL, chk_test_data, 9);
    elib_ft_crc8_ctx_t ctx;
    elib_ft_crc8_update(&ctx, NULL, 9);
    assert(elib_ft_crc8_final(NULL) == 0);
    assert(elib_ft_crc8(NULL, 9, 0x07, 0x00, 0x00, 0) == 0);
}

static void test_crc16_xmodem(void)
{
    /* CRC16-XMODEM: poly=0x1021, init=0x0000, ref=0 鈫?"123456789" = 0x31C3 */
    elib_ft_crc16_ctx_t ctx;
    ELIB_FT_CRC16_XMODEM_INIT(&ctx);
    elib_ft_crc16_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc16_final(&ctx) == 0x31C3);
}

static void test_crc16_modbus(void)
{
    /* CRC16-MODBUS: poly=0xA001, init=0xFFFF, ref=1 鈫?"123456789" = 0x4B37 */
    elib_ft_crc16_ctx_t ctx;
    ELIB_FT_CRC16_MODBUS_INIT(&ctx);
    elib_ft_crc16_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc16_final(&ctx) == 0x4B37);
}

static void test_crc16_multiframe(void)
{
    elib_ft_crc16_ctx_t ctx;
    ELIB_FT_CRC16_XMODEM_INIT(&ctx);
    elib_ft_crc16_update(&ctx, chk_test_data, 4);
    elib_ft_crc16_update(&ctx, chk_test_data + 4, 5);
    assert(elib_ft_crc16_final(&ctx) == 0x31C3);
}

static void test_crc16_oneshot(void)
{
    assert(elib_ft_crc16(chk_test_data, 9, 0x1021, 0x0000, 0x0000, 0) == 0x31C3);
    assert(elib_ft_crc16(chk_test_data, 9, 0xA001, 0xFFFF, 0x0000, 1) == 0x4B37);
}

static void test_crc16_null(void)
{
    elib_ft_crc16_init(NULL, 0x1021, 0x0000, 0x0000, 0);
    elib_ft_crc16_update(NULL, chk_test_data, 9);
    elib_ft_crc16_ctx_t ctx;
    elib_ft_crc16_update(&ctx, NULL, 9);
    assert(elib_ft_crc16_final(NULL) == 0);
    assert(elib_ft_crc16(NULL, 9, 0x1021, 0x0000, 0x0000, 0) == 0);
}

static void test_crc32_ethernet(void)
{
    /* CRC32-Ethernet: poly=0xEDB88320, init=0xFFFFFFFF, xor_out=0xFFFFFFFF, ref=1
     * "123456789" 鈫?0xCBF43926 */
    elib_ft_crc32_ctx_t ctx;
    ELIB_FT_CRC32_ETHERNET_INIT(&ctx);
    elib_ft_crc32_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc32_final(&ctx) == 0xCBF43926u);
}

static void test_crc32_multiframe(void)
{
    elib_ft_crc32_ctx_t ctx;
    ELIB_FT_CRC32_ETHERNET_INIT(&ctx);
    elib_ft_crc32_update(&ctx, chk_test_data, 4);
    elib_ft_crc32_update(&ctx, chk_test_data + 4, 5);
    assert(elib_ft_crc32_final(&ctx) == 0xCBF43926u);
}

static void test_crc32_oneshot(void)
{
    assert(elib_ft_crc32(chk_test_data, 9, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1) == 0xCBF43926u);
}

static void test_crc32_null(void)
{
    elib_ft_crc32_init(NULL, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1);
    elib_ft_crc32_update(NULL, chk_test_data, 9);
    elib_ft_crc32_ctx_t ctx;
    elib_ft_crc32_update(&ctx, NULL, 9);
    assert(elib_ft_crc32_final(NULL) == 0);
    assert(elib_ft_crc32(NULL, 9, 0xEDB88320u, 0xFFFFFFFFu, 0xFFFFFFFFu, 1) == 0);
}

static void test_crc32_msb_first(void)
{
    /* CRC32-ISO-HDLC MSB-first: poly=0x04C11DB7, init=0xFFFFFFFF, xor_out=0xFFFFFFFF, ref=0
     * "123456789" 鈫?0xCBF43926 */
    elib_ft_crc32_ctx_t ctx;
    elib_ft_crc32_init(&ctx, 0x04C11DB7u, 0xFFFFFFFFu, 0xFFFFFFFFu, 0);
    elib_ft_crc32_update(&ctx, chk_test_data, 9);
    assert(elib_ft_crc32_final(&ctx) == 0xCBF43926u);
}

static void test_fnv1a32_basic(void)
{
    /* FNV-1a test vector: empty string */
    assert(elib_ft_hash_fnv1a32((const uint8_t *)"", 0) == 2166136261u);
    /* "hello" */
    assert(elib_ft_hash_fnv1a32((const uint8_t *)"hello", 5) == 0xF9DDEC5Fu);
}

static void test_fnv1a32_null(void)
{
    assert(elib_ft_hash_fnv1a32(NULL, 0) == 2166136261u);
    assert(elib_ft_hash_fnv1a32(NULL, 10) == 2166136261u);
}

static void test_fnv1a64_basic(void)
{
    assert(elib_ft_hash_fnv1a64((const uint8_t *)"", 0) == 14695981039346656037ULL);
    /* "hello" */
    assert(elib_ft_hash_fnv1a64((const uint8_t *)"hello", 5) == 0xA9B44F1F6C77E781u);
}

static void test_fnv1a64_null(void)
{
    assert(elib_ft_hash_fnv1a64(NULL, 0) == 14695981039346656037ULL);
}

static void test_checksum_uninitialized_ctx(void)
{
    /* zeroed ctx must refuse update with NOT_INITIALIZED */
    elib_ft_sum8_ctx_t s;
    elib_ft_crc8_ctx_t c8;
    elib_ft_crc16_ctx_t c16;
    elib_ft_crc32_ctx_t c32;
    memset(&s, 0, sizeof(s));
    memset(&c8, 0, sizeof(c8));
    memset(&c16, 0, sizeof(c16));
    memset(&c32, 0, sizeof(c32));

    assert(elib_ft_sum8_update(&s, chk_test_data, 9)
           == ELIB_FT_ERR_NOT_INITIALIZED);
    assert(elib_ft_crc8_update(&c8, chk_test_data, 9)
           == ELIB_FT_ERR_NOT_INITIALIZED);
    assert(elib_ft_crc16_update(&c16, chk_test_data, 9)
           == ELIB_FT_ERR_NOT_INITIALIZED);
    assert(elib_ft_crc32_update(&c32, chk_test_data, 9)
           == ELIB_FT_ERR_NOT_INITIALIZED);
    assert(elib_ft_sum8_final(&s) == 0);
    assert(elib_ft_crc8_final(&c8) == 0);

    /* a proper init makes the very same context usable */
    assert(elib_ft_sum8_init(&s) == ELIB_FT_OK);
    assert(elib_ft_sum8_update(&s, chk_test_data, 9) == ELIB_FT_OK);
    assert(elib_ft_sum8_final(&s) == 0xDD);
}

static void test_checksum_xor_out_final(void)
{
    /* xor_out is configured once at init and applied by final() */
    assert(elib_ft_crc8(chk_test_data, 9, 0x07, 0x00, 0x00, 0) == 0xF4);
    assert(elib_ft_crc8(chk_test_data, 9, 0x07, 0x00, 0xA5, 0)
           == (uint8_t)(0xF4 ^ 0xA5));
}

int main(void)
{
    RUN_TEST(test_sum8_basic);
    RUN_TEST(test_sum8_multiframe);
    RUN_TEST(test_sum8_oneshot);
    RUN_TEST(test_sum8_null);
    RUN_TEST(test_crc8_ccitt);
    RUN_TEST(test_crc8_multiframe);
    RUN_TEST(test_crc8_oneshot);
    RUN_TEST(test_crc8_null);
    RUN_TEST(test_crc16_xmodem);
    RUN_TEST(test_crc16_modbus);
    RUN_TEST(test_crc16_multiframe);
    RUN_TEST(test_crc16_oneshot);
    RUN_TEST(test_crc16_null);
    RUN_TEST(test_crc32_ethernet);
    RUN_TEST(test_crc32_multiframe);
    RUN_TEST(test_crc32_oneshot);
    RUN_TEST(test_crc32_null);
    RUN_TEST(test_crc32_msb_first);
    RUN_TEST(test_fnv1a32_basic);
    RUN_TEST(test_fnv1a32_null);
    RUN_TEST(test_fnv1a64_basic);
    RUN_TEST(test_fnv1a64_null);

    RUN_TEST(test_checksum_uninitialized_ctx);
    RUN_TEST(test_checksum_xor_out_final);
    printf("\nAll %d tests passed.\n", 24);
    return 0;
}
