#ifndef CEYLONICUS_UTF8_H
#define CEYLONICUS_UTF8_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UTF8_OK = 0,
    UTF8_EOF,        // no more bytes
    UTF8_INVALID     // malformed sequence
} Utf8Status;

/*
  Reads the next UTF-8 code point from buf[0..len).
  - *i is the current byte index; advanced on UTF8_OK
  - out_cp receives the decoded Unicode code point (scalar value)
  Returns UTF8_OK, UTF8_EOF, or UTF8_INVALID.
*/
Utf8Status utf8_next(const uint8_t* buf, size_t len, size_t* i, uint32_t* out_cp);

#ifdef __cplusplus
}
#endif

#endif // CEYLONICUS_UTF8_H
