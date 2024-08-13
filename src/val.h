#pragma once

#include "klangc.h"

typedef enum klangc_value_type {
  KLANGC_VTYPE_EXPR,
  KLANGC_VTYPE_ALGE,
  KLANGC_VTYPE_REF,
  KLANGC_VTYPE_APPL,
  KLANGC_VTYPE_INT,
  KLANGC_VTYPE_STRING,
} klangc_value_type_t;

typedef enum klangc_eval_result_t {
  KLANGC_EVAL_RESULT_OK,
  KLANGC_EVAL_RESULT_ERROR,
} klangc_eval_result_t;

klangc_eval_result_t klangc_value_eval_appl(klangc_value_t *val);

int klangc_value_push(klangc_value_t *val, klangc_value_t *val2);

klangc_value_type_t klangc_value_get_type(klangc_value_t *val);