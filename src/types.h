#pragma once

#include "klangc.h"
#include <stdio.h>

/** bind要素 */
typedef struct klangc_expr_closure_bind klangc_bind_t;

/** クロージャ */
typedef struct klangc_expr_closure klangc_expr_closure_t;

/** クロージャのエントリ */
typedef struct klangc_expr_closure_entry klangc_expr_closure_entry_t;

/** パターン */
typedef struct klangc_pat klangc_pat_t;

/** 参照（パターン） */
typedef struct klangc_pat_ref klangc_pat_ref_t;

/** 代数的データ（パターン） */
typedef struct klangc_pat_alge klangc_pat_alge_t;

/** ASパターン（パターン） */
typedef struct klangc_pat_as klangc_pat_as_t;

/** 式 */
typedef struct klangc_expr klangc_expr_t;

/** 環境（式） */
typedef struct klangc_expr_env klangc_expr_env_t;

/** 参照（式） */
typedef struct klangc_expr_ref klangc_expr_ref_t;

/** 参照先（式） */
typedef struct klangc_expr_ref_target klangc_expr_ref_target_t;

/** 適用（式） */
typedef struct klangc_expr_appl klangc_expr_appl_t;

/** ラムダ式（式） */
typedef struct klangc_expr_lambda klangc_expr_lambda_t;

/** ハッシュテーブル */
typedef struct klangc_hash klangc_hash_t;

/** 値 */
typedef struct klangc_value klangc_value_t;

/** 参照（値） */
typedef struct klangc_value_ref klangc_value_ref_t;

/** 適用（値） */
typedef struct klangc_value_appl klangc_value_appl_t;

/** シンボル（値） */
typedef struct klangc_symbol klangc_symbol_t;

/** 参照 */
typedef struct klangc_ref klangc_ref_t;

/** 入力 */
typedef struct klangc_input klangc_input_t;

/** 出力 */
typedef struct klangc_output klangc_output_t;

/** 環境（値） */
typedef struct klangc_env klangc_env_t;

/** 位置情報 */
typedef struct klangc_ipos {
  klangc_input_t *kip_input;
  off_t kip_offset;
  int kip_line;
  int kip_col;
} klangc_ipos_t;

/** パース結果 */
typedef enum klangc_parse_result {
  KLANGC_PARSE_OK,
  KLANGC_PARSE_NOPARSE,
  KLANGC_PARSE_ERROR,
} klangc_parse_result_t;

/** バインド結果 */
typedef enum klangc_bind_result {
  KLANGC_BIND_OK,
  KLANGC_BIND_ERROR,
} klangc_bind_result_t;

/** バインドチェック結果 */
typedef enum klangc_unbound_result {
  KLANGC_UNBOUND_OK,
  KLANGC_UNBOUND_ERROR,
} klangc_unbound_result_t;