/*
 * SHI_OPA (Object Pool Allocator)
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_opa.h
 *
 * Note: SHI_OPA is not recommended for string allocations.
 *
 * IMPLEMENTATION:
 * Define SHI_OPA_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_OPA_IMPLEMENTATION
 * #include "shi_opa.h"
 */

#ifndef SHI_OPA_H
#define SHI_OPA_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct __mem_block__ {
  size_t type_size;           /* : Size of object in bytes */
  size_t type_offset;         /* : Current numbers of objects in memory */
  size_t type_cap;            /* : Number of objects memory can hold */
  struct __mem_block__ *next; /* : Next block of memory */
  void *bytes;                /* : Actual memory block bytes */
} __mem_block__;

/// Initializes a new __mem_block__
__mem_block__ *init_mem_block(size_t size, size_t cap);

/// Allocates a new memory block block, chains it, and updates the caller's
/// pointer to point to the newly created active block.
int push_new_mem_block(__mem_block__ **current);

/// Pushes an object into the memory block.
/// Takes a pointer refrence so it can automatically append a new block if the
/// current one is full.
void push_to_mem_block(__mem_block__ **current, void *object);

/// Returns the oblect at `n' index
void *at_mem_block(__mem_block__ *head, size_t index);

/// function to free the entire linked memory chain
void free_mem_chain(__mem_block__ *head);

// macro interface.
#define SHI_OPA __mem_block__

/// Init Object Pool.
#define shi_opa_init(type, block_capa)                                         \
  (SHI_OPA *)init_mem_block(sizeof(type), block_capa)
/// Push Object to Object Pool.
#define shi_opa_push(pool, object)                                             \
  push_to_mem_block((SHI_OPA **)&pool, &object)
/// Get object at `n' index from Object Pool.
#define shi_opa_index(pool_head, index)                                        \
  at_mem_block((SHI_OPA *)pool_head, index)
/// Destroy Object Pool.
#define shi_opa_free(pool_head) free_mem_chain((SHI_OPA *)pool_head)

#endif // SHI_OPA_H

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_OPA_IMPLEMENTATION

__mem_block__ *init_mem_block(size_t size, size_t cap) {
  __mem_block__ *block = (__mem_block__ *)malloc(sizeof(__mem_block__));
  if (!block)
    return NULL;

  void *bytes = (void *)malloc(size * cap);
  if (!bytes) {
    free(block);
    return NULL;
  }

  *block = (__mem_block__){size, 0, cap, NULL, bytes};
  return block;
}

int push_new_mem_block(__mem_block__ **current) {
  if (!current || !*current)
    return 0;

  __mem_block__ *next =
      init_mem_block((*current)->type_size, (*current)->type_cap);
  if (!next) {
    return 0;
  }

  (*current)->next = next;
  *current = next;
  return 1;
}

void push_to_mem_block(__mem_block__ **current, void *object) {
  if (!current || !*current || !object)
    return;

  if ((*current)->type_offset >= (*current)->type_cap) {
    if (!push_new_mem_block(current)) {
      return;
    }
  }

  // Calculate byte Offset
  char *dest = (char *)(*current)->bytes +
               ((*current)->type_offset * (*current)->type_size);

  // Copy the object from refrence
  char *src = (char *)object;
  for (size_t i = 0; i < (*current)->type_size; ++i) {
    dest[i] = src[i];
  }

  ++(*current)->type_offset;
}

void *at_mem_block(__mem_block__ *head, size_t index) {
  __mem_block__ *current = head;
  while (current != NULL) {
    if (index < current->type_cap) {

      char *target_address =
          (char *)current->bytes + (index * current->type_size);
      return (void *)target_address;
    }

    index -= current->type_cap;
    current = current->next;
  }
  return NULL;
}

void free_mem_chain(__mem_block__ *head) {
  while (head) {
    __mem_block__ *temp = head->next;
    free(head->bytes);
    free(head);
    head = temp;
  }
}

#endif // SHI_OPA_IMPLEMENTATION

/*
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
