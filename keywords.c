/*
    Author: RezSat <yehanwasura@duck.com>
*/


#include <string.h>
#include "keywords.h"

// Simply got from the github.com/RezSat/Ceylonicus/tokens.py 
static const char* CEY_KEYWORDS[] = {
  "var","and","or","not","if","elseif","else","for","to","step","while",
  "function","then","end","return","continue","break","do",
  "විචල්ය","විචල්‍ය","සහ","හෝ","නොමැත","නොව","නැත","නොවේ","නොවන",
  "ශ්‍රීතය","කාර්යය","නම්","නැත්නම්","නැතිනම්","මෙහි",
  "එසේ_නැත්නම්","එසේ_නැතිනම්","එසේත්_නැත්නම්","එසේත්_නැතිනම්",
  "අවසන්","දක්වා","පියවර","තෙක්","සිට","අතර","අතරතුර",
  "නවත්වන්න","දෙන්න","දිගටම","කරන්න",
  NULL
};


int lexer_default_is_keyword(StrSlice s) {
    for (size_t k = 0; CEY_KEYWORDS[k]; k++) {
        const char* kw = CEY_KEYWORDS[k];
        size_t kwlen = strlen(kw);
        if (s.len == kwlen && memcmp(s.ptr, kw, kwlen) == 0) return 1;
    }
    return 0;
}