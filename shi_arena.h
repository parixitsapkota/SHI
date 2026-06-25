/*
 * SHI_ARENA (Region Based Arena Allocator)
 *Original: https://github.com/oarixsapkota/SHI/blob/main/shi_arena.h
 *
 * Note: based on shi_opa.h structure
 *
 * IMPLEMENTATION:
 * Define SHI_ARENA_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_ARENA_IMPLEMENTATION
 * #include "shi_arena.h"
 */

#ifndef SHI_ARENA_H
#define SHI_ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct __arena_region__ {
  size_t cap;                    /* : Region capacity in bytes */
  size_t offset;                 /* : Current bytes used in region */
  struct __arena_region__ *next; /* : Next region in chain */
  uint8_t *bytes;                /* : Actual memory region bytes */
} __arena_region__;

typedef struct __arena__ {
  size_t region_size; /* : Default size in bytes for new regions */
  __arena_region__ *begin;
  __arena_region__ *end;
} __arena__;

/// Initializes a new __arena_region__ with the given byte capacity.
__arena_region__ *init_arena_region(size_t cap);

/// Initializes a new __arena__ with the given default region size.
__arena__ *init_arena(size_t region_size);

/// Allocates a new region, chains it after the arena's current end, and
/// advances the arena's end pointer to it. Capacity is at least
/// `min_cap` bytes, or the arena's default region_size, whichever is larger.
int push_new_arena_region(__arena__ *arena, size_t min_cap);

/// Allocates `size` bytes from the arena. Walks forward through already
/// chained regions before allocating a new one.
void *arena_alloc(__arena__ *arena, size_t size);

/// Copies `size` bytes from `data` into a fresh allocation in the arena.
void *arena_memdup(__arena__ *arena, void *data, size_t size);

/// Resets all regions' offsets to 0 without freeing them, so the arena's
/// memory chain is fully reusable.
void arena_reset(__arena__ *arena);

/// Frees the entire region chain and the arena itself.
void free_arena(__arena__ *arena);

// macro interface.
#define SHI_ARENA __arena__

/// Init Arena. `region_size' is the byte size of each chained memory block.
#define shi_arena_init(region_size) init_arena(region_size)
/// Allocate `size' bytes from the Arena.
#define shi_arena_alloc(arena, size) arena_alloc((SHI_ARENA *)arena, size)
/// Allocate space for and copy an existing object into the Arena.
#define shi_arena_push(arena, object) arena_memdup((SHI_ARENA *)arena, &object, sizeof(object))
/// Reset Arena, keeping its memory chain intact for reuse.
#define shi_arena_reset(arena) arena_reset((SHI_ARENA *)arena)
/// Destroy Arena.
#define shi_arena_free(arena) free_arena((SHI_ARENA *)arena)

#endif // SHI_ARENA_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#include <stddef.h>
#include <stdio.h>

#define SHI_ARENA_IMPLEMENTATION

typedef struct {
  char value;
  size_t col;
} Obj;

int main(void) {
  char *buffer = "This is some dummy text";

  SHI_ARENA *arena = shi_arena_init(64);

  Obj *objs[64];
  size_t n = 0;

  for (size_t i = 0; buffer[i] != '\0'; ++i) {
    // skip whitespace.
    if (buffer[i] == ' ') {
      continue;
    }
    Obj obj = (Obj){buffer[i], i};
    objs[n++] = (Obj *)shi_arena_push(arena, obj);
  }

  for (size_t i = 0; i < n; ++i) {
    printf("CHAR : %c\n", objs[i]->value);
  }

  shi_arena_free(arena);
  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_ARENA_IMPLEMENTATION

__arena_region__ *init_arena_region(size_t cap) {
  __arena_region__ *region = (__arena_region__ *)malloc(sizeof(__arena_region__));
  if (!region) {
    return NULL;
  }

  uint8_t *bytes = (uint8_t *)malloc(cap);
  if (!bytes) {
    free(region);
    return NULL;
  }

  *region = (__arena_region__){cap, 0, NULL, bytes};
  return region;
}

__arena__ *init_arena(size_t region_size) {
  __arena__ *arena = (__arena__ *)malloc(sizeof(__arena__));
  if (!arena) {
    return NULL;
  }

  *arena = (__arena__){region_size, NULL, NULL};
  return arena;
}

int push_new_arena_region(__arena__ *arena, size_t min_cap) {
  if (!arena) {
    return 0;
  }

  size_t cap = arena->region_size;
  if (cap < min_cap) {
    cap = min_cap;
  }

  __arena_region__ *region = init_arena_region(cap);
  if (!region) {
    return 0;
  }

  if (!arena->end) {
    arena->begin = region;
    arena->end = region;
  } else {
    arena->end->next = region;
    arena->end = region;
  }

  return 1;
}

void *arena_alloc(__arena__ *arena, size_t size) {
  if (!arena || size == 0) {
    return NULL;
  }

  if (!arena->end) {
    if (!push_new_arena_region(arena, size)) {
      return NULL;
    }
  }

  while (arena->end->offset + size > arena->end->cap && arena->end->next != NULL) {
    arena->end = arena->end->next;
  }

  if (arena->end->offset + size > arena->end->cap) {
    if (!push_new_arena_region(arena, size)) {
      return NULL;
    }
  }

  void *dest = arena->end->bytes + arena->end->offset;
  arena->end->offset += size;
  return dest;
}

void *arena_memdup(__arena__ *arena, void *data, size_t size) {
  if (!data || size == 0) {
    return NULL;
  }

  void *dest = arena_alloc(arena, size);
  if (!dest) {
    return NULL;
  }

  uint8_t *d = (uint8_t *)dest;
  uint8_t *s = (uint8_t *)data;
  for (size_t i = 0; i < size; ++i) {
    d[i] = s[i];
  }

  return dest;
}

void arena_reset(__arena__ *arena) {
  if (!arena) {
    return;
  }

  for (__arena_region__ *region = arena->begin; region != NULL; region = region->next) {
    region->offset = 0;
  }

  arena->end = arena->begin;
}

void free_arena(__arena__ *arena) {
  if (!arena) {
    return;
  }

  __arena_region__ *region = arena->begin;
  while (region) {
    __arena_region__ *next = region->next;
    free(region->bytes);
    free(region);
    region = next;
  }

  free(arena);
}

#endif // SHI_ARENA_IMPLEMENTATION

/*
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */