#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "keywords.h"

// Helper to turn enums into readable text
const char* token_type_to_str(TokenType type) {
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
        case TOK_NEWLINE:     return "NEWLINE";
        case TOK_EOF:         return "EOF";
        case TOK_LPAREN:      return "LPAREN";
        case TOK_RPAREN:      return "RPAREN";
        case TOK_LSQUARE:     return "LSQUARE";
        case TOK_RSQUARE:     return "RSQUARE";
        case TOK_COMMA:       return "COMMA";
        case TOK_ARROW:       return "ARROW";
        default:              return "OPERATOR/OTHER";
    }
}

int main() {
    // 1. Load the file into memory
    const char* filename = "writex.cyl";
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(size);
    fread(buffer, 1, size, f);
    fclose(f);

    // 2. Initialize Lexer
    Lexer lx;
    lexer_init(&lx, filename, buffer, size);
    
    // Register the keyword checker we defined in keywords.c
    lexer_set_keyword_fn(&lx, lexer_default_is_keyword);

    printf("Tokenizing: %s\n", filename);
    printf("---------------------------------------\n");

    // 3. Tokenization Loop
    Token tok;
    LexerStatus status;
    while ((status = lexer_next_token(&lx, &tok)) == LEX_OK) {
        printf("[%zu:%zu] %-12s | ", tok.start.line + 1, tok.start.column + 1, token_type_to_str(tok.type));

        // Display values based on type
        if (tok.type == TOK_INT) {
            printf("%lld\n", tok.value.i);
        } else if (tok.type == TOK_FLOAT) {
            printf("%f\n", tok.value.f);
        } else if (tok.type == TOK_ID || tok.type == TOK_KEYWORD || tok.type == TOK_STRING) {
            // Note: %.*s is used because StrSlice is not null-terminated
            printf("'%.*s'\n", (int)tok.value.str.len, tok.value.str.ptr);
            
            // Cleanup: lex_string allocates memory that we now own
            if (tok.type == TOK_STRING) {
                free((void*)tok.value.str.ptr);
            }
        } else {
            printf("\n");
        }
    }

    // 4. Handle End or Errors
    if (status == LEX_EOF) {
        printf("---------------------------------------\nSuccessfully reached EOF.\n");
    } else {
        printf("\nLEXER ERROR at line %zu, col %zu: ", lx.error_pos_start.line + 1, lx.error_pos_start.column + 1);
        if (status == LEX_EXPECTED_CHAR) {
            printf("Expected '%c'\n", lx.expected_ascii);
        } else if (status == LEX_ILLEGAL_CHAR) {
            printf("Illegal character (U+%04X)\n", lx.error_cp);
        } else {
            printf("Status Code: %d\n", status);
        }
    }

    free(buffer);
    return 0;
}