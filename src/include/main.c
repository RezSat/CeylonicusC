/*
    Author: RezSat <yehanwasura@duck.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s <file.cyl>\n", progname);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "error: could not open file: %s\n", filename);
        return 1;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        fprintf(stderr, "error: failed to seek file: %s\n", filename);
        return 1;
    }

    long file_size = ftell(f);
    if (file_size < 0) {
        fclose(f);
        fprintf(stderr, "error: failed to get file size: %s\n", filename);
        return 1;
    }

    rewind(f);

    uint8_t *buffer = (uint8_t *)malloc((size_t)file_size);
    if (!buffer) {
        fclose(f);
        fprintf(stderr, "error: out of memory while reading: %s\n", filename);
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, (size_t)file_size, f);
    fclose(f);

    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "error: failed to read file fully: %s\n", filename);
        free(buffer);
        return 1;
    }

    printf("loaded %ld bytes from %s\n", file_size, filename);
    free(buffer);
    return 0;
}
