#pragma once

#include "input.h"
#include "klangc.h"

/**
 * int値をパースする
 * @param input 入力
 * @param pintval int値を格納する変数
 */
klangc_parse_result_t klangc_int_parse(klangc_input_t *input, int *pintval);

/**
 * 文字列をパースする
 * @param input 入力
 * @param pstrval 文字列を格納する変数
 */
klangc_parse_result_t klangc_string_parse(klangc_input_t *input,
                                          const char **pstrval);

/**
 * 期待する文字をパースする
 * @param input 入力
 * @param c 期待する文字
 * @param pc パースした文字を格納する変数
 * @return パース結果
 */
int klangc_expect(klangc_input_t *input, int c, int *pc);
