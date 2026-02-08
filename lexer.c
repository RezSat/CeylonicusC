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
    // Match the original CHARACTERS regex intent (from python prototype):
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

static void token_init(Token* t, TokenType type, const Position* start, const Position* end) {
    memset(t, 0, sizeof(*t));
    t->type = type;
    t->start = *start;
    t->end = *end;
}

static LexerStatus make_simple_token(Lexer* lx, Token* out, TokenType type) {
    Position start = lx->pos;
    uint32_t cp = 0;
    LexerStatus st = advance_cp(lx, &cp);
    if (st != LEX_OK) return st;
    Position end = lx->pos;
    token_init(out, type, &start, &end);
    return LEX_OK;
}

static LexerStatus skip_comment(Lexer* lx) {
    // current is '#'
    uint32_t cp = 0;
    LexerStatus st = advance_cp(lx, &cp);
    if (st != LEX_OK) return st;

    for (;;) {
        uint32_t p = 0;
        st = peek_cp(lx, &p);
        if (st == LEX_EOF) return LEX_OK;
        if (st != LEX_OK) return st;

        if (p == (uint32_t)'\n') return LEX_OK; // stop before newline
        // consume
        st = advance_cp(lx, &p);
        if (st != LEX_OK && st != LEX_EOF) return st;
    }
}

static LexerStatus lex_number(Lexer* lx, Token* out) {
    Position start = lx->pos;
    size_t start_byte = lx->i;

    int dot_count = 0;

    for (;;) {
        uint32_t cp = 0;
        LexerStatus st = peek_cp(lx, &cp);
        if (st != LEX_OK) break;

        if (cp == (uint32_t)'.') {
            if (dot_count == 1) break;
            dot_count++;
            uint32_t consumed = 0;
            st = advance_cp(lx, &consumed);
            if (st != LEX_OK) return st;
            continue;
        }

        if (!is_ascii_digit(cp)) break;

        uint32_t consumed = 0;
        st = advance_cp(lx, &consumed);
        if (st != LEX_OK) return st;
    }

    size_t end_byte = lx->i;
    size_t n = end_byte - start_byte;

    // Reject lone "." (otherwise strtod would accept it weirdly / fail)
    if (n == 1 && lx->src[start_byte] == (uint8_t)'.') {
        Position end = lx->pos;
        set_error(lx, &start, &end, (uint32_t)'.', 0);
        return LEX_ILLEGAL_CHAR;
    }

    char* tmp = (char*)malloc(n + 1);
    if (!tmp) {
        // treat as illegal for now (TODO: add OUT_OF_MEMORY later)
        Position end = lx->pos;
        set_error(lx, &start, &end, 0, 0);
        return LEX_ILLEGAL_CHAR;
    }
    memcpy(tmp, (const char*)lx->src + start_byte, n);
    tmp[n] = '\0';

    Position end = lx->pos;

    if (dot_count == 0) {
        token_init(out, TOK_INT, &start, &end);
        out->value.i = strtoll(tmp, NULL, 10);
    } else {
        token_init(out, TOK_FLOAT, &start, &end);
        out->value.f = strtod(tmp, NULL);
    }

    free(tmp);
    return LEX_OK;
}

static LexerStatus lex_identifier_or_keyword(Lexer* lx, Token* out) {
    Position start = lx->pos;
    size_t start_byte = lx->i;

    for (;;) {
        uint32_t cp = 0;
        LexerStatus st = peek_cp(lx, &cp);
        if (st != LEX_OK) break;
        if (!is_ident_char(cp)) break;

        uint32_t consumed = 0;
        st = advance_cp(lx, &consumed);
        if (st != LEX_OK) return st;
    }

    size_t end_byte = lx->i;
    Position end = lx->pos;

    StrSlice slice;
    slice.ptr = (const char*)lx->src + start_byte;
    slice.len = end_byte - start_byte;

    TokenType type = TOK_ID;
    if (lx->is_keyword && lx->is_keyword(slice)) type = TOK_KEYWORD;

    token_init(out, type, &start, &end);
    out->value.str = slice;
    return LEX_OK;
}

