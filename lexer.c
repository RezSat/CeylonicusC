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

static void set_error(Lexer* lx, const Position* start, const Position* end,
                      uint32_t cp, char expected_ascii) {
    if (!lx) return;
    if (start) set_pos(&lx->error_pos_start, start);
    if (end)   set_pos(&lx->error_pos_end, end);
    lx->error_cp = cp;
    lx->expected_ascii = expected_ascii;
}

/* ----------------------------
   UTF-8 peek/advance
   ---------------------------- */

static LexerStatus status_from_utf8(Utf8Status s) {
    switch (s) {
        case UTF8_OK: return LEX_OK;
        case UTF8_EOF: return LEX_EOF;
        case UTF8_INVALID: return LEX_INVALID_UTF8;
        default: return LEX_INVALID_UTF8;
    }
}

static LexerStatus peek_cp(Lexer* lx, uint32_t* out_cp) {
    if (!lx || !out_cp) return LEX_INVALID_UTF8;

    if (lx->has_current) {
        *out_cp = lx->current_cp;
        return LEX_OK;
    }

    size_t tmp_i = lx->i;
    uint32_t cp = 0;
    Utf8Status us = utf8_next(lx->src, lx->len, &tmp_i, &cp);
    LexerStatus ls = status_from_utf8(us);
    if (ls != LEX_OK) return ls;

    lx->has_current = 1;
    lx->current_cp = cp;
    *out_cp = cp;
    return LEX_OK;
}

static LexerStatus advance_cp(Lexer* lx, uint32_t* out_cp) {
    if (!lx || !out_cp) return LEX_INVALID_UTF8;

    // If we already peeked, consume from the real stream.
    uint32_t cp = 0;
    size_t old_i = lx->i;

    if (lx->has_current) {
        // We must re-decode to know how many bytes to advance.
        // (We could cache byte length too, but this is simplest and still fast.)
        size_t tmp_i = lx->i;
        Utf8Status us = utf8_next(lx->src, lx->len, &tmp_i, &cp);
        LexerStatus ls = status_from_utf8(us);
        if (ls != LEX_OK) return ls;

        lx->i = tmp_i;
        lx->has_current = 0;
    } else {
        Utf8Status us = utf8_next(lx->src, lx->len, &lx->i, &cp);
        LexerStatus ls = status_from_utf8(us);
        if (ls != LEX_OK) return ls;
    }

    // Update position: index is byte index. Column is codepoint column.
    lx->pos.index = lx->i;

    if (cp == (uint32_t)'\n') {
        lx->pos.line += 1;
        lx->pos.column = 0;
    } else {
        // If old_i == lx->i (shouldn't happen), still treat as one column step.
        lx->pos.column += 1;
    }

    *out_cp = cp;
    return LEX_OK;
}

/* ----------------------------
   Token builders
   ---------------------------- */
