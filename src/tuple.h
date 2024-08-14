#pragma once

#include "expr_alge.h"
#include "klangc.h"
#include "pat_alge.h"
#include "str.h"
#include "symbol.h"
#include "types.h"

__attribute_used__ static const klangc_str_t *klangc_tuple_str(void) {
  static const klangc_str_t *tuple_str = NULL;
  if (tuple_str == NULL) {
    tuple_str = klangc_str_new(",", 1);
  }
  return tuple_str;
}

__attribute_used__ static const klangc_symbol_t *klangc_tuple_symbol(void) {
  static const klangc_symbol_t *tuple_sym = NULL;
  if (tuple_sym == NULL) {
    tuple_sym = klangc_symbol_new(klangc_tuple_str());
  }
  return tuple_sym;
}

__attribute_used__ static klangc_pat_alge_t *klangc_unit_pat_alge(void) {
  static klangc_pat_alge_t *unit_pat_alge = NULL;
  if (unit_pat_alge == NULL) {
    unit_pat_alge = klangc_pat_alge_new(klangc_tuple_symbol());
  }
  return unit_pat_alge;
}

__attribute_used__ static klangc_expr_alge_t *klangc_unit_expr_alge(void) {
  static klangc_expr_alge_t *unit_expr_alge = NULL;
  if (unit_expr_alge == NULL) {
    unit_expr_alge = klangc_expr_alge_new(klangc_tuple_symbol());
  }
  return unit_expr_alge;
}

klangc_parse_result_t klangc_pat_parse_tuple(klangc_input_t *input,
                                             klangc_pat_t **ppat);

klangc_parse_result_t klangc_expr_parse_tuple(klangc_input_t *input,
                                              klangc_expr_t **pexpr);

int klangc_pat_is_tuple(klangc_pat_t *pat);

int klangc_expr_is_tuple(klangc_expr_t *expr);

void klangc_pat_print_tuple(klangc_output_t *output, klangc_pat_t *pat);

void klangc_expr_print_tuple(klangc_output_t *output, klangc_expr_t *expr);