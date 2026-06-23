/*
 * SHI_FLAGS (Command-Line Flag Parser)
 * Original: https://github.com/tsoding/flag.h
 *
 * IMPLEMENTATION:
 * Define SHI_FLAGS_IMPLEMENTATION in exactly one C/C++ file before including
 * this header to instantiate the implementation block.
 *
 * #define SHI_FLAGS_IMPLEMENTATION
 * #include "shi_flags.h"
 */

#ifndef SHI_FLAGS_H
#define SHI_FLAGS_H

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SHI_FLAGS_CAP
#define SHI_FLAGS_CAP 256
#endif // SHI_FLAGS_CAP

#ifndef SHI_FLAG_LIST_INIT_CAP
#define SHI_FLAG_LIST_INIT_CAP 8
#endif // SHI_FLAG_LIST_INIT_CAP

#define shi_flag_list_append(type, list, item)                                                                                 \
  do {                                                                                                                         \
    if ((list)->count >= (list)->capacity) {                                                                                   \
      size_t new_capacity = (list)->capacity == 0 ? SHI_FLAG_LIST_INIT_CAP : (list)->capacity * 2;                             \
      (list)->items = (type *)realloc((list)->items, new_capacity * sizeof(*(list)->items));                                   \
      (list)->capacity = new_capacity;                                                                                         \
    }                                                                                                                          \
    (list)->items[(list)->count++] = item;                                                                                     \
  } while (0)

typedef struct {
  const char **items;
  size_t count;
  size_t capacity;
} Shi_Flag_List;

typedef struct {
  char **items;
  size_t count;
  size_t capacity;
} Shi_Flag_List_Mut;

/// Returns the name of the flag bound to `val' in the global context.
char *shi_flag_name(void *val);
/// Binds a short alias (e.g. "b" for "-b") to the flag bound to `val' in the global context.
void shi_flag_set_short(void *val, const char *short_name);

/// Defines a bool flag in the global context and returns a pointer to its value.
bool *shi_flag_bool(const char *name, bool def, const char *desc);
/// Defines a bool flag in the global context, bound to `var'.
void shi_flag_bool_var(bool *var, const char *name, bool def, const char *desc);

/// Defines a float flag in the global context and returns a pointer to its value.
float *shi_flag_float(const char *name, float def, const char *desc);
/// Defines a float flag in the global context, bound to `var'.
void shi_flag_float_var(float *var, const char *name, float def, const char *desc);

/// Defines a double flag in the global context and returns a pointer to its value.
double *shi_flag_double(const char *name, double def, const char *desc);
/// Defines a double flag in the global context, bound to `var'.
void shi_flag_double_var(double *var, const char *name, double def, const char *desc);

/// Defines a uint64_t flag in the global context and returns a pointer to its value.
uint64_t *shi_flag_uint64(const char *name, uint64_t def, const char *desc);
/// Defines a uint64_t flag in the global context, bound to `var'.
void shi_flag_uint64_var(uint64_t *var, const char *name, uint64_t def, const char *desc);

/// Defines a size flag (with byte-size suffix parsing) in the global context.
size_t *shi_flag_size(const char *name, uint64_t def, const char *desc);
/// Defines a size flag in the global context, bound to `var'.
void shi_flag_size_var(size_t *var, const char *name, uint64_t def, const char *desc);

/// Defines a string flag in the global context and returns a pointer to its value.
char **shi_flag_str(const char *name, const char *def, const char *desc);
/// Defines a string flag in the global context, bound to `var'.
void shi_flag_str_var(char **var, const char *name, const char *def, const char *desc);

/// Defines a repeatable string-list flag (immutable items) in the global context.
Shi_Flag_List *shi_flag_list(const char *name, const char *desc);
/// Defines a repeatable string-list flag in the global context, bound to `var'.
void shi_flag_list_var(Shi_Flag_List *var, const char *name, const char *desc);

/// Defines a repeatable string-list flag (mutable items) in the global context.
Shi_Flag_List_Mut *shi_flag_list_mut(const char *name, const char *desc);
/// Defines a mutable string-list flag in the global context, bound to `var'.
void shi_flag_list_mut_var(Shi_Flag_List_Mut *var, const char *name, const char *desc);

