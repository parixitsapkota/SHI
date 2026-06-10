#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHI_OPA_IMPLEMENTATION
#include "shi_opa.h"

char *s_read_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "FATAL : Failed to open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  const size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = malloc(file_size + 1);

  const size_t bytes = fread(content, 1, file_size, file);
  if (bytes != file_size) {
    fprintf(stderr, "FATAL : Failed to read bytes form file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  content[file_size] = '\0';

  fclose(file);
  return content;
}

char *s_substr(const char *str, const size_t start, const size_t end) {
  const size_t length = end - start;
  char *substr = malloc(length + 1);
  strncpy(substr, str + start, length);
  substr[length] = '\0';
  return substr;
}

char *get_word(const char *buffer, size_t *i, size_t *col) {
  const size_t start = *i;
  while (!isspace(buffer[*i])) {
    ++*col;
    ++*i;
  }
  return s_substr(buffer, start, *i);
}

typedef struct {
  char *word;
  size_t line;
  size_t col;
} Word;

int main(void) {
  char *buffer = s_read_file("test.txt");

  SHI_OPA *pool_head = shi_opa_init(Word, 10);
  SHI_OPA *pool = pool_head;

  size_t i = 0, line = 1, col = 1;
  while (buffer[i] != '\0') {
    // Handle whitespace.
    if (isspace(buffer[i])) {
      if (buffer[i] == '\n') {
        col = 1;
        ++line;
      } else {
        ++col;
      }
      ++i;
      continue;
    }
    size_t tcol = col;
    char *word_value = get_word(buffer, &i, &col);
    Word word = (Word){word_value, line, tcol};
    shi_opa_push(pool, word);
    ++i;
  }
  Word word = (Word){NULL, 0, 0};
  shi_opa_push(pool, word);

  i = 0;
  for (;;) {
    Word *word = shi_opa_index(pool_head, i);
    if (!word->word)
      break;
    printf("Word : %s\n", word->word);
    free(word->word);
    ++i;
  }

  shi_opa_free(pool_head);

  free(buffer);
  return 0;
}
