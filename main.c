/*
    Author: RezSat <yehanwasura@duck.com>
*/

#include <stdio.h>
#include <stdlib.h>

#include "include/lexer.h"
#include "include/keywords.h"


int main() {
    FILE* fptr;
    fptr = fopen("write.cyl", "rb");
    fseek(fptr, 0, SEEK_END);
    size_t size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(size);
    fread(buffer, 1, size, fptr);
    fclose(fptr);
    
    // Initialize lexer
    Lexer lx;
    lexer_init(&lx, "write.cyl", buffer, size);
    lexer_set_keyword_fn(&lx, lexer_default_is_keyword);
    return 0;
}
