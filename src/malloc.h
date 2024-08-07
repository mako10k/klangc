#pragma once

#include "klangc.h"

#include <stdio.h>

void *klangc_malloc(size_t size);
void *klangc_realloc(void *ptr, size_t size);
void klangc_free(void *ptr);
char *klangc_strdup(const char *s);
