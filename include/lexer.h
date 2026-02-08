#ifndef CEYLONICUS_LEXER_H
#define CEYLONICUS_LEXER_H

#include <stddef.h>
#include <stdint.h>

#include "token.h"
#include "utf8.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LexerStatus {
    LEX_OK = 0,
    LEX_EOF,
    LEX_INVALID_UTF8,
    LEX_ILLEGAL_CHAR,
    LEX_UNTERMINATED_STRING,
    LEX_EXPECTED_CHAR
} LexerStatus;

typedef int (*LexerIsKeywordFn)(StrSlice s);

typedef struct Lexer {
    const uint8_t* src;
    size_t len;

    const char* filename; //can be NULL
    size_t i; //current byte index

    Position pos; //current position (byte index + line/column)

    // one-codepoint lookahead cache
    int has_current;
    uint32_t current_cp;

    LexerIsKeywordFn is_keyword; // optional keyword checker

    // last error information
    // (filled when status != LEX_OK/LEX_EOF)

    Position error_pos_start;
    Position error_pos_end;
    uint32_t error_cp; //offending codepoint when relevant
    char expected_ascii; // '=' after '!' for != (0 if not used)

} Lexer;

// Initialize a lexer over a UTF-8 byte buffer
void lexer_init(Lexer* lx, const char* filename, const uint8_t* src, size_t len);

// set/replace keyword matcher (optional)
void lexer_set_keyword_fn(Lexer* lx, LexerIsKeywordFn fn);

/*
Get next token.
    - on LEX_OK; out_tok is valid
    - on LEX_OFF: out_tok will typically be TOK_EOF (implementation choice)
    - on error: out_tok is unspecified; check lx->error_* fields for details.
*/
LexerStatus lexer_next_token(Lexer* lx, Token* out_tok);

#ifdef __cplusplus
}
#endif

#endif // CEYLONICUS_LEXER_H
