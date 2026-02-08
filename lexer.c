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
