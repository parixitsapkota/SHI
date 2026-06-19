/*
 * SHI_HS (Hash Set / Hash Table)
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_hs.h
 *
 * Note: Keys are null-terminated strings. Values are stored as `void *`,
 * so SHI_HS can hold pointers to anything (or be cast back from integers).
 *
 * IMPLEMENTATION:
 * Define SHI_HS_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_HS_IMPLEMENTATION
 * #include "shi_hs.h"
 */

#ifndef SHI_HS_H
#define SHI_HS_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* : Table grows (doubles bucket_cap) once count / bucket_cap exceeds this. */
#define SHI_HS_LOAD_FACTOR 0.75

typedef struct __hs_entry__ {
  char *key;                 /* : Null-terminated key string (owned copy) */
  void *value;               /* : Stored value */
  int in_use;                /* : 0 if slot is empty, 1 if occupied */
  struct __hs_entry__ *next; /* : Next entry in this bucket's chain */
} __hs_entry__;

typedef struct __hash_set__ {
  size_t bucket_cap;      /* : Number of buckets in the table */
  size_t count;           /* : Number of key/value pairs stored */
  __hs_entry__ **buckets; /* : Array of bucket chain heads */
} __hash_set__;

/// Initializes a new __hash_set__ with `bucket_cap' buckets.
__hash_set__ *init_hash_set(size_t bucket_cap);

/// Hashes a null-terminated string (FNV-1a).
size_t hash_string(const char *str);

/// Inserts or updates the value associated with `key'.
void put_to_hash_set(__hash_set__ *set, const char *key, void *value);

/// Grows the set to `new_bucket_cap' buckets and rehashes every entry.
/// Called automatically by put_to_hash_set when the load factor is exceeded,
/// but can also be called directly to pre-size a set.
void resize_hash_set(__hash_set__ *set, size_t new_bucket_cap);

/// Returns the value associated with `key', or NULL if not found.
void *get_from_hash_set(__hash_set__ *set, const char *key);

/// Returns 1 if `key' exists in the set, 0 otherwise.
int has_in_hash_set(__hash_set__ *set, const char *key);

/// Removes `key' from the set. Returns 1 if it was present, 0 otherwise.
int del_from_hash_set(__hash_set__ *set, const char *key);

/// Frees the entire hash set, including all chained entries and their keys.
void free_hash_set(__hash_set__ *set);

// macro interface.
#define SHI_HS __hash_set__

/// Init Hash Set with `cap' buckets.
#define shi_hs_init(cap) (SHI_HS *)init_hash_set(cap)
/// Put `key' -> `value' into the Hash Set.
#define shi_hs_put(set, key, value)                                            \
  put_to_hash_set((SHI_HS *)set, key, (void *)(value))
/// Manually resize the Hash Set to `cap' buckets (also happens automatically).
#define shi_hs_resize(set, cap) resize_hash_set((SHI_HS *)set, cap)
/// Get the value for `key' from the Hash Set.
#define shi_hs_get(set, key) get_from_hash_set((SHI_HS *)set, key)
/// Check if `key' exists in the Hash Set.
#define shi_hs_has(set, key) has_in_hash_set((SHI_HS *)set, key)
/// Delete `key' from the Hash Set.
#define shi_hs_del(set, key) del_from_hash_set((SHI_HS *)set, key)
/// Destroy Hash Set.
#define shi_hs_free(set) free_hash_set((SHI_HS *)set)

#endif // SHI_HS_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#include <stddef.h>
#include <stdio.h>

#define SHI_HS_IMPLEMENTATION

