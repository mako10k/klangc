#pragma once

#include "klangc.h"


typedef enum {
  KLANGC_PTYPE_SYMBOL,
  KLANGC_PTYPE_REF,
  KLANGC_PTYPE_APPL,
  KLANGC_PTYPE_AS,
  KLANGC_PTYPE_INT,
  KLANGC_PTYPE_STRING,
} klangc_pattern_type_t;

typedef int (*klangc_pattern_foreach_ref_func_t)(klangc_pattern_ref_t *ref, void *data);

klangc_parse_result_t klangc_pattern_parse(klangc_input_t *input,
                                           klangc_pattern_t **ppattern);

void klangc_pattern_print(klangc_output_t *output, int prec,
                          klangc_pattern_t *pattern);

int klangc_pattern_foreach_ref(klangc_pattern_t *pat,
                               klangc_pattern_foreach_ref_func_t bind_fn,
                               void *data);

klangc_pattern_type_t klangc_pattern_get_type(klangc_pattern_t *pattern);

klangc_symbol_t *klangc_pattern_get_symbol(klangc_pattern_t *pattern);