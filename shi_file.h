/*
 * SHI_FILE (file read/write)
 * Original: https://github.com/oarixsapkota/SHI/blob/main/shi_file.h
 *
 * IMPLEMENTATION:
 * Define SHI_FILE_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_FILE_IMPLEMENTATION
 * #include "shi_file.h"
 */

#ifndef SHI_FILE_H
#define SHI_FILE_H

#include <stddef.h>

char *read_file(const char *path, size_t *bytes);
void write_file(const char *path, const char *buffer, const char *mode);

#define shi_file_read(path, bytes) read_file(path, bytes)
#define shi_file_write(path, buffer) write_file(path, buffer, "wb")
#define shi_file_append(path, buffer) write_file(path, buffer, "a")

#define shi_file_create(path) shi_file_append(path, NULL);
#define shi_file_remove(path) remove(path)

#endif // SHI_FILE_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#include <stdio.h>
#include <stdlib.h>
#define SHI_FILE_IMPLEMENTATION

int main() {
  size_t bytes = 0;
  char *read_buffer = shi_file_read("shi_file.h", &bytes);
  printf("Read %lu bytes.\n", bytes);
  free(read_buffer);

  shi_file_write("build/test.txt", "first line.");

  shi_file_append("build/test.txt", "\nsecond line.");

  shi_file_create("build/a_file");

  shi_file_create("build/emptyfile");

  shi_file_remove("build/a_file");

  return 0;
}

#endif // SHI_TEST

// ===============
// IMPLEMENTATION
// ===============
#ifdef SHI_FILE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path, size_t *bytes) {
  FILE *file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "FATAL : Failed to open file: %s\n", path);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  const size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = malloc(file_size + 1);
  if (!content) {
    fprintf(stderr, "Failed to allocate %lu bytes for: %s\n", file_size, path);
    exit(EXIT_FAILURE);
  }

  const size_t bytes_red = fread(content, 1, file_size, file);
  if (bytes_red != file_size) {
    fprintf(stderr, "FATAL : Failed to read bytes form file: %s\n", path);
    exit(EXIT_FAILURE);
  }

  content[file_size] = '\0';

  if (bytes) {
    *bytes = bytes_red;
  }
  fclose(file);
  return content;
}

void write_file(const char *path, const char *buffer, const char *mode) {
  FILE *file = fopen(path, mode);
  if (!file) {
    fprintf(stderr, "Failed to open file: %s\n", path);
    exit(EXIT_FAILURE);
  }
  if (buffer) {
    fputs(buffer, file);
  }
  fclose(file);
}

#endif // SHI_FILE_IMPLEMENTATION

/*
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/oarixsapkota/SHI?tab=Apache-2.0-1-ov-file
 */
