/*
 * hstr (string hasing).
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_hstr.h
 *
 * IMPLEMENTATION:
 * Define SHI_HSRT_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_HSRT_IMPLEMENTATION
 * #include "shi_hstr.h"
 */

#ifndef SHI_HSRT_H
#define SHI_HSRT_H

#include <stddef.h>
#include <stdint.h>

size_t shi_hstr_fnv1a(const char *str);
size_t shi_hstr_fnv1a_seed(const char *str, uint32_t seed);

#endif // SHI_HSRT_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#define SHI_HSRT_IMPLEMENTATION

#include <stdio.h>

#define test_fnv1a(str) printf("%s => %lu\n", str, shi_hstr_fnv1a(str))
#define test_fnv1a_seed(str, seed) printf("%s => %lu\n", str, shi_hstr_fnv1a_seed(str, seed))

int main() {
  printf("Normal hstr\n");
  test_fnv1a("indentifier1");
  test_fnv1a("indentifier2");
  printf("seeded hstr\n");
  test_fnv1a_seed("indentifier1", 99);
  test_fnv1a_seed("indentifier2", 99);
  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_HSRT_IMPLEMENTATION

size_t shi_hstr_fnv1a(const char *str) {
  size_t hash;
  size_t fnv_prime;

  if (sizeof(size_t) == 8) {
    // 64-bit constants
    hash = 14695981039346656037ULL;
    fnv_prime = 1099511628211ULL;
  } else {
    // 32-bit constants
    hash = 2166136261U;
    fnv_prime = 16777619U;
  }

  while (*str) {
    hash ^= (unsigned char)(*str);
    hash *= fnv_prime;
    str++;
  }

  return hash;
}

size_t shi_hstr_fnv1a_seed(const char *str, uint32_t seed) {
  size_t hash;
  size_t fnv_prime;

  if (sizeof(size_t) == 8) {
    // 64-bit constants
    hash = 14695981039346656037ULL ^ seed;
    fnv_prime = 1099511628211ULL;
  } else {
    // 32-bit constants
    hash = 2166136261U ^ seed;
    fnv_prime = 16777619U;
  }

  while (*str) {
    hash ^= (uint8_t)(*str);
    hash *= fnv_prime;
    str++;
  }
  return hash;
}

#endif // SHI_HSRT_IMPLEMENTATION

/*
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
