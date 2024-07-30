#ifndef __KLANGC_H__
#define __KLANGC_H__

#ifdef HAS_CONFIG_H
#include "config.h"
#endif

#include "klangc_malloc.h"
#include <stdio.h>

/** 定義 */
typedef struct klangc_def klangc_def_t;
/** 値 */
typedef struct klangc_value klangc_value_t;
/** パターン */
typedef struct klangc_pattern klangc_pattern_t;
/** 式 */
typedef struct klangc_expr klangc_expr_t;
/** ハッシュ */
typedef struct klangc_hash klangc_hash_t;

#endif // __KLANGC_H__