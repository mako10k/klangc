#pragma once

#include "klangc.h"

typedef enum {
  KLANGC_PTYPE_REF,
  KLANGC_PTYPE_ALGE,
  KLANGC_PTYPE_AS,
  KLANGC_PTYPE_INT,
  KLANGC_PTYPE_STRING,
} klangc_pat_type_t;

typedef enum {
  KLANGC_PAT_PARSE_NORMAL = 0,
  KLANGC_PAT_PARSE_NOARG = 1,
} klangc_pat_parse_opt_t;

typedef int (*klangc_pat_foreach_ref_func_t)(klangc_pat_ref_t *ref, void *data);

klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_parse_opt_t noarg,
                                       klangc_pat_t **ppat);

void klangc_pat_print(klangc_output_t *output, int prec, klangc_pat_t *pat);

int klangc_pat_foreach_ref(klangc_pat_t *pat,
                           klangc_pat_foreach_ref_func_t bind_fn, void *data);

klangc_pat_type_t klangc_pat_get_type(klangc_pat_t *pat);

klangc_pat_alge_t *klangc_pat_get_alge(klangc_pat_t *pat);

/**
 * パターンに含まれる参照を抽出して env に登録する
 * @param env 環境
 * @param pat パターン
 * @param target 参照先
 * @return 結果 KLANGC_BIND_OK: 成功, KLANGC_BIND_ERROR: 失敗
 */
klangc_bind_result_t klangc_pat_bind(klangc_expr_env_t *env, klangc_pat_t *pat,
                                     klangc_expr_ref_target_t *target);