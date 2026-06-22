/*
 * SHI_UTF8
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_utf8.h
 *
 * IMPLEMENTATION:
 * Define SHI_UTF8_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_UTF8_IMPLEMENTATION
 * #include "shi_utf8.h"
 */

#ifndef SHI_UTF8_H
#define SHI_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Rune uint32_t
#define Uchar unsigned char

bool shi_is_utf8_ascii_control_char(Uchar c);
bool shi_is_utf8_ascii_char(Uchar c);
bool shi_is_utf8_1byte_unit(Uchar c);
bool shi_is_utf8_2byte_unit(Uchar c);
bool shi_is_utf8_3byte_unit(Uchar c);
bool shi_is_utf8_4byte_unit(Uchar c);
bool shi_is_utf8_continuation(Uchar c);
bool shi_is_utf8_unused(Uchar c);

Rune shi_utf8_decode_2byte(Uchar b1, Uchar b2);
Rune shi_utf8_decode_3byte(Uchar b1, Uchar b2, Uchar b3);
Rune shi_utf8_decode_4byte(Uchar b1, Uchar b2, Uchar b3, Uchar b4);

Rune *shi_utf8_decode(const Uchar *buffer, size_t *len);
Uchar *shi_utf8_encode(const Rune *buffer, size_t *len);

#endif // SHI_UTF8_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#define SHI_UTF8_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// U+000041 U+0000F6 U+0020AC U+01F600

void utf8_print(const Rune *utf8, size_t len) {
  if (!utf8)
    return;
  for (size_t i = 0; i < len; ++i) {
    printf("Rune : U+%06X\n", utf8[i]);
  }
}

void utf8_print_units(const Uchar *buf, size_t len) {
  if (!buf)
    return;
  for (size_t i = 0; i < len; ++i) {
    printf("%02X ", buf[i]);
  }
  printf("\n");
}

