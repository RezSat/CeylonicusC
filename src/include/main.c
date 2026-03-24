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

static void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [--tokens] <file.cyl>\n", progname);
}

static const char *token_type_to_str(TokenType type) {
    switch (type) {
        case TOK_INT:     return "INT";
        case TOK_FLOAT:   return "FLOAT";
        case TOK_ID:      return "IDENTIFIER";
        case TOK_KEYWORD: return "KEYWORD";
        case TOK_STRING:  return "STRING";
        case TOK_EOF:     return "EOF";
        default:          return "UNKNOWN";
    }
}

static int read_entire_file(const char *filename, uint8_t **out_buffer, size_t *out_size) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "error: could not open file: %s\n", filename);
        return 0;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "error: failed to seek file: %s\n", filename);
        fclose(f);
        return 0;
    }

    long file_size = ftell(f);
    if (file_size < 0) {
        fprintf(stderr, "error: failed to get file size: %s\n", filename);
        fclose(f);
        return 0;
    }

    rewind(f);

    uint8_t *buffer = (uint8_t *)malloc((size_t)file_size);
    if (!buffer) {
        fprintf(stderr, "error: out of memory while reading: %s\n", filename);
        fclose(f);
        return 0;
    }

    size_t bytes_read = fread(buffer, 1, (size_t)file_size, f);
    fclose(f);

    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "error: failed to read file fully: %s\n", filename);
        free(buffer);
        return 0;
    }

    *out_buffer = buffer;
    *out_size = (size_t)file_size;
    return 1;
}

static void print_lexer_error(const Lexer *lx, LexerStatus status) {
    fprintf(stderr, "lexer error: ");

    switch (status) {
        case LEX_INVALID_UTF8:
            fprintf(stderr, "invalid UTF-8 sequence\n");
            break;
        case LEX_ILLEGAL_CHAR:
            fprintf(stderr, "illegal character\n");
            break;
        case LEX_UNTERMINATED_STRING:
            fprintf(stderr, "unterminated string literal\n");
            break;
        case LEX_EXPECTED_CHAR:
            fprintf(stderr, "expected '%c'\n", lx->expected_ascii);
            break;
        default:
            fprintf(stderr, "unknown lexer error (%d)\n", status);
            break;
    }
}

static int run_lexer(const char *filename, const uint8_t *buffer, size_t size, int dump_tokens) {
    Lexer lx;
    lexer_init(&lx, filename, buffer, size);
    lexer_set_keyword_fn(&lx, lexer_default_is_keyword);

    Token tok;
    LexerStatus status;

    while ((status = lexer_next_token(&lx, &tok)) == LEX_OK) {
        if (dump_tokens) {
            printf("%s\n", token_type_to_str(tok.type));
        }
    }

    if (status == LEX_EOF) {
        return 0;
    }

    print_lexer_error(&lx, status);
    return 2;
}

int main(int argc, char **argv) {
    int dump_tokens = 0;
    const char *filename = NULL;

    if (argc == 2) {
        filename = argv[1];
    } else if (argc == 3 && strcmp(argv[1], "--tokens") == 0) {
        dump_tokens = 1;
        filename = argv[2];
    } else {
        print_usage(argv[0]);
        return 1;
    }

    uint8_t *buffer = NULL;
    size_t size = 0;

    if (!read_entire_file(filename, &buffer, &size)) {
        return 1;
    }

    int result = run_lexer(filename, buffer, size, dump_tokens);

    free(buffer);
    return result;
}
