#include <string.h>
#include "token.h"

static const char* CEY_KEYWORDS[] = {


};


int lexer_default_is_keyword(StrSlice s) {
    return 0;
}