/// Parses `argv' against the flags defined in the global context.
bool shi_flag_parse(int argc, char **argv);
/// Returns the count of unparsed arguments left after shi_flag_parse().
int shi_flag_rest_argc(void);
/// Returns the unparsed arguments left after shi_flag_parse().
char **shi_flag_rest_argv(void);
/// Returns the program name consumed by shi_flag_parse().
const char *shi_flag_program_name(void);
/// Prints the last parse error of the global context to `stream'.
void shi_flag_print_error(FILE *stream);
/// Prints the usage of all flags defined in the global context to `stream'.
void shi_flag_print_options(FILE *stream);

/// Allocates a new opaque flag context.
void *shi_flag_c_new(const char *program_name);
/// Frees an opaque flag context allocated by shi_flag_c_new().
void shi_flag_c_free(void *c);
/// Returns the name of the flag bound to `val' in context `c'.
char *shi_flag_c_name(void *c, void *val);
/// Binds a short alias (e.g. "b" for "-b") to the flag bound to `val' in context `c'.
void shi_flag_c_set_short(void *c, void *val, const char *short_name);

/// Defines a bool flag in context `c' and returns a pointer to its value.
bool *shi_flag_c_bool(void *c, const char *name, bool def, const char *desc);
/// Defines a bool flag in context `c', bound to `var'.
void shi_flag_c_bool_var(void *c, bool *var, const char *name, bool def, const char *desc);

/// Defines a float flag in context `c' and returns a pointer to its value.
float *shi_flag_c_float(void *c, const char *name, float def, const char *desc);
/// Defines a float flag in context `c', bound to `var'.
void shi_flag_c_float_var(void *c, float *var, const char *name, float def, const char *desc);

/// Defines a double flag in context `c' and returns a pointer to its value.
double *shi_flag_c_double(void *c, const char *name, double def, const char *desc);
/// Defines a double flag in context `c', bound to `var'.
void shi_flag_c_double_var(void *c, double *var, const char *name, double def, const char *desc);

/// Defines a uint64_t flag in context `c' and returns a pointer to its value.
uint64_t *shi_flag_c_uint64(void *c, const char *name, uint64_t def, const char *desc);
/// Defines a uint64_t flag in context `c', bound to `var'.
void shi_flag_c_uint64_var(void *c, uint64_t *var, const char *name, uint64_t def, const char *desc);

/// Defines a size flag in context `c' and returns a pointer to its value.
size_t *shi_flag_c_size(void *c, const char *name, uint64_t def, const char *desc);
/// Defines a size flag in context `c', bound to `var'.
void shi_flag_c_size_var(void *c, size_t *var, const char *name, uint64_t def, const char *desc);

/// Defines a string flag in context `c' and returns a pointer to its value.
char **shi_flag_c_str(void *c, const char *name, const char *def, const char *desc);
/// Defines a string flag in context `c', bound to `var'.
void shi_flag_c_str_var(void *c, char **var, const char *name, const char *def, const char *desc);

/// Defines a string-list flag in context `c'.
Shi_Flag_List *shi_flag_c_list(void *c, const char *name, const char *desc);
/// Defines a string-list flag in context `c', bound to `var'.
void shi_flag_c_list_var(void *c, Shi_Flag_List *var, const char *name, const char *desc);

/// Defines a mutable string-list flag in context `c'.
Shi_Flag_List_Mut *shi_flag_c_list_mut(void *c, const char *name, const char *desc);
/// Defines a mutable string-list flag in context `c', bound to `var'.
void shi_flag_c_list_mut_var(void *c, Shi_Flag_List_Mut *var, const char *name, const char *desc);

/// Parses `argv' against the flags defined in context `c'.
bool shi_flag_c_parse(void *c, int argc, char **argv);
/// Returns the count of unparsed arguments left in context `c'.
int shi_flag_c_rest_argc(void *c);
/// Returns the unparsed arguments left in context `c'.
char **shi_flag_c_rest_argv(void *c);
/// Returns the program name consumed by shi_flag_c_parse() for context `c'.
const char *shi_flag_c_program_name(void *c);
/// Explicitly sets the program name for context `c'.
void shi_flag_c_set_program_name(void *c, const char *program_name);
/// Prints the last parse error of context `c' to `stream'.
void shi_flag_c_print_error(void *c, FILE *stream);
/// Prints the usage of all flags defined in context `c' to `stream'.
void shi_flag_c_print_options(void *c, FILE *stream);

