#ifndef __KLANGC_DEF_H__
#define __KLANGC_DEF_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

/**
 * 定義をパースする
 * @param input 入力
 * @param enclosed_by この定義を包む定義
 * @param pdef パースした定義
 * @return パース結果
 */
klangc_parse_result_t klangc_def_parse(klangc_input_t *input,
                                       klangc_closure_t *enclosed_by,
                                       klangc_closure_t **pdef);

/**
 * 定義を表示する
 * @param output 出力
 * @param def 定義
 */
void klangc_def_print(klangc_output_t *output, klangc_closure_t *def);

/**
 * 定義をバインドする
 * @param def 定義
 * @param name 名前
 * @param def_next 次の定義
 * @return バインド結果
 */
int klangc_def_bind(klangc_closure_t *def, const char *name,
                    klangc_def_ent_t *def_ent);

klangc_closure_t *klangc_def_new(klangc_ipos_t ipos,
                                 klangc_closure_t *enclosed_by);

int klangc_def_get(klangc_closure_t *def, const char *name,
                   klangc_def_ent_t **pdef_ent, klangc_closure_t **pdef);

int klangc_def_walk(klangc_closure_t *def,
                    int (*func)(klangc_closure_t *def, klangc_pattern_t *pat,
                                klangc_expr_t *expr, void *data),
                    void *data);

klangc_def_ent_t *klangc_def_ent_new(klangc_pattern_t *pat, klangc_expr_t *expr,
                                     klangc_ipos_t ipos);

#endif // __KLANGC_DEF_H__