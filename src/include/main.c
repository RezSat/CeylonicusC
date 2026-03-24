/*
    Author: RezSat <yehanwasura@duck.com>
*/

#include <stdio.h>

static void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s <file.cyl>\n", progname);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    printf("input: %s\n", argv[1]);
    return 0;
}
