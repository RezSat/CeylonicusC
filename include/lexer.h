#ifndef CEYLONICUS_LEXER_H
#define CEYLONICUS_LEXER_H

#include "token.h"

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



#ifdef __cplusplus
}
#endif

#endif // CEYLONICUS_LEXER_H
