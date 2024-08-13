#include "malloc.h"
#include <gc.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#include <string.h>

#undef GC_MALLOC
#undef GC_REALLOC
#undef GC_FREE
#undef GC_STRDUP
#define GC_MALLOC(size) malloc(size)
#define GC_REALLOC(ptr, size) realloc(ptr, size)
#define GC_FREE(ptr) free(ptr)
#define GC_STRDUP(s) strdup(s)
#endif

// *******************************
// Memory.
// *******************************
void *klangc_malloc(size_t size) {
  void *ptr = GC_MALLOC(size);
  if (ptr == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }
  return ptr;
}

void *klangc_realloc(void *ptr, size_t size) {
  ptr = GC_REALLOC(ptr, size);
  if (ptr == NULL) {
    fprintf(stderr, "realloc failed\n");
    exit(1);
  }
  return ptr;
}

void klangc_free(void *ptr) { GC_FREE(ptr); }

char *klangc_strdup(const char *s) {
  char *ptr = GC_STRDUP(s);
  if (ptr == NULL) {
    fprintf(stderr, "strdup failed\n");
    exit(1);
  }
  return ptr;
}