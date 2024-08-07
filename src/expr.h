#pragma once

#include "klangc.h"

typedef enum klangc_expr_type {
  KLANGC_ETYPE_SYMBOL,
  KLANGC_ETYPE_REF,
  KLANGC_ETYPE_APPL,
  KLANGC_ETYPE_INT,
  KLANGC_ETYPE_STRING,
  KLANGC_ETYPE_LAMBDA,
  KLANGC_ETYPE_CLOSURE,
} klangc_expr_type_t;

klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_closure_t *enclosed_by,
                                        klangc_expr_t **pexpr);

void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr);

int klangc_expr_bind(klangc_closure_t *closure, klangc_expr_t *expr);

int klangc_expr_bind_ent(klangc_closure_t *closure, klangc_closure_ent_t *ent,
                         void *data);

int klangc_expr_check_unbound(klangc_output_t *output,
                              klangc_closure_t *closure, klangc_expr_t *expr);

klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr);

klangc_expr_type_t klangc_expr_get_type(klangc_expr_t *expr);

klangc_symbol_t *klangc_expr_get_symbol(klangc_expr_t *expr);

klangc_expr_ref_t *klangc_expr_get_ref(klangc_expr_t *expr);

klangc_expr_appl_t *klangc_expr_get_appl(klangc_expr_t *expr);

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *expr);

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *expr);

int klangc_expr_get_int(klangc_expr_t *expr);

const char *klangc_expr_get_string(klangc_expr_t *expr);

klangc_lambda_t *klangc_expr_get_lambda(klangc_expr_t *expr);

klangc_closure_t *klangc_expr_get_closure(klangc_expr_t *expr);