#endif // SHI_FLAGS_H

// =================
// TESTING & EXAMPLE
// =================
#ifdef SHI_TEST

#include <stdio.h>

#define SHI_FLAGS_IMPLEMENTATION

int main(int argc, char **argv) {
  bool *verbose = shi_flag_bool("verbose", false, "enable verbose output");
  char **name = shi_flag_str("name", "world", "name to greet");
  uint64_t *count = shi_flag_uint64("count", 1, "number of times to greet");
  size_t *bufsize = shi_flag_size("bufsize", 1024, "buffer size");
  shi_flag_set_short(bufsize, "b");

  if (!shi_flag_parse(argc, argv)) {
    shi_flag_print_error(stderr);
    fprintf(stderr, "Usage: %s [OPTIONS]\n", shi_flag_program_name());
    shi_flag_print_options(stderr);
    return 1;
  }

  for (uint64_t i = 0; i < *count; ++i) {
    printf("Hello, %s! (verbose=%d, bufsize=%zu)\n", *name, *verbose, *bufsize);
  }

  return 0;
}

#endif // SHI_TEST

#ifdef SHI_FLAGS_IMPLEMENTATION

typedef enum {
  SHI_FLAG_BOOL = 0,
  SHI_FLAG_UINT64,
  SHI_FLAG_DOUBLE,
  SHI_FLAG_FLOAT,
  SHI_FLAG_SIZE,
  SHI_FLAG_STR,
  SHI_FLAG_LIST,
  SHI_FLAG_LIST_MUT,
  SHI_COUNT_FLAG_TYPES,
} Shi_Flag_Type;

static_assert(SHI_COUNT_FLAG_TYPES == 8, "Exhaustive Shi_Flag_Value definition");
typedef union {
  char *as_str;
  uint64_t as_uint64;
  double as_double;
  float as_float;
  bool as_bool;
  size_t as_size;
  Shi_Flag_List as_list;
  Shi_Flag_List_Mut as_list_mut;
} Shi_Flag_Value;

typedef enum {
  SHI_FLAG_NO_ERROR = 0,
  SHI_FLAG_ERROR_UNKNOWN,
  SHI_FLAG_ERROR_NO_VALUE,
  SHI_FLAG_ERROR_INVALID_NUMBER,
  SHI_FLAG_ERROR_INTEGER_OVERFLOW,
  SHI_FLAG_ERROR_FLOAT_OVERFLOW,
  SHI_FLAG_ERROR_DOUBLE_OVERFLOW,
  SHI_FLAG_ERROR_INVALID_SIZE_SUFFIX,
  SHI_COUNT_FLAG_ERRORS,
} Shi_Flag_Error;

typedef struct {
  Shi_Flag_Type type;
  char *name;
  char *short_name;
  char *desc;

  Shi_Flag_Value val;
  void *ref;

  Shi_Flag_Value def;
} Shi_Flag;

typedef struct {
  Shi_Flag flags[SHI_FLAGS_CAP];
  size_t flags_count;

  Shi_Flag_Error flag_error;
  char *flag_error_name;

  const char *program_name;

  int rest_argc;
  char **rest_argv;
} Shi_Flag_Context;

static Shi_Flag *shi_flag__new_flag(Shi_Flag_Context *c, Shi_Flag_Type type, const char *name, const char *desc);
static void *shi_flag__get_ref(Shi_Flag *flag);
static bool shi_flag__size_calculate_multiplier(char *endptr, unsigned long long int *result);
static char *shi_flag_shift_args(int *argc, char ***argv);

static Shi_Flag_Context shi_flag_global_context;

void *shi_flag_c_new(const char *program_name) {
  Shi_Flag_Context *fc = (Shi_Flag_Context *)malloc(sizeof(*fc));
  memset(fc, 0, sizeof(*fc));
  fc->program_name = program_name;
  return fc;
}

void shi_flag_c_free(void *c) { free(c); }

