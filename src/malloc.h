#pragma once

#include "klangc.h"

#include <stdio.h>

/**
 * メモリを確保
 * @param size サイズ
 * @return 確保したメモリ
 */
void *klangc_malloc(size_t size);

/**
 * メモリを再確保
 * @param ptr メモリ
 * @param size サイズ
 * @return 確保したメモリ
 */
void *klangc_realloc(void *ptr, size_t size);

/**
 * メモリを解放
 * @param ptr メモリ
 */
void klangc_free(void *ptr);

/**
 * 文字列を複製
 * @param s 文字列
 * @return 複製した文字列
 */
char *klangc_strdup(const char *s);
