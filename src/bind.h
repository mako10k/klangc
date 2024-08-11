#pragma once

#include "klangc.h"

/**
 * 束縛式を作成する
 * @param pat 参照パターン
 * @param expr 被参照式
 * @param next 次の束縛式
 * @param ipos 位置情報
 * @return 作成した束縛式
 */
klangc_bind_t *klangc_bind_new(klangc_pat_t *pat, klangc_expr_t *expr,
                               klangc_bind_t *next, klangc_ipos_t ipos);

/**
 * 束縛式の参照パターンを取得する
 * @param bind 束縛式
 * @return 参照パターン
 */
klangc_pat_t *klangc_bind_get_pat(klangc_bind_t *bind);

/**
 * 束縛式の被参照式を取得する
 * @param bind 束縛式
 * @return 被参照式
 */
klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind);

/**
 * 束縛式の次の束縛式を取得する
 * @param bind 束縛式
 * @return 次の束縛式
 */
klangc_bind_t *klangc_bind_get_next(klangc_bind_t *bind);

/**
 * 束縛式の位置情報を取得する
 * @param bind 束縛式
 * @return 位置情報
 */
klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind);

/**
 * 入力から束縛式をパースする
 * @param input 入力
 * @param pbind パースした束縛式
 * @return パース結果
 */
klangc_parse_result_t klangc_bind_parse(klangc_input_t *input,
                                        klangc_bind_t **pbind);

/**
 * 束縛式を出力する
 * @param output 出力
 * @param bind 束縛式
 */
void klangc_bind_print(klangc_output_t *output, klangc_bind_t *bind);

/**
 * 束縛式をバインドする
 * @param env 環境
 * @param bind 束縛式
 * @return バインド結果 KLANGC_BIND_OK: 成功, KLANGC_BIND_ERROR: 失敗
 */
klangc_bind_result_t klangc_bind_bind(klangc_expr_env_t *env,
                                      klangc_bind_t *bind);