static Shi_Flag *shi_flag__new_flag(Shi_Flag_Context *c, Shi_Flag_Type type, const char *name, const char *desc) {
  assert(c->flags_count < SHI_FLAGS_CAP);
  Shi_Flag *flag = &c->flags[c->flags_count++];
  memset(flag, 0, sizeof(*flag));
  flag->type = type;
  flag->name = (char *)name;
  flag->desc = (char *)desc;
  return flag;
}

static void *shi_flag__get_ref(Shi_Flag *flag) {
  if (flag->ref) {
    return flag->ref;
  }
  return &flag->val;
}

char *shi_flag_name(void *val) { return shi_flag_c_name(&shi_flag_global_context, val); }

char *shi_flag_c_name(void *c, void *val) {
  Shi_Flag_Context *fc = (Shi_Flag_Context *)c;

  for (size_t i = 0; i < fc->flags_count; ++i) {
    Shi_Flag *flag = &fc->flags[i];
    if (shi_flag__get_ref(flag) == val) {
      return flag->name;
    }
  }

  return NULL;
}

void shi_flag_set_short(void *val, const char *short_name) { shi_flag_c_set_short(&shi_flag_global_context, val, short_name); }

void shi_flag_c_set_short(void *c, void *val, const char *short_name) {
  Shi_Flag_Context *fc = (Shi_Flag_Context *)c;

  for (size_t i = 0; i < fc->flags_count; ++i) {
    Shi_Flag *flag = &fc->flags[i];
    if (shi_flag__get_ref(flag) == val) {
      flag->short_name = (char *)short_name;
      return;
    }
  }
}

bool *shi_flag_c_bool(void *c, const char *name, bool def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_BOOL, name, desc);
  flag->def.as_bool = def;
  flag->val.as_bool = def;
  return &flag->val.as_bool;
}

void shi_flag_c_bool_var(void *c, bool *var, const char *name, bool def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_BOOL, name, desc);
  flag->def.as_bool = def;
  flag->ref = var;
  *var = def;
}

bool *shi_flag_bool(const char *name, bool def, const char *desc) {
  return shi_flag_c_bool(&shi_flag_global_context, name, def, desc);
}

void shi_flag_bool_var(bool *var, const char *name, bool def, const char *desc) {
  shi_flag_c_bool_var(&shi_flag_global_context, var, name, def, desc);
}

float *shi_flag_c_float(void *c, const char *name, float def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_FLOAT, name, desc);
  flag->def.as_float = def;
  flag->val.as_float = def;
  return &flag->val.as_float;
}

void shi_flag_c_float_var(void *c, float *var, const char *name, float def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_FLOAT, name, desc);
  flag->def.as_float = def;
  flag->ref = var;
  *var = def;
}

double *shi_flag_c_double(void *c, const char *name, double def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_DOUBLE, name, desc);
  flag->def.as_double = def;
  flag->val.as_double = def;
  return &flag->val.as_double;
}

void shi_flag_c_double_var(void *c, double *var, const char *name, double def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_DOUBLE, name, desc);
  flag->def.as_double = def;
  flag->ref = var;
  *var = def;
}

float *shi_flag_float(const char *name, float def, const char *desc) {
  return shi_flag_c_float(&shi_flag_global_context, name, def, desc);
}

void shi_flag_float_var(float *var, const char *name, float def, const char *desc) {
  shi_flag_c_float_var(&shi_flag_global_context, var, name, def, desc);
}

double *shi_flag_double(const char *name, double def, const char *desc) {
  return shi_flag_c_double(&shi_flag_global_context, name, def, desc);
}

void shi_flag_double_var(double *var, const char *name, double def, const char *desc) {
  shi_flag_c_double_var(&shi_flag_global_context, var, name, def, desc);
}

uint64_t *shi_flag_c_uint64(void *c, const char *name, uint64_t def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_UINT64, name, desc);
  flag->val.as_uint64 = def;
  flag->def.as_uint64 = def;
  return &flag->val.as_uint64;
}

void shi_flag_c_uint64_var(void *c, uint64_t *var, const char *name, uint64_t def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_UINT64, name, desc);
  flag->def.as_uint64 = def;
  flag->ref = var;
  *var = def;
}

uint64_t *shi_flag_uint64(const char *name, uint64_t def, const char *desc) {
  return shi_flag_c_uint64(&shi_flag_global_context, name, def, desc);
}

