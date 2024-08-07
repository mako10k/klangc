#pragma once

#include "klangc.h"

typedef enum {
  KLANGC_PTYPE_SYMBOL,
  KLANGC_PTYPE_REF,
  KLANGC_PTYPE_APPL,
  KLANGC_PTYPE_AS,
  KLANGC_PTYPE_INT,
  KLANGC_PTYPE_STRING,
} klangc_pat_type_t;

typedef int (*klangc_pat_foreach_ref_func_t)(klangc_pat_ref_t *ref, void *data);

klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_t **ppat);

void klangc_pat_print(klangc_output_t *output, int prec, klangc_pat_t *pat);

int klangc_pat_foreach_ref(klangc_pat_t *pat,
                           klangc_pat_foreach_ref_func_t bind_fn, void *data);

klangc_pat_type_t klangc_pat_get_type(klangc_pat_t *pat);

klangc_symbol_t *klangc_pat_get_symbol(klangc_pat_t *pat);