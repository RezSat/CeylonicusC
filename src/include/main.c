/*
    Author: RezSat <yehanwasura@duck.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "keywords.h"
#include "token.h"

static void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s <file.cyl>\n", progname);
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

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    uint8_t *buffer = NULL;
    size_t size = 0;

    if (!read_entire_file(filename, &buffer, &size)) {
        return 1;
    }

    Lexer lx;
    lexer_init(&lx, filename, buffer, size);
    lexer_set_keyword_fn(&lx, lexer_default_is_keyword);

    Token tok;
    LexerStatus status;
    while ((status = lexer_next_token(&lx, &tok)) == LEX_OK) {
    }

    free(buffer);

    if (status == LEX_EOF) {
        printf("lexing completed\n");
        return 0;
    }

    fprintf(stderr, "lexer failed with status %d\n", status);
    return 2;
}
