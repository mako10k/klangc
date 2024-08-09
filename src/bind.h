#pragma once

#include "klangc.h"

/**
 * bind要素を作成する
 * @param pat 左パターン
 * @param expr 右式
 * @param next 次のbind要素
 * @param ipos 位置情報
 * @return 作成したbind要素
 */
klangc_bind_t *klangc_bind_new(klangc_pat_t *pat, klangc_expr_t *expr,
                               klangc_bind_t *next, klangc_ipos_t ipos);

/**
 * bind要素の左パターンを取得する
 * @param bind bind要素
 * @return 左パターン
 */
klangc_pat_t *klangc_bind_get_pat(klangc_bind_t *bind);

/**
 * bind要素の右式を取得する
 * @param bind bind要素
 * @return 右式
 */
klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind);

/**
 * bind要素の次のbind要素を取得する
 * @param bind bind要素
 * @return 次のbind要素
 */
klangc_bind_t *klangc_bind_get_next(klangc_bind_t *bind);

/**
 * bind要素の位置情報を取得する
 * @param bind bind要素
 * @return 位置情報
 */
klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind);

/**
 * 入力からbind要素をパースする
 * @param input 入力
 * @param pbind パースしたbind要素
 * @return パース結果
 */
klangc_parse_result_t klangc_bind_parse(klangc_input_t *input,
                                        klangc_bind_t **pbind);

/**
 * bind要素を出力する
 * @param output 出力
 * @param bind bind要素
 */
void klangc_bind_print(klangc_output_t *output, klangc_bind_t *bind);

/**
 * bind要素をバインドする
 * @param env 環境
 * @param bind bind要素
 * @return バインド結果 KLANGC_BIND_OK: 成功, KLANGC_BIND_ERROR: 失敗
 */
klangc_bind_result_t klangc_bind_bind(klangc_expr_env_t *env,
                                      klangc_bind_t *bind);

/**
 * bind要素の未バインドの参照があるかチェックする
 * @param bind bind要素
 * @return チェック結果 KLANGC_UNBOUND_OK: 成功, KLANGC_UNBOUND_ERROR: 失敗
 */
klangc_unbound_result_t klangc_bind_check_unbound(klangc_bind_t *bind);