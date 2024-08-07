#pragma once

#include "../klangc.h"

klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func,
                                         klangc_expr_t *arg);

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl);

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl);