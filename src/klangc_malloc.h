#ifndef __KLANGC_MALLOC_H__
#define __KLANGC_MALLOC_H__

#include <stdio.h>

void *klangc_malloc(size_t size);
void *klangc_realloc(void *ptr, size_t size);
void klangc_free(void *ptr);
char *klangc_strdup(const char *s);

#endif // __KLANGC_MALLOC_H__
