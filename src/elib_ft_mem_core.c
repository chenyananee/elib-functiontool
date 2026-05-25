/* elib_ft_mem_core.c - FunctionTool Memcpy/Memset Implementation */

#include "elib_ft_mem_core.h"
#include <string.h>

uint32_t elib_ft_memcpy(void *dst, const void *src, uint32_t len, uint32_t max_len)
{
    if (dst == NULL || src == NULL) {
        return 0;
    }

    uint32_t actual = (len > max_len) ? max_len : len;
    memcpy(dst, src, (size_t)actual);

    return actual;
}

uint32_t elib_ft_memset(void *dst, uint8_t val, uint32_t len, uint32_t max_len)
{
    if (dst == NULL) {
        return 0;
    }

    uint32_t actual = (len > max_len) ? max_len : len;
    memset(dst, (int)val, (size_t)actual);

    return actual;
}