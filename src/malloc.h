#pragma once

#include "klangc.h"

#include <stdio.h>

// *******************************
// Memory.
// *******************************
/**
 * Allocate memory.
 * @param size Size.
 * @return Memory.
 */
void *klangc_malloc(size_t size);

/**
 * Reallocate memory.
 * @param ptr Memory.
 * @param size Size.
 * @return Memory.
 */
void *klangc_realloc(void *ptr, size_t size);

/**
 * Free memory.
 * @param ptr Memory.
 */
void klangc_free(void *ptr);

/**
 * Duplicate a string.
 * @param s String.
 * @return Duplicate.
 */
char *klangc_strdup(const char *s);
