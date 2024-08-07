#include <gc.h>
#include <stdio.h>
#include <stdlib.h>

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