void shi_flag_uint64_var(uint64_t *var, const char *name, uint64_t def, const char *desc) {
  shi_flag_c_uint64_var(&shi_flag_global_context, var, name, def, desc);
}

size_t *shi_flag_c_size(void *c, const char *name, uint64_t def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_SIZE, name, desc);
  flag->val.as_size = def;
  flag->def.as_size = def;
  return &flag->val.as_size;
}

void shi_flag_c_size_var(void *c, size_t *var, const char *name, uint64_t def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_SIZE, name, desc);
  flag->ref = var;
  flag->def.as_size = def;
  *var = def;
}

size_t *shi_flag_size(const char *name, uint64_t def, const char *desc) {
  return shi_flag_c_size(&shi_flag_global_context, name, def, desc);
}

void shi_flag_size_var(size_t *var, const char *name, uint64_t def, const char *desc) {
  shi_flag_c_size_var(&shi_flag_global_context, var, name, def, desc);
}

char **shi_flag_c_str(void *c, const char *name, const char *def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_STR, name, desc);
  flag->val.as_str = (char *)def;
  flag->def.as_str = (char *)def;
  return &flag->val.as_str;
}

void shi_flag_c_str_var(void *c, char **var, const char *name, const char *def, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_STR, name, desc);
  flag->ref = var;
  flag->def.as_str = (char *)def;
  *var = (char *)def;
}

char **shi_flag_str(const char *name, const char *def, const char *desc) {
  return shi_flag_c_str(&shi_flag_global_context, name, def, desc);
}

void shi_flag_str_var(char **var, const char *name, const char *def, const char *desc) {
  shi_flag_c_str_var(&shi_flag_global_context, var, name, def, desc);
}

Shi_Flag_List *shi_flag_c_list(void *c, const char *name, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_LIST, name, desc);
  return &flag->val.as_list;
}

void shi_flag_c_list_var(void *c, Shi_Flag_List *var, const char *name, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_LIST, name, desc);
  flag->ref = var;
}

Shi_Flag_List_Mut *shi_flag_c_list_mut(void *c, const char *name, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_LIST_MUT, name, desc);
  return &flag->val.as_list_mut;
}

void shi_flag_c_list_mut_var(void *c, Shi_Flag_List_Mut *var, const char *name, const char *desc) {
  Shi_Flag *flag = shi_flag__new_flag((Shi_Flag_Context *)c, SHI_FLAG_LIST_MUT, name, desc);
  flag->ref = var;
}

Shi_Flag_List *shi_flag_list(const char *name, const char *desc) {
  return shi_flag_c_list(&shi_flag_global_context, name, desc);
}

void shi_flag_list_var(Shi_Flag_List *var, const char *name, const char *desc) {
  shi_flag_c_list_var(&shi_flag_global_context, var, name, desc);
}

Shi_Flag_List_Mut *shi_flag_list_mut(const char *name, const char *desc) {
  return shi_flag_c_list_mut(&shi_flag_global_context, name, desc);
}

void shi_flag_list_mut_var(Shi_Flag_List_Mut *var, const char *name, const char *desc) {
  shi_flag_c_list_mut_var(&shi_flag_global_context, var, name, desc);
}

static char *shi_flag_shift_args(int *argc, char ***argv) {
  assert(*argc > 0);
  char *result = **argv;
  *argv += 1;
  *argc -= 1;
  return result;
}

int shi_flag_rest_argc(void) { return shi_flag_global_context.rest_argc; }

int shi_flag_c_rest_argc(void *c) { return ((Shi_Flag_Context *)c)->rest_argc; }

char **shi_flag_rest_argv(void) { return shi_flag_global_context.rest_argv; }

char **shi_flag_c_rest_argv(void *c) { return ((Shi_Flag_Context *)c)->rest_argv; }

const char *shi_flag_program_name(void) { return shi_flag_global_context.program_name; }

const char *shi_flag_c_program_name(void *c) { return ((Shi_Flag_Context *)c)->program_name; }

void shi_flag_c_set_program_name(void *c, const char *program_name) { ((Shi_Flag_Context *)c)->program_name = program_name; }

