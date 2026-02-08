#include "utf8.h"

static int is_count(uint8_t b) {
    return (b & 0xC0u) == 0x80u; // 10xxxxxx
}


Utf8Status utf8_next(const uint8_t* buf, size_t len, size_t* i, uint32_t* out_cp) {
    if (!buf || !i || !out_cp) return UTF8_INVALID;
    if (*i >= len) return UTF8_EOF;

    uint8_t b0 = buf[*i];

    // ASCII: 0xxxxxxx
    if ((b0 & 0x80u) == 0) {
        *out_cp = (uint32_t)b0;
        *i += 1;
        return UTF8_OK;
    }

    // Reject continuation byte as a start byte
    if (is_count(b0)) return UTF8_INVALID;
}