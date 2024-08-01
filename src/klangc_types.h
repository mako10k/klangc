#ifndef __KLANGC_TYPES_H__
#define __KLANGC_TYPES_H__

#include "klangc.h"

/** 定義エントリ
 * `<defent> ::= <pattern> "=" <expr> ";";`
 */
typedef struct klangc_def_ent klangc_def_ent_t;

/**
 * 定義
 * `<def> ::= <defent>*;`
 */
typedef struct klangc_def klangc_def_t;

/** パターン
 * `<pattern> ::= <symbol> | <symbol> <pattern>;`
 */
typedef struct klangc_pattern klangc_pattern_t;

/** 式
 * `<expr> ::= <symbol> | <appl> | <int> | <string> | <lambda> | <dict>;`
 */
typedef struct klangc_expr klangc_expr_t;

/**
 * 辞書
 * `<dict> ::= "{" <def> "}";`
 */
typedef struct klangc_dict klangc_dict_t;

/** ハッシュ */
typedef struct klangc_hash klangc_hash_t;
/** 値 */
typedef struct klangc_value klangc_value_t;

#endif // __KLANGC_TYPES_H__