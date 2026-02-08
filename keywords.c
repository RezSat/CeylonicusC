#include <string.h>
#include "token.h"

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
    return 0;
}