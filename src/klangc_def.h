#ifndef __KLANGC_DEF_H__
#define __KLANGC_DEF_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_types.h"

/**
 * 定義をパースする
 * @param fp ファイルポインタ
 * @return パースした定義
 */
klangc_hash_t *klangc_def_parse(klangc_input_t *input);

/**
 * 定義を出力する
 * @param fp ファイルポインタ
 * @param defs 定義
 */
void klangc_def_print(klangc_output_t *output, klangc_hash_t *defs);

/**
 * 定義をバインドする
 */
int klangc_def_bind(klangc_hash_t *defs, const char *name, klangc_def_t *def);

#endif // __KLANGC_DEF_H__