static LexerStatus lex_string(Lexer* lx, Token* out) {
    // Supports both " and ' like python version.
    Position start = lx->pos;

    uint32_t quote = 0;
    LexerStatus st = advance_cp(lx, &quote); // consume opening quote
    if (st != LEX_OK) return st;

    // Build unescaped string in a growable buffer
    size_t cap = 32;
    size_t len = 0;
    char* buf = (char*)malloc(cap);
    if (!buf) {
        Position end = lx->pos;
        set_error(lx, &start, &end, 0, 0);
        return LEX_ILLEGAL_CHAR;
    }

    int escaping = 0;

    for (;;) {
        uint32_t cp = 0;
        st = peek_cp(lx, &cp);

        if (st == LEX_EOF) {
            Position end = lx->pos;
            set_error(lx, &start, &end, 0, 0);
            free(buf);
            return LEX_UNTERMINATED_STRING;
        }
        if (st != LEX_OK) {
            free(buf);
            return st;
        }

        if (!escaping && cp == quote) {
            // consume closing quote
            uint32_t consumed = 0;
            st = advance_cp(lx, &consumed);
            if (st != LEX_OK) { free(buf); return st; }
            break;
        }

        // consume cp
        uint32_t consumed = 0;
        st = advance_cp(lx, &consumed);
        if (st != LEX_OK) { free(buf); return st; }

        if (!escaping && consumed == (uint32_t)'\\') {
            escaping = 1;
            continue;
        }

        uint32_t out_ch = consumed;
        if (escaping) {
            if (consumed == (uint32_t)'n') out_ch = (uint32_t)'\n';
            else if (consumed == (uint32_t)'t') out_ch = (uint32_t)'\t';
            // else: keep as-is
            escaping = 0;
        }

        // Store as UTF-8 bytes. For now: only special escapes become ASCII,
        // and otherwise we keep original codepoints by re-encoding minimal UTF-8.
        // Since consumed is a Unicode scalar value (from utf8_next), we can encode it.

        // Ensure capacity for up to 4 bytes
        if (len + 4 >= cap) {
            cap *= 2;
            char* nb = (char*)realloc(buf, cap);
            if (!nb) {
                Position end = lx->pos;
                set_error(lx, &start, &end, 0, 0);
                free(buf);
                return LEX_ILLEGAL_CHAR;
            }
            buf = nb;
        }

        if (out_ch <= 0x7F) {
            buf[len++] = (char)out_ch;
        } else if (out_ch <= 0x7FF) {
            buf[len++] = (char)(0xC0 | ((out_ch >> 6) & 0x1F));
            buf[len++] = (char)(0x80 | (out_ch & 0x3F));
        } else if (out_ch <= 0xFFFF) {
            buf[len++] = (char)(0xE0 | ((out_ch >> 12) & 0x0F));
            buf[len++] = (char)(0x80 | ((out_ch >> 6) & 0x3F));
            buf[len++] = (char)(0x80 | (out_ch & 0x3F));
        } else {
            buf[len++] = (char)(0xF0 | ((out_ch >> 18) & 0x07));
            buf[len++] = (char)(0x80 | ((out_ch >> 12) & 0x3F));
            buf[len++] = (char)(0x80 | ((out_ch >> 6) & 0x3F));
            buf[len++] = (char)(0x80 | (out_ch & 0x3F));
        }
    }

    // Null-terminate for convenience (not counted in slice.len)
    if (len + 1 >= cap) {
        char* nb = (char*)realloc(buf, len + 1);
        if (!nb) {
            Position end = lx->pos;
            set_error(lx, &start, &end, 0, 0);
            free(buf);
            return LEX_ILLEGAL_CHAR;
        }
        buf = nb;
    }
    buf[len] = '\0';

    Position end = lx->pos;
    token_init(out, TOK_STRING, &start, &end);
    out->value.str.ptr = buf; // heap-owned
    out->value.str.len = len;

    return LEX_OK;
}

