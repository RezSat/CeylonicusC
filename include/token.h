/*
    Author: RezSat <yehanwasura@duck.com>
*/


#ifndef CEYLONICUS_TOKEN_H
#define CEYLONICUS_TOKEN_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
    In the prototype (Ceylonicus) I have added it so that python code can be run,
    but that is only added for fun because it was entirely written in python anyway,
    this time I will not implement such a feature because I have no idea
    how that would be integrated here (at least not at the time I am writing the lexer)
*/

typedef enum {
    TOK_INT,
    TOK_FLOAT,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_EOF,
    TOK_ID,
    TOK_KEYWORD,
    TOK_POWER,
    TOK_EQ,
    TOK_COMMA,
    TOK_EQEQ,
    TOK_NOTEQ,
    TOK_LESSTHAN,
    TOK_GREATERTHAN,
    TOK_LTEQ,
    TOK_GTEQ,
    TOK_NEWLINE,
    TOK_ARROW,
    TOK_STRING,
    TOK_LSQUARE,
    TOK_RSQUARE,
    TOK_PLUSEQ,
    TOK_MINUSEQ,
} TokenType;

typedef struct {
    const char* ptr;
    size_t len;
} StrSlice;

typedef union TokenValue {
    int64_t i; // integer (short hand for TOK_INT)
    double f; // float (short hand for TOK_FLOAT)
    StrSlice str; // string (for TOK_KEYWORD, TOK_ID, TOK_STRING)
} TokenValue;

typedef struct {
    size_t index; //byte index
    size_t line;
    size_t column;
} Position;

typedef struct {
    TokenType type;
    Position start;
    Position end;
    TokenValue value;
} Token;

#ifdef __cplusplus
}
#endif

#endif // CEYLONICUS_TOKEN_H