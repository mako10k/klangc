#pragma once

#include "klangc.h"
#include "types.h"

typedef enum klangc_eval_result_t {
  KLANGC_EVAL_RESULT_OK,
  KLANGC_EVAL_RESULT_ERROR,
} klangc_eval_result_t;

// ----- CONSTRUCTORS -----
klangc_value_t *klangc_value_new_alge(klangc_value_alge_t *alge);
klangc_value_t *klangc_value_new_appl(klangc_value_appl_t *appl);
klangc_value_t *klangc_value_new_str(const klangc_str_t *str);
klangc_value_t *klangc_value_new_ioref(klangc_value_ioref_t *ioref);
klangc_value_t *klangc_value_new_error(const char *message);
klangc_value_t *klangc_value_new_int(int val);
/**
 * evaluate value to WHNF
 * @param val value
 */
klangc_eval_result_t klangc_value_eval(klangc_value_t *val);

int klangc_value_push(klangc_value_t *val, klangc_value_t *val2);

klangc_value_type_t klangc_value_get_type(klangc_value_t *val);

klangc_value_alge_t *klangc_value_get_alge(klangc_value_t *val);

const klangc_str_t *klangc_value_get_string(klangc_value_t *val);

klangc_value_ioref_t *klangc_value_get_ioref(klangc_value_t *val);

const char *klangc_value_get_error(klangc_value_t *val);

int klangc_value_get_int(klangc_value_t *val);

// ----- IOREF -----
klangc_value_ioref_t *klangc_value_ioref_new(klangc_inst_ent_t *ioref);
klangc_inst_ent_t *klangc_value_ioref_get_ioref(klangc_value_ioref_t *ioref);

klangc_value_t *klangc_value_new_data(klangc_value_data_t *data);
klangc_value_data_t *klangc_value_get_data(klangc_value_t *val);