static bool shi_flag__size_calculate_multiplier(char *endptr, unsigned long long int *result) {
  if (strcmp(endptr, "c") == 0) {
    (*result) *= 1ULL;
  } else if (strcmp(endptr, "w") == 0) {
    (*result) *= 2ULL;
  } else if (strcmp(endptr, "b") == 0) {
    (*result) *= 512ULL;
  } else if (strcmp(endptr, "kB") == 0) {
    (*result) *= 1000ULL;
  } else if (strcmp(endptr, "K") == 0 || strcmp(endptr, "KiB") == 0) {
    (*result) *= 1024ULL;
  } else if (strcmp(endptr, "MB") == 0) {
    (*result) *= 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "M") == 0 || strcmp(endptr, "MiB") == 0 || strcmp(endptr, "xM") == 0) {
    (*result) *= 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "GB") == 0) {
    (*result) *= 1000ULL * 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "G") == 0 || strcmp(endptr, "GiB") == 0) {
    (*result) *= 1024ULL * 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "TB") == 0) {
    (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "T") == 0 || strcmp(endptr, "TiB") == 0) {
    (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "PB") == 0) {
    (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "P") == 0 || strcmp(endptr, "PiB") == 0) {
    (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "EB") == 0) {
    (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "E") == 0 || strcmp(endptr, "EiB") == 0) {
    (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "ZB") == 0) {
    (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "Z") == 0 || strcmp(endptr, "ZiB") == 0) {
    (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "YB") == 0) {
    (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
  } else if (strcmp(endptr, "Y") == 0 || strcmp(endptr, "YiB") == 0) {
    (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
  } else if (strcmp(endptr, "") != 0) {
    return false;
  }
  return true;
}

bool shi_flag_c_parse(void *c, int argc, char **argv) {
  Shi_Flag_Context *fc = (Shi_Flag_Context *)c;

  if (fc->program_name == NULL) {
    fc->program_name = shi_flag_shift_args(&argc, &argv);
  }

  while (argc > 0) {
    char *flag = shi_flag_shift_args(&argc, &argv);

    if (*flag != '-') {
      fc->rest_argc = argc + 1;
      fc->rest_argv = argv - 1;
      return true;
    }

    if (strcmp(flag, "--") == 0) {
#ifdef SHI_FLAG_PUSH_DASH_DASH_BACK
      fc->rest_argc = argc + 1;
      fc->rest_argv = argv - 1;
#else
      fc->rest_argc = argc;
      fc->rest_argv = argv;
#endif // SHI_FLAG_PUSH_DASH_DASH_BACK
      return true;
    }

    flag += 1;

    bool ignore = false;
    if (*flag == '/') {
      ignore = true;
      flag += 1;
    }

    char *equals = strchr(flag, '=');
    if (equals != NULL) {
      *equals = '\0';
      equals += 1;
    }

    bool found = false;
    for (size_t i = 0; i < fc->flags_count; ++i) {
      if (strcmp(fc->flags[i].name, flag) == 0 || (fc->flags[i].short_name && strcmp(fc->flags[i].short_name, flag) == 0)) {
        static_assert(SHI_COUNT_FLAG_TYPES == 8, "Exhaustive flag type parsing");
        switch (fc->flags[i].type) {
        case SHI_FLAG_LIST: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }

          if (!ignore) {
            Shi_Flag_List *list = (Shi_Flag_List *)shi_flag__get_ref(&fc->flags[i]);
            shi_flag_list_append(const char *, list, arg);
          }
        } break;

        case SHI_FLAG_LIST_MUT: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }

          if (!ignore) {
            Shi_Flag_List_Mut *list = (Shi_Flag_List_Mut *)shi_flag__get_ref(&fc->flags[i]);
            shi_flag_list_append(char *, list, arg);
          }
        } break;

        case SHI_FLAG_BOOL: {
          if (!ignore) {
            *(bool *)shi_flag__get_ref(&fc->flags[i]) = true;
          }
        } break;

        case SHI_FLAG_STR: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }

          if (!ignore) {
            *(char **)shi_flag__get_ref(&fc->flags[i]) = arg;
          }
        } break;

        case SHI_FLAG_UINT64: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }

          static_assert(sizeof(unsigned long long int) == sizeof(uint64_t),
                        "Platform requires unsigned long long int and uint64_t to be the same size");
          char *endptr;
          unsigned long long int result = strtoull(arg, &endptr, 10);

          if (*endptr != '\0') {
            fc->flag_error = SHI_FLAG_ERROR_INVALID_NUMBER;
            fc->flag_error_name = flag;
            return false;
          }

          if (result == ULLONG_MAX && errno == ERANGE) {
            fc->flag_error = SHI_FLAG_ERROR_INTEGER_OVERFLOW;
            fc->flag_error_name = flag;
            return false;
          }

          if (!ignore) {
            *(uint64_t *)shi_flag__get_ref(&fc->flags[i]) = result;
          }
        } break;

        case SHI_FLAG_FLOAT: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }
          char *endptr;
          float result = strtof(arg, &endptr);

          if (*endptr != '\0') {
            fc->flag_error = SHI_FLAG_ERROR_INVALID_NUMBER;
            fc->flag_error_name = flag;
            return false;
          }

          if (result == FLT_MAX && errno == ERANGE) {
            fc->flag_error = SHI_FLAG_ERROR_FLOAT_OVERFLOW;
            fc->flag_error_name = flag;
            return false;
          }

          if (!ignore) {
            *(float *)shi_flag__get_ref(&fc->flags[i]) = result;
          }
        } break;

        case SHI_FLAG_DOUBLE: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }
          char *endptr;
          double result = strtod(arg, &endptr);

          if (*endptr != '\0') {
            fc->flag_error = SHI_FLAG_ERROR_INVALID_NUMBER;
            fc->flag_error_name = flag;
            return false;
          }

          if (result == DBL_MAX && errno == ERANGE) {
            fc->flag_error = SHI_FLAG_ERROR_DOUBLE_OVERFLOW;
            fc->flag_error_name = flag;
            return false;
          }

          if (!ignore) {
            *(double *)shi_flag__get_ref(&fc->flags[i]) = result;
          }
        } break;

        case SHI_FLAG_SIZE: {
          char *arg;
          if (equals == NULL) {
            if (argc == 0) {
              fc->flag_error = SHI_FLAG_ERROR_NO_VALUE;
              fc->flag_error_name = flag;
              return false;
            }
            arg = shi_flag_shift_args(&argc, &argv);
          } else {
            arg = equals;
          }

          static_assert(sizeof(unsigned long long int) == sizeof(size_t),
                        "Platform requires unsigned long long int and size_t to be the same size");
          char *endptr;
          unsigned long long int result = strtoull(arg, &endptr, 10);

          if (!shi_flag__size_calculate_multiplier(endptr, &result)) {
            fc->flag_error = SHI_FLAG_ERROR_INVALID_SIZE_SUFFIX;
            fc->flag_error_name = flag;
            return false;
          }

          if (result == ULLONG_MAX && errno == ERANGE) {
            fc->flag_error = SHI_FLAG_ERROR_INTEGER_OVERFLOW;
            fc->flag_error_name = flag;
            return false;
          }

          if (!ignore) {
            *(size_t *)shi_flag__get_ref(&fc->flags[i]) = result;
          }
        } break;

        case SHI_COUNT_FLAG_TYPES:
        default: {
          assert(0 && "unreachable");
          exit(69);
        }
        }

        found = true;
      }
    }

    if (!found) {
      fc->flag_error = SHI_FLAG_ERROR_UNKNOWN;
      fc->flag_error_name = flag;
      return false;
    }
  }

  fc->rest_argc = argc;
  fc->rest_argv = argv;
  return true;
}

