/*
 * SHI_UTF32
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_utf32.h
 *
 * IMPLEMENTATION:
 * Define SHI_UTF32_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_UTF32_IMPLEMENTATION
 * #include "shi_utf32.h"
 */

#ifndef SHI_UTF32_H
#define SHI_UTF32_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Rune uint32_t
#define U32 uint32_t

bool shi_is_utf32_valid_unit(U32 c);
bool shi_is_utf32_surrogate(U32 c);
bool shi_is_utf32_unused(U32 c);

Rune *shi_utf32_decode(const U32 *buffer, size_t *len);
U32 *shi_utf32_encode(const Rune *buffer, size_t *len);

#endif // SHI_UTF32_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#define SHI_UTF32_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// U+000041 U+0000F6 U+0020AC U+01F600

void utf32_print(const Rune *runes, size_t len) {
  if (!runes)
    return;
  for (size_t i = 0; i < len; ++i) {
    printf("Rune : U+%06X\n", runes[i]);
  }
}

void utf32_print_units(const U32 *buf, size_t len) {
  if (!buf)
    return;
  for (size_t i = 0; i < len; ++i) {
    printf("%08X ", buf[i]);
  }
  printf("\n");
}

int main(void) {
  U32 buffer[] = {0x000041, 0x0000F6, 0x0020AC, 0x01F600};
  size_t len = sizeof(buffer) / sizeof(buffer[0]);

  Rune *runes = shi_utf32_decode(buffer, &len);
  utf32_print(runes, len);

  size_t enc_len = len;
  U32 *reencoded = shi_utf32_encode(runes, &enc_len);
  utf32_print_units(reencoded, enc_len);

  free(runes);
  free(reencoded);
  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_UTF32_IMPLEMENTATION

bool shi_is_utf32_surrogate(const U32 c) { return c >= 0xD800 && c <= 0xDFFF; }

bool shi_is_utf32_valid_unit(const U32 c) {
  return c <= 0x10FFFF && !shi_is_utf32_surrogate(c);
}

bool shi_is_utf32_unused(const U32 c) { return !shi_is_utf32_valid_unit(c); }

#include <stdlib.h>

Rune *shi_utf32_decode(const U32 *buffer, size_t *len) {
  if (!buffer || !len)
    return NULL;

  size_t length = *len;
  size_t rune_count = 0;

  for (size_t i = 0; i < length; ++i) {
    if (shi_is_utf32_valid_unit(buffer[i])) {
      ++rune_count;
    }
  }

  Rune *runes = (Rune *)malloc(sizeof(Rune) * rune_count);
  if (!runes)
    return NULL;

  size_t i = 0, j = 0;
  while (i < length && j < rune_count) {
    if (shi_is_utf32_valid_unit(buffer[i])) {
      runes[j++] = (Rune)buffer[i];
    }
    // invalid units (surrogate values or > 0x10FFFF) are dropped
    i += 1;
  }

  *len = j;
  return runes;
}

U32 *shi_utf32_encode(const Rune *buffer, size_t *len) {
  if (!buffer || !len)
    return NULL;

  size_t rune_count = *len;
  size_t unit_count = 0;

  // counts for valid runes
  for (size_t i = 0; i < rune_count; ++i) {
    if (shi_is_utf32_valid_unit(buffer[i])) {
      ++unit_count;
    }
  }

  U32 *out = (U32 *)malloc(sizeof(U32) * unit_count);
  *len = unit_count;
  if (!out)
    return NULL;

  size_t j = 0;
  for (size_t i = 0; i < rune_count; ++i) {
    if (shi_is_utf32_valid_unit(buffer[i])) {
      out[j++] = (U32)buffer[i];
    }
    // invalid runes are silently skipped, matching the count above
  }

  *len = j;
  return out;
}

#endif // SHI_UTF32_IMPLEMENTATION

/*
 * Resources:
 * https://en.wikipedia.org/wiki/UTF-32
 *
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
