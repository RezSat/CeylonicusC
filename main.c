/*
    Author: RezSat <yehanwasura@duck.com>
*/

#include <stdio.h>

#include "include/lexer.h"
#include "include/keywords.h"


int main() {
    FILE* fptr;
    fptr = fopen("write.cyl", "r");
    printf("%s", fptr);
    return 0;
}