bool shi_flag_parse(int argc, char **argv) { return shi_flag_c_parse(&shi_flag_global_context, argc, argv); }

void shi_flag_c_print_options(void *c, FILE *stream) {
  Shi_Flag_Context *fc = (Shi_Flag_Context *)c;
  for (size_t i = 0; i < fc->flags_count; ++i) {
    Shi_Flag *flag = &fc->flags[i];
    char label[128];
    if (flag->short_name) {
      snprintf(label, sizeof(label), "%s, -%s", flag->name, flag->short_name);
    } else {
      snprintf(label, sizeof(label), "%s", flag->name);
    }

    static_assert(SHI_COUNT_FLAG_TYPES == 8, "Exhaustive flag type defaults printing");
    switch (fc->flags[i].type) {
    case SHI_FLAG_LIST_MUT:
    case SHI_FLAG_LIST:
      fprintf(stream, "    -%s <str> ... -%s <str> ...\n", label, label);
      fprintf(stream, "        %s\n", flag->desc);
      break;
    case SHI_FLAG_BOOL:
      fprintf(stream, "    -%s\n", label);
      fprintf(stream, "        %s\n", flag->desc);
      if (flag->def.as_bool) {
        fprintf(stream, "        Default: %s\n", flag->def.as_bool ? "true" : "false");
      }
      break;
    case SHI_FLAG_UINT64:
      fprintf(stream, "    -%s <int>\n", label);
      fprintf(stream, "        %s\n", flag->desc);
      fprintf(stream, "        Default: %" PRIu64 "\n", flag->def.as_uint64);
      break;
    case SHI_FLAG_FLOAT:
      fprintf(stream, "    -%s <float>\n", label);
      fprintf(stream, "        %s\n", flag->desc);
      fprintf(stream, "        Default: %f\n", flag->def.as_float);
      break;
    case SHI_FLAG_DOUBLE:
      fprintf(stream, "    -%s <double>\n", label);
      fprintf(stream, "        %s\n", flag->desc);
      fprintf(stream, "        Default: %lf\n", flag->def.as_double);
      break;
    case SHI_FLAG_SIZE:
      fprintf(stream, "    -%s <int>\n", label);
      fprintf(stream, "        %s\n", flag->desc);
      fprintf(stream, "        Default: %zu\n", flag->def.as_size);
      break;
    case SHI_FLAG_STR:
      fprintf(stream, "    -%s <str>\n", label);
      fprintf(stream, "        %s\n", flag->desc);
      if (flag->def.as_str) {
        fprintf(stream, "        Default: %s\n", flag->def.as_str);
      }
      break;
    case SHI_COUNT_FLAG_TYPES:
    default: assert(0 && "unreachable"); exit(69);
    }
  }
}

