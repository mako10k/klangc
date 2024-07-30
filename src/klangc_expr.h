#ifndef __KLANGC_EXPR_H__
#define __KLANGC_EXPR_H__

#include "klangc.h"
#include "klangc_input.h"
#include <stdio.h>

klangc_expr_t *klangc_expr_parse(klangc_input_t *input);
void klangc_expr_print(FILE *fp, klangc_expr_t *expr);

#endif // __KLANGC_EXPR_H__