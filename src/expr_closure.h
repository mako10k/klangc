#pragma once

#include "klangc.h"

/**
 * クロージャを作成
 * @param expr 式
 * @param bind バインド
 * @return 作成したクロージャ
 */
klangc_expr_closure_t *klangc_expr_closure_new(klangc_expr_t *expr,
                                               klangc_bind_t *bind);

/**
 * クロージャの式を取得する
 * @param closure クロージャ
 * @return 式
 */
klangc_expr_t *klangc_expr_closure_get_expr(klangc_expr_closure_t *closure);

/**
 * クロージャのバインドを取得する
 * @param closure クロージャ
 * @return バインド
 */
klangc_bind_t *klangc_expr_closure_get_bind(klangc_expr_closure_t *closure);

/**
 * クロージャをパースする
 * @param input 入力
 * @param pclosure パースしたクロージャ
 * @return パース結果
 */
klangc_parse_result_t
klangc_expr_closure_parse_nobrace(klangc_input_t *input,
                                  klangc_expr_closure_t **pclosure);

/**
 * クロージャをパースする
 * @param input 入力
 * @param pclosure パースしたクロージャ
 * @return パース結果
 */
klangc_parse_result_t
klangc_expr_closure_parse(klangc_input_t *input,
                          klangc_expr_closure_t **pclosure);

/**
 * クロージャを出力する
 * @param output 出力
 * @param eclosure クロージャ
 */
void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_expr_closure_t *eclosure);

/**
 * クロージャを束縛する
 * @param env 環境
 * @param closure クロージャ
 * @return バインド結果 KLANGC_BIND_OK: 成功, KLANGC_BIND_ERROR: 失敗
 */
klangc_bind_result_t klangc_expr_closure_bind(klangc_expr_env_t *env,
                                              klangc_expr_closure_t *closure);
