#ifndef __KLANGC_DEF_H__
#define __KLANGC_DEF_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

/**
 * 定義をパースする
 * @param fp ファイルポインタ
 * @return パースした定義
 */
klangc_parse_result_t klangc_def_parse(klangc_input_t *input,
                                       klangc_def_t **pdef);

/**
 * 定義を出力する
 * @param fp ファイルポインタ
 * @param def 定義
 */
void klangc_def_print(klangc_output_t *output, klangc_def_t *def);

/**
 * 定義をバインドする
 */
int klangc_def_bind(klangc_def_t *def, const char *name,
                    klangc_def_ent_t *def_ent);

#endif // __KLANGC_DEF_H__