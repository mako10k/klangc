#pragma once

#include "klangc.h"
#include "types.h"

/** 式タイプ */
typedef enum klangc_expr_type {
  KLANGC_ETYPE_ALGE,
  KLANGC_ETYPE_REF,
  KLANGC_ETYPE_APPL,
  KLANGC_ETYPE_INT,
  KLANGC_ETYPE_STRING,
  KLANGC_ETYPE_LAMBDA,
  KLANGC_ETYPE_CLOSURE,
} klangc_expr_type_t;

typedef enum {
  KLANGC_EXPR_PARSE_NORMAL = 0,
  KLANGC_EXPR_PARSE_NOAPPL = 1,
} klangc_expr_parse_opt_t;

/**
 * 式をパースする
 * @param input 入力
 * @param pexpr パースした式
 * @return パース結果
 */
klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_expr_parse_opt_t epopt,
                                        klangc_expr_t **pexpr);

/**
 * 式を出力する
 * @param output 出力
 * @param prec 優先順位
 * @param expr 式
 */
void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr);

/**
 * 式に含まれる参照を解決する
 * @param upper 上位環境
 * @param expr 式
 * @return 解決結果
 */
klangc_bind_result_t klangc_expr_bind(klangc_expr_env_t *upper,
                                      klangc_expr_t *expr);

/**
 * 式に含まれる未束縛変数をチェックする
 * @param expr 式
 * @return チェック結果
 */
klangc_unbound_result_t klangc_expr_check_unbound(klangc_expr_t *expr);

/**
 * 式の位置情報を取得する
 * @param expr 式
 * @return 位置情報
 */
klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr);

/**
 * 式タイプを取得する
 * @param expr 式
 * @return 式タイプ
 */
klangc_expr_type_t klangc_expr_get_type(klangc_expr_t *expr);

/**
 * シンボル（式）を取得する
 * @param expr 式
 * @return シンボル（式）
 */
klangc_symbol_t *klangc_expr_get_symbol(klangc_expr_t *expr);

/**
 * 参照（式）を取得する
 * @param expr 式
 * @return 参照（式）
 */
klangc_expr_ref_t *klangc_expr_get_ref(klangc_expr_t *expr);

/**
 * 適用（式）を取得する
 * @param expr 式
 * @return 適用（式）
 */
klangc_expr_appl_t *klangc_expr_get_appl(klangc_expr_t *expr);

/**
 * 整数を取得する
 * @param expr 式
 * @return 整数
 */
int klangc_expr_get_int(klangc_expr_t *expr);

/**
 * 文字列を取得する
 * @param expr 式
 * @return 文字列
 */
const char *klangc_expr_get_string(klangc_expr_t *expr);

/**
 * ラムダ式（式）を取得する
 * @param expr 式
 * @return ラムダ式（式）
 */
klangc_expr_lambda_t *klangc_expr_get_lambda(klangc_expr_t *expr);

/**
 * クロージャ（式）を取得する
 * @param expr 式
 * @return クロージャ（式）
 */
klangc_expr_closure_t *klangc_expr_get_closure(klangc_expr_t *expr);