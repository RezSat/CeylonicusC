#ifndef CEYLONICUS_LEXER_H
#define CEYLONICUS_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LEX_OK = 0,
    LEX_EOF,
    LEX_INVALID_UTF8,
    LEX_ILLEGAL_CHAR,
    LEX_UNTERMINATED_STRING,
    LEX_EXPECTED_CHAR
} LexerStatus;


#ifdef __cplusplus
}
#endif

#endif // CEYLONICUS_LEXER_H
