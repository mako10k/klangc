#ifndef __KLANGC_EXPR_H__
#define __KLANGC_EXPR_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_expr_t **pexpr);
void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr);

#endif // __KLANGC_EXPR_H__