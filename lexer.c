#include "lexer.h"

#include <stdlib.h>
#include <string.h>

/* ----------------------------
   Small helpers (code points)
   ---------------------------- */

static int is_ascii_digit(uint32_t cp) {
    return cp >= (uint32_t)'0' && cp <= (uint32_t)'9';
}

static int is_ascii_alpha(uint32_t cp) {
    return (cp >= (uint32_t)'a' && cp <= (uint32_t)'z') ||
           (cp >= (uint32_t)'A' && cp <= (uint32_t)'Z');
}

static int is_ident_char(uint32_t cp) {
    // Match your CHARACTERS regex intent:
    // [\u200d\u200b\u0D80-\u0DFFa-zA-Z0-9_]+
    if (cp == 0x200D || cp == 0x200B) return 1; // ZWJ/ZWS
    if (cp >= 0x0D80 && cp <= 0x0DFF) return 1; // Sinhala block
    if (is_ascii_alpha(cp)) return 1;
    if (is_ascii_digit(cp)) return 1;
    if (cp == (uint32_t)'_') return 1;
    return 0;
}

static void set_pos(Position* dst, const Position* src) {
    *dst = *src;
}