int main(void) {
  Uchar *buffer = (Uchar *)"\x41\xC3\xB6\xE2\x82\xAC\xF0\x9F\x98\x80";
  size_t len = strlen((char *)buffer);
  Rune *runes = shi_utf8_decode(buffer, &len);
  utf8_print(runes, len);

  size_t enc_len = len;
  Uchar *reencoded = shi_utf8_encode(runes, &enc_len);
  utf8_print_units(reencoded, enc_len);

  free(runes);
  free(reencoded);
  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_UTF8_IMPLEMENTATION

bool shi_is_utf8_ascii_control_char(const Uchar c) { return c <= 0x1F; }

bool shi_is_utf8_ascii_char(const Uchar c) { return c >= 0x20 && c <= 0x7E; }

bool shi_is_utf8_1byte_unit(const Uchar c) { return c >= 0x01 && c <= 0x7F; }

bool shi_is_utf8_continuation(const Uchar c) { return c >= 0x80 && c <= 0xBF; }

bool shi_is_utf8_2byte_unit(const Uchar c) { return c >= 0xC2 && c <= 0xDF; }

bool shi_is_utf8_3byte_unit(const Uchar c) { return c >= 0xE0 && c <= 0xEF; }

bool shi_is_utf8_4byte_unit(const Uchar c) { return c >= 0xF0 && c <= 0xF4; }

bool shi_is_utf8_unused(const Uchar c) {
  return c == 0xC0 || c == 0xC1 || c >= 0xF5;
}

Rune shi_utf8_decode_2byte(const Uchar b1, const Uchar b2) {
  return ((b1 & 0x1F) << 6) | (b2 & 0x3F);
}

Rune shi_utf8_decode_3byte(const Uchar b1, const Uchar b2, const Uchar b3) {
  return ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
}

Rune shi_utf8_decode_4byte(const Uchar b1, const Uchar b2, const Uchar b3,
                           const Uchar b4) {
  return ((b1 & 0x07) << 18) | ((b2 & 0x3F) << 12) | ((b3 & 0x3F) << 6) |
         (b4 & 0x3F);
}

#include <stdlib.h>

Rune *shi_utf8_decode(const Uchar *buffer, size_t *len) {
  if (!buffer || !len)
    return NULL;

  size_t length = *len;
  size_t rune_count = 0;

  for (size_t i = 0; i < length; ++i) {
    if (shi_is_utf8_1byte_unit(buffer[i]) ||
        shi_is_utf8_2byte_unit(buffer[i]) ||
        shi_is_utf8_3byte_unit(buffer[i]) ||
        shi_is_utf8_4byte_unit(buffer[i])) {
      ++rune_count;
    }
  }

  Rune *utf8 = (Rune *)malloc(sizeof(Rune) * rune_count);
  if (!utf8)
    return NULL;

  size_t i = 0, j = 0;
  while (i < length && j < rune_count) {
    if (shi_is_utf8_1byte_unit(buffer[i])) {
      utf8[j++] = (Rune)buffer[i];
      i += 1;
    } else if (shi_is_utf8_2byte_unit(buffer[i]) && i + 1 < length) {
      utf8[j++] = shi_utf8_decode_2byte(buffer[i], buffer[i + 1]);
      i += 2;
    } else if (shi_is_utf8_3byte_unit(buffer[i]) && i + 2 < length) {
      utf8[j++] =
          shi_utf8_decode_3byte(buffer[i], buffer[i + 1], buffer[i + 2]);
      i += 3;
    } else if (shi_is_utf8_4byte_unit(buffer[i]) && i + 3 < length) {
      utf8[j++] = shi_utf8_decode_4byte(buffer[i], buffer[i + 1], buffer[i + 2],
                                        buffer[i + 3]);
      i += 4;
    } else {
      i += 1;
    }
  }

  *len = j;
  return utf8;
}

Uchar *shi_utf8_encode(const Rune *buffer, size_t *len) {
  if (!buffer || !len)
    return NULL;

  size_t rune_count = *len;
  size_t unit_count = 0;

  for (size_t i = 0; i < rune_count; ++i) {
    Rune r = buffer[i];
    if (r <= 0x7F) {
      unit_count += 1;
    } else if (r <= 0x7FF) {
      unit_count += 2;
    } else if (r <= 0xFFFF) {
      unit_count += 3;
    } else if (r <= 0x10FFFF) {
      unit_count += 4;
    }
    // Runes above 0x10FFFF are invalid Unicode and are dropped (0 bytes).
  }

  Uchar *out = (Uchar *)malloc(sizeof(Uchar) * unit_count);
  *len = unit_count;
  if (!out)
    return NULL;

  size_t j = 0;
  for (size_t i = 0; i < rune_count; ++i) {
    Rune r = buffer[i];

    if (r <= 0x7F) {
      out[j++] = (Uchar)r;
    } else if (r <= 0x7FF) {
      // 110xxxxx 10xxxxxx
      out[j++] = (Uchar)(0xC0 | (r >> 6));
      out[j++] = (Uchar)(0x80 | (r & 0x3F));
    } else if (r <= 0xFFFF) {
      // 1110xxxx 10xxxxxx 10xxxxxx
      out[j++] = (Uchar)(0xE0 | (r >> 12));
      out[j++] = (Uchar)(0x80 | ((r >> 6) & 0x3F));
      out[j++] = (Uchar)(0x80 | (r & 0x3F));
    } else if (r <= 0x10FFFF) {
      // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      out[j++] = (Uchar)(0xF0 | (r >> 18));
      out[j++] = (Uchar)(0x80 | ((r >> 12) & 0x3F));
      out[j++] = (Uchar)(0x80 | ((r >> 6) & 0x3F));
      out[j++] = (Uchar)(0x80 | (r & 0x3F));
    }
    // Invalid runes (> 0x10FFFF) are silently skipped, matching the
  }

  *len = j;
  return out;
}

#endif // SHI_UTF8_IMPLEMENTATION

/*
 * Resources:
 * https://upload.wikimedia.org/wikipedia/commons/3/38/UTF-8_Encoding_Scheme.png
 *
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