void shi_flag_print_options(FILE *stream) { shi_flag_c_print_options(&shi_flag_global_context, stream); }

void shi_flag_c_print_error(void *c, FILE *stream) {
  Shi_Flag_Context *fc = (Shi_Flag_Context *)c;
  static_assert(SHI_COUNT_FLAG_ERRORS == 8, "Exhaustive flag error printing");
  switch (fc->flag_error) {
  case SHI_FLAG_NO_ERROR: fprintf(stream, "Operation Failed Successfully!"); break;
  case SHI_FLAG_ERROR_UNKNOWN: fprintf(stream, "ERROR: -%s: unknown flag\n", fc->flag_error_name); break;
  case SHI_FLAG_ERROR_NO_VALUE: fprintf(stream, "ERROR: -%s: no value provided\n", fc->flag_error_name); break;
  case SHI_FLAG_ERROR_INVALID_NUMBER: fprintf(stream, "ERROR: -%s: invalid number\n", fc->flag_error_name); break;
  case SHI_FLAG_ERROR_INTEGER_OVERFLOW: fprintf(stream, "ERROR: -%s: integer overflow\n", fc->flag_error_name); break;
  case SHI_FLAG_ERROR_FLOAT_OVERFLOW: fprintf(stream, "ERROR: -%s: float overflow\n", fc->flag_error_name); break;
  case SHI_FLAG_ERROR_DOUBLE_OVERFLOW: fprintf(stream, "ERROR: -%s: double overflow\n", fc->flag_error_name); break;
  case SHI_FLAG_ERROR_INVALID_SIZE_SUFFIX: fprintf(stream, "ERROR: -%s: invalid size suffix\n", fc->flag_error_name); break;
  case SHI_COUNT_FLAG_ERRORS:
  default: assert(0 && "unreachable"); exit(69);
  }
}

void shi_flag_print_error(FILE *stream) { shi_flag_c_print_error(&shi_flag_global_context, stream); }

#endif // SHI_FLAGS_IMPLEMENTATION

/*
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION:
 *
 * License : https://github.com/tsoding/flag.h?tab=MIT-1-ov-file
 */