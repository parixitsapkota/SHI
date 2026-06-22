/*
 * SHI_UTF16
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_utf16.h
 *
 * IMPLEMENTATION:
 * Define SHI_UTF16_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_UTF16_IMPLEMENTATION
 * #include "shi_utf16.h"
 */

#ifndef SHI_UTF16_H
#define SHI_UTF16_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Rune uint32_t
#define U16 uint16_t

bool shi_is_utf16_bmp_unit(Rune r);
bool shi_is_utf16_high_surrogate(U16 c);
bool shi_is_utf16_low_surrogate(U16 c);
bool shi_is_utf16_surrogate(U16 c);
bool shi_is_utf16_supplementary(Rune r);
bool shi_is_utf16_unused(Rune r);

Rune shi_utf16_decode_pair(U16 high, U16 low);
void shi_utf16_encode_pair(Rune r, U16 *high, U16 *low);

Rune *shi_utf16_decode(const U16 *buffer, size_t *len);
U16 *shi_utf16_encode(const Rune *buffer, size_t *len);

#endif // SHI_UTF16_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#define SHI_UTF16_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// U+000041 U+0000F6 U+0020AC U+01F600

void utf16_print(const Rune *runes, size_t len) {
  if (!runes)
    return;
  for (size_t i = 0; i < len; ++i) {
    printf("Rune : U+%06X\n", runes[i]);
  }
}

void utf16_print_units(const U16 *buf, size_t len) {
  if (!buf)
    return;
  for (size_t i = 0; i < len; ++i) {
    printf("%04X ", buf[i]);
  }
  printf("\n");
}

int main(void) {
  U16 buffer[] = {0x0041, 0x00F6, 0x20AC, 0xD83D, 0xDE00};
  size_t len = sizeof(buffer) / sizeof(buffer[0]);

  Rune *runes = shi_utf16_decode(buffer, &len);
  utf16_print(runes, len);

  size_t enc_len = len;
  U16 *reencoded = shi_utf16_encode(runes, &enc_len);
  utf16_print_units(reencoded, enc_len);

  free(runes);
  free(reencoded);
  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_UTF16_IMPLEMENTATION

bool shi_is_utf16_bmp_unit(const Rune r) {
  return r <= 0xFFFF && !(r >= 0xD800 && r <= 0xDFFF);
}

bool shi_is_utf16_high_surrogate(const U16 c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

bool shi_is_utf16_low_surrogate(const U16 c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

bool shi_is_utf16_surrogate(const U16 c) { return c >= 0xD800 && c <= 0xDFFF; }

bool shi_is_utf16_supplementary(const Rune r) {
  return r >= 0x10000 && r <= 0x10FFFF;
}

bool shi_is_utf16_unused(const Rune r) {
  return (r >= 0xD800 && r <= 0xDFFF) || r > 0x10FFFF;
}

Rune shi_utf16_decode_pair(const U16 high, const U16 low) {
  return (((Rune)(high - 0xD800) << 10) | (Rune)(low - 0xDC00)) + 0x10000;
}

void shi_utf16_encode_pair(const Rune r, U16 *high, U16 *low) {
  Rune v = r - 0x10000;
  *high = (U16)(0xD800 + (v >> 10));
  *low = (U16)(0xDC00 + (v & 0x3FF));
}

#include <stdlib.h>

Rune *shi_utf16_decode(const U16 *buffer, size_t *len) {
  if (!buffer || !len)
    return NULL;

  size_t length = *len;
  size_t rune_count = 0;

  for (size_t i = 0; i < length; ++i) {
    if (shi_is_utf16_high_surrogate(buffer[i])) {
      if (i + 1 < length && shi_is_utf16_low_surrogate(buffer[i + 1])) {
        ++rune_count;
        ++i;
      }
    } else if (!shi_is_utf16_low_surrogate(buffer[i])) {
      ++rune_count;
    }
  }

  Rune *runes = (Rune *)malloc(sizeof(Rune) * rune_count);
  if (!runes)
    return NULL;

  size_t i = 0, j = 0;
  while (i < length && j < rune_count) {
    if (shi_is_utf16_high_surrogate(buffer[i]) && i + 1 < length &&
        shi_is_utf16_low_surrogate(buffer[i + 1])) {
      runes[j++] = shi_utf16_decode_pair(buffer[i], buffer[i + 1]);
      i += 2;
    } else if (shi_is_utf16_low_surrogate(buffer[i]) ||
               shi_is_utf16_high_surrogate(buffer[i])) {
      i += 1;
    } else {
      runes[j++] = (Rune)buffer[i];
      i += 1;
    }
  }

  *len = j;
  return runes;
}

U16 *shi_utf16_encode(const Rune *buffer, size_t *len) {
  if (!buffer || !len)
    return NULL;

  size_t rune_count = *len;
  size_t unit_count = 0;

  for (size_t i = 0; i < rune_count; ++i) {
    Rune r = buffer[i];
    if (shi_is_utf16_bmp_unit(r)) {
      unit_count += 1;
    } else if (shi_is_utf16_supplementary(r)) {
      unit_count += 2;
    }
  }

  U16 *out = (U16 *)malloc(sizeof(U16) * unit_count);
  *len = unit_count;
  if (!out)
    return NULL;

  size_t j = 0;
  for (size_t i = 0; i < rune_count; ++i) {
    Rune r = buffer[i];

    if (shi_is_utf16_bmp_unit(r)) {
      out[j++] = (U16)r;
    } else if (shi_is_utf16_supplementary(r)) {
      U16 high, low;
      shi_utf16_encode_pair(r, &high, &low);
      out[j++] = high;
      out[j++] = low;
    }
    // runes that are surrogate values on their own, or above 0x10FFFF
  }

  *len = j;
  return out;
}

#endif // SHI_UTF16_IMPLEMENTATION

/*
 * Resources:
 * https://en.wikipedia.org/wiki/UTF-16
 *
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
