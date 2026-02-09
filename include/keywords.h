/*
    Author: RezSat <yehanwasura@duck.com>
*/


#ifndef CEYLONICUS_KEYWORDS_H
#define CEYLONICUS_KEYWRODS_H

#include "token.h"

#ifdef __cplusplus
extern "C" {
#endif

int lexer_default_is_keyword(StrSlice s);

#ifdef __cplusplus
}
#endif

#endif  //CEYLONICUS_KEYWORDS_H