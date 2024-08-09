#pragma once

#include "klangc.h"

typedef enum klangc_expr_ref_target_type {
  KLANGC_ERTTYPE_BIND,
  KLANGC_ERTTYPE_LAMBDA,
} klangc_expr_ref_target_type_t;

klangc_expr_ref_target_t *klangc_expr_ref_target_new_bind(klangc_bind_t *bind);

klangc_expr_ref_target_t *klangc_expr_ref_target_new_lambda(klangc_expr_lambda_t *lambda);

klangc_expr_ref_target_type_t klangc_expr_ref_target_get_type(klangc_expr_ref_target_t *target);

klangc_bind_t *klangc_expr_ref_target_get_bind(klangc_expr_ref_target_t *target);

klangc_expr_lambda_t *klangc_expr_ref_target_get_lambda(klangc_expr_ref_target_t *target);