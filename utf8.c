#include "utf8.h"

static int is_cont(uint8_t b) {
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
    if (is_cont(b0)) return UTF8_INVALID;

    // Determine sequence length (with minimal validation ranges)

    uint32_t cp = 0;
    size_t need = 0;

    if ((b0 & 0xE0u) == 0xC0u) {
        // 110xxxxx => 2 bytes
        need = 2;
        cp = (uint32_t)(b0 & 0x1Fu);
    } else if ((b0 & 0xF0u) == 0xE0u) {
        // 1110xxxx => 3 bytes
        need = 3;
        cp = (uint32_t)(b0 & 0x0Fu);
    } else if ((b0 & 0xF8u) == 0xF0u) {
        // 11110xxx => 4 bytes
        need = 4;
        cp = (uint32_t)(b0 & 0x07u);
    } else {
        // 0xF5..0xFF re invalid in UTF-8
        // also 0xF8.. are not used
        return UTF8_INVALID;
    }

    if (*i + need > len) return UTF8_INVALID;

    // Read continuation bytes
    for (size_t k = 1; k < need; k++) {
        uint8_t bx = buf[*i + k];
        if (!is_cont(bx)) return UTF8_INVALID;
        cp = (cp << 6) | (uint32_t)(bx & 0x3Fu);
    }

    // Overlong checks and unicode range checks
    // These ensure the shortest valid enncoding is used.

    if (need == 2 && cp < 0x80u) return UTF8_INVALID;
    if (need == 3 && cp < 0x800u) return UTF8_INVALID;
    if (need == 4 && cp < 0x10000u) return UTF8_INVALID;

    // Reject surrogates (U+D800..U+DFFF)
    if (cp >= 0xD800u && cp <= 0xDFFFu) return UTF8_INVALID;

    // Reject out of range (> U+10FFFF)
    if (cp > 0x10FFFFu) return UTF8_INVALID;

    *out_cp = cp;
    *i += need;
    return UTF8_OK;

}