int main(void) {
  SHI_HS *set = shi_hs_init(8);

  int a = 1, b = 2, c = 3;
  shi_hs_put(set, "one", &a);
  shi_hs_put(set, "two", &b);
  shi_hs_put(set, "three", &c);

  const char *keys[] = {"one", "two", "three", "four"};
  for (size_t i = 0; i < 4; ++i) {
    int *val = (int *)shi_hs_get(set, keys[i]);
    if (val)
      printf("KEY : %s -> %d\n", keys[i], *val);
    else
      printf("KEY : %s -> (missing)\n", keys[i]);
  }

  shi_hs_del(set, "two");
  printf("has \"two\" after delete: %d\n", shi_hs_has(set, "two"));

  // Table grows automatically as load factor is exceeded.
  printf("bucket_cap before bulk insert: %zu\n", set->bucket_cap);
  for (int i = 0; i < 100; ++i) {
    char namebuf[16];
    sprintf(namebuf, "k%d", i);
    shi_hs_put(set, namebuf, (void *)(size_t)i);
  }
  printf("bucket_cap after bulk insert: %zu\n", set->bucket_cap);
  printf("count after bulk insert: %zu\n", set->count);

  // Manual resize (e.g. to pre-size before a known number of inserts).
  shi_hs_resize(set, 256);
  printf("bucket_cap after manual resize: %zu\n", set->bucket_cap);
  printf("k42 still found after resize: %d\n",
         (int)(size_t)shi_hs_get(set, "k42"));

  shi_hs_free(set);
  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_HS_IMPLEMENTATION

#include <string.h>

// FNV-1a
size_t hash_string(const char *str) {
  size_t hash = (size_t)14695981039346656037ULL;
  while (*str) {
    hash ^= (unsigned char)(*str++);
    hash *= (size_t)1099511628211ULL;
  }
  return hash;
}

static char *__hs_strdup__(const char *str) {
  size_t len = strlen(str) + 1;
  char *copy = (char *)malloc(len);
  if (!copy)
    return NULL;
  for (size_t i = 0; i < len; ++i)
    copy[i] = str[i];
  return copy;
}

__hash_set__ *init_hash_set(size_t bucket_cap) {
  if (bucket_cap == 0)
    bucket_cap = 1;

  __hash_set__ *set = (__hash_set__ *)malloc(sizeof(__hash_set__));
  if (!set)
    return NULL;

  __hs_entry__ **buckets =
      (__hs_entry__ **)calloc(bucket_cap, sizeof(__hs_entry__ *));
  if (!buckets) {
    free(set);
    return NULL;
  }

  set->bucket_cap = bucket_cap;
  set->count = 0;
  set->buckets = buckets;
  return set;
}

void resize_hash_set(__hash_set__ *set, size_t new_bucket_cap) {
  if (!set || new_bucket_cap == 0 || new_bucket_cap == set->bucket_cap)
    return;

  __hs_entry__ **new_buckets =
      (__hs_entry__ **)calloc(new_bucket_cap, sizeof(__hs_entry__ *));
  if (!new_buckets)
    return; // allocation failed; keep old table, just degrade gracefully

  // Re-thread every existing entry into the new bucket array based on its
  // hash mod new_bucket_cap. Entries are moved (not copied), so no new
  // allocations or key copies are needed.
  for (size_t i = 0; i < set->bucket_cap; ++i) {
    __hs_entry__ *current = set->buckets[i];
    while (current != NULL) {
      __hs_entry__ *next = current->next;
      size_t new_index = hash_string(current->key) % new_bucket_cap;

      current->next = new_buckets[new_index];
      new_buckets[new_index] = current;

      current = next;
    }
  }

  free(set->buckets);
  set->buckets = new_buckets;
  set->bucket_cap = new_bucket_cap;
}

void put_to_hash_set(__hash_set__ *set, const char *key, void *value) {
  if (!set || !key)
    return;

  size_t index = hash_string(key) % set->bucket_cap;
  __hs_entry__ *current = set->buckets[index];

  while (current != NULL) {
    if (current->in_use && strcmp(current->key, key) == 0) {
      current->value = value;
      return;
    }
    current = current->next;
  }

  __hs_entry__ *entry = (__hs_entry__ *)malloc(sizeof(__hs_entry__));
  if (!entry)
    return;

  entry->key = __hs_strdup__(key);
  entry->value = value;
  entry->in_use = 1;
  entry->next = set->buckets[index];

  set->buckets[index] = entry;
  ++set->count;

  // Grow when load factor (count / bucket_cap) exceeds SHI_HS_LOAD_FACTOR.
  // Compared as count > bucket_cap * factor, computed in integer-safe form
  // by cross-multiplying to avoid float division on every insert.
  if ((double)set->count > (double)set->bucket_cap * SHI_HS_LOAD_FACTOR)
    resize_hash_set(set, set->bucket_cap * 2);
}

void *get_from_hash_set(__hash_set__ *set, const char *key) {
  if (!set || !key)
    return NULL;

  size_t index = hash_string(key) % set->bucket_cap;
  __hs_entry__ *current = set->buckets[index];

  while (current != NULL) {
    if (current->in_use && strcmp(current->key, key) == 0)
      return current->value;
    current = current->next;
  }
  return NULL;
}

int has_in_hash_set(__hash_set__ *set, const char *key) {
  if (!set || !key)
    return 0;

  size_t index = hash_string(key) % set->bucket_cap;
  __hs_entry__ *current = set->buckets[index];

  while (current != NULL) {
    if (current->in_use && strcmp(current->key, key) == 0)
      return 1;
    current = current->next;
  }
  return 0;
}

int del_from_hash_set(__hash_set__ *set, const char *key) {
  if (!set || !key)
    return 0;

  size_t index = hash_string(key) % set->bucket_cap;
  __hs_entry__ *current = set->buckets[index];
  __hs_entry__ *prev = NULL;

  while (current != NULL) {
    if (current->in_use && strcmp(current->key, key) == 0) {
      if (prev)
        prev->next = current->next;
      else
        set->buckets[index] = current->next;

      free(current->key);
      free(current);
      --set->count;
      return 1;
    }
    prev = current;
    current = current->next;
  }
  return 0;
}

void free_hash_set(__hash_set__ *set) {
  if (!set)
    return;

  for (size_t i = 0; i < set->bucket_cap; ++i) {
    __hs_entry__ *current = set->buckets[i];
    while (current != NULL) {
      __hs_entry__ *temp = current->next;
      free(current->key);
      free(current);
      current = temp;
    }
  }

  free(set->buckets);
  free(set);
}

#endif // SHI_HS_IMPLEMENTATION

/*
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