/* ----------------------------
   Public API
   ---------------------------- */

void lexer_init(Lexer* lx, const char* filename, const uint8_t* src, size_t len) {
    memset(lx, 0, sizeof(*lx));
    lx->src = src;
    lx->len = len;
    lx->filename = filename;
    lx->i = 0;

    lx->pos.index = 0;
    lx->pos.line = 0;
    lx->pos.column = 0;

    lx->has_current = 0;
    lx->current_cp = 0;

    lx->is_keyword = NULL;

    lx->error_pos_start = lx->pos;
    lx->error_pos_end = lx->pos;
    lx->error_cp = 0;
    lx->expected_ascii = 0;
}

void lexer_set_keyword_fn(Lexer* lx, LexerIsKeywordFn fn) {
    if (lx) lx->is_keyword = fn;
}

LexerStatus lexer_next_token(Lexer* lx, Token* out_tok) {
    if (!lx || !out_tok) return LEX_ILLEGAL_CHAR;

    // Clear previous error info
    lx->error_pos_start = lx->pos;
    lx->error_pos_end = lx->pos;
    lx->error_cp = 0;
    lx->expected_ascii = 0;

    for (;;) {
        uint32_t cp = 0;
        LexerStatus st = peek_cp(lx, &cp);

        if (st == LEX_EOF) {
            // Emit EOF token
            token_init(out_tok, TOK_EOF, &lx->pos, &lx->pos);
            return LEX_EOF;
        }

        if (st != LEX_OK) {
            set_error(lx, &lx->pos, &lx->pos, 0, 0);
            return st;
        }

        // Skip spaces and tabs
        if (cp == (uint32_t)' ' || cp == (uint32_t)'\t' || cp == (uint32_t)'\r') {
            uint32_t consumed = 0;
            st = advance_cp(lx, &consumed);
            if (st != LEX_OK) return st;
            continue;
        }

        // Comment
        if (cp == (uint32_t)'#') {
            st = skip_comment(lx);
            if (st != LEX_OK) return st;
            continue;
        }

        // NEWLINE: ';' or '\n'
        if (cp == (uint32_t)';' || cp == (uint32_t)'\n') {
            return make_simple_token(lx, out_tok, TOK_NEWLINE);
        }

        // Number: digit OR '.' followed by digit
        if (is_ascii_digit(cp) || cp == (uint32_t)'.') {
            if (cp == (uint32_t)'.') {
                // lookahead: '.' must be followed by digit to start a number
                size_t tmp_i = lx->i;
                uint32_t c0 = 0;
                Utf8Status us0 = utf8_next(lx->src, lx->len, &tmp_i, &c0);
                (void)us0;
                uint32_t c1 = 0;
                Utf8Status us1 = utf8_next(lx->src, lx->len, &tmp_i, &c1);
                if (us1 != UTF8_OK || !is_ascii_digit(c1)) {
                    Position start = lx->pos;
                    // consume '.' so we don't get stuck
                    uint32_t consumed = 0;
                    advance_cp(lx, &consumed);
                    Position end = lx->pos;
                    set_error(lx, &start, &end, (uint32_t)'.', 0);
                    return LEX_ILLEGAL_CHAR;
                }
            }
            return lex_number(lx, out_tok);
        }

        // Identifier / Keyword
        if (is_ident_char(cp)) {
            return lex_identifier_or_keyword(lx, out_tok);
        }

        // String
        if (cp == (uint32_t)'"' || cp == (uint32_t)'\'' ) {
            return lex_string(lx, out_tok);
        }
        
        // Operators / punctuation
        switch (cp) {

    }
}