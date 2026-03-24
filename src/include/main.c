/*
    Author: RezSat <yehanwasura@duck.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lexer.h"
#include "keywords.h"
#include "token.h"

static const char *token_type_to_str(TokenType type) {
    switch (type) {
        case TOK_INT:         return "INT";
        case TOK_FLOAT:       return "FLOAT";
        case TOK_ID:          return "IDENTIFIER";
        case TOK_KEYWORD:     return "KEYWORD";
        case TOK_STRING:      return "STRING";
        case TOK_PLUS:        return "PLUS";
        case TOK_MINUS:       return "MINUS";
        case TOK_MUL:         return "MUL";
        case TOK_DIV:         return "DIV";
        case TOK_LPAREN:      return "LPAREN";
        case TOK_RPAREN:      return "RPAREN";
        case TOK_LSQUARE:     return "LSQUARE";
        case TOK_RSQUARE:     return "RSQUARE";
        case TOK_COMMA:       return "COMMA";
        case TOK_EQ:          return "EQ";
        case TOK_EQEQ:        return "EQEQ";
        case TOK_NOTEQ:       return "NOTEQ";
        case TOK_LESSTHAN:    return "LESSTHAN";
        case TOK_GREATERTHAN: return "GREATERTHAN";
        case TOK_LTEQ:        return "LTEQ";
        case TOK_GTEQ:        return "GTEQ";
        case TOK_POWER:       return "POWER";
        case TOK_ARROW:       return "ARROW";
        case TOK_PLUSEQ:      return "PLUSEQ";
        case TOK_MINUSEQ:     return "MINUSEQ";
        case TOK_NEWLINE:     return "NEWLINE";
        case TOK_EOF:         return "EOF";
        default:              return "UNKNOWN";
    }
}