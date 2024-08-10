#pragma once

#include "expr.h"
#include "klangc.h"

klangc_expr_alge_t *klangc_expr_alge_new(klangc_symbol_t *constr);

void klangc_expr_alge_add_args(klangc_expr_alge_t *expr, int argc,
                               klangc_expr_t **args);

klangc_symbol_t *klangc_expr_alge_get_constr(klangc_expr_alge_t *expr);

klangc_expr_t *klangc_expr_alge_get_arg(klangc_expr_alge_t *expr, int index);

klangc_parse_result_t klangc_expr_alge_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_alge_t **pexpr);

void klangc_expr_alge_print(klangc_output_t *output, int prec,
                            klangc_expr_alge_t *expr);

klangc_bind_result_t klangc_expr_alge_bind(klangc_expr_env_t *upper,
                                           klangc_expr_alge_t *expr);

klangc_unbound_result_t
klangc_expr_alge_check_unbound(klangc_expr_alge_t *expr);
