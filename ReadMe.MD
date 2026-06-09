<div align="center">
  <img src="https://github.com/oarixsapkota/laba/blob/master/img/laba.jpg"><br>
</div>

A lightweight, single-header C library that implements a Linked Arena Bump Allocator.

It provides extremely fast memory allocation by "bumping" a pointer across a pre-allocated chunk of linked memory chunks. When the chunk is full, LABA automatically allocates a new chunk and links it to the previous one. This eliminates heap fragmentation, guarantees excellent cache locality for iterations, and allows you to free an entire data structure with a single function call.

## Features

* **Single-Header Design:** Drop `laba.h` into your project and you are ready to go. No complex build systems required.
* **O(1) Allocations:** Pushing data into the arena is as fast as incrementing an offset.
* **Fragmentation:** Fragmentation associated with calling `malloc` for hundreds of small, individual nodes is reduced.
* **Instant Cleanup:** Freeing the arena cleans up all allocated nodes instantly.

---

## Usage

Because LABA is an STB-style single-header library, you need to implement it in exactly **one** C file to avoid linker errors.

1. Copy `laba.h` into your project.
2. In exactly **one** `.c` file, define `LABA_IMPLEMENTATION` before including the header.
```c
#define LABA_IMPLEMENTATION
#include "laba.h"

int main() {
  // Your code here...
  return 0;
}

```

In all other files where you need the allocator, simply include the header normally

---

## Quick Start Example

Here is a complete example of how to initialize an arena, push structs into it, iterate over them, and clean up.

```c
#include <stdio.h>

#define LABA_IMPLEMENTATION
#include "laba.h"

typedef struct {
  float x;
  float y;
} Vector2;

int main() {
  // 1. Initialize an arena for Vector2 structs with a chunk capacity of 100
  Laba *arena = init_laba(Vector2, 100);

  // 2. Push data directly into the arena
  for (int i = 0; i < 250; i++) {
    Vector2 vec = {(float)i, (float)i * 2.0f};
    // push_laba automatically copies the struct data into the contiguous memory block
    push_laba(arena, Vector2, &vec); 
  }

  // 3. Iterate over the data
  for (size_t i = 0; i < 250; i++) {
    // index_laba returns a void pointer, cast it to your type
    Vector2 *v = (Vector2 *)index_laba(arena, i);
    printf("Vector %zu: X=%.1f, Y=%.1f\n", i, v->x, v->y);
  }

  // 4. Clean up all 250 vectors and the internal memory blocks at once
  destroy_laba(arena);

  return 0;
}

```

---

## 📖 API Reference

LABA provides convenient, type-aware macros that wrap the underlying  `void*` C functions.

### Macros

| Macro | Description |
| --- | --- |
| `init_laba(type, capacity)` | Initializes a new arena. `type` is the struct or data type you want to store. `capacity` is the number of elements per memory block. |
| `push_laba(laba, type, node)` | Pushes a node into the arena. `node` should be a pointer to the data you want to copy into the arena. |
| `index_laba(laba, index)` | Retrieves a pointer to the element at the specified logical index. Returns `NULL` if out of bounds. |
| `destroy_laba(laba)` | Safely frees the arena, all linked memory blocks, and the contained data. |

### Core Functions

If you prefer to bypass the macros, you can use the underlying C functions directly:

* `Laba *laba_init(size_t capacity, size_t type_size);`
* `void laba_push(Laba *self, void *node);`
* `void *laba_index(Laba *self, size_t index);`
* `void laba_destroy(Laba *self);`

---

## 📄 License

This project is open-source and available under the [Apache License](https://github.com/oarixsapkota/LABA/blob/main/LICENSE).
