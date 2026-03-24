/*
    Author: RezSat <yehanwasura@duck.com>
*/

#ifndef CEYLONICUS_AST_H
#define CEYLONICUS_AST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AST_BLOCK,
    AST_NUMBER,
    AST_STRING,
    AST_VAR_ACCESS,
    AST_VAR_ASSIGN,
    AST_UNARY,
    AST_BINARY,
    AST_LIST,
    AST_CALL,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_FUNC_DEF,
    AST_RETURN,
    AST_CONTINUE,
    AST_BREAK
} ASTKind;


#ifdef __cplusplus
}
#endif


#endif  //CEYLONICUS_AST_H