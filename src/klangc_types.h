#pragma once

#include "klangc.h"
#include <stdio.h>

typedef struct klangc_bind klangc_bind_t;

typedef struct klangc_closure klangc_closure_t;

typedef struct klangc_closure_ent klangc_closure_ent_t;

typedef struct klangc_pattern klangc_pattern_t;

typedef struct klangc_pattern_appl klangc_pattern_appl_t;

typedef struct klangc_pattern_as klangc_pattern_as_t;

typedef struct klangc_pattern_int klangc_pattern_int_t;

typedef struct klangc_pattern_string klangc_pattern_string_t;

typedef struct klangc_expr klangc_expr_t;

typedef struct klangc_expr_appl klangc_expr_appl_t;

typedef struct klangc_expr_choice klangc_expr_choice_t;

typedef struct klangc_expr_lambda klangc_expr_lambda_t;

typedef struct klangc_hash klangc_hash_t;

typedef struct klangc_value klangc_value_t;

typedef struct klangc_symbol klangc_symbol_t;

typedef struct klangc_ref klangc_ref_t;

typedef struct klangc_input klangc_input_t;

typedef struct klangc_output klangc_output_t;

typedef struct klangc_ipos {
  klangc_input_t *kip_input;
  off_t kip_offset;
  int kip_line;
  int kip_col;
} klangc_ipos_t;

typedef enum klangc_parse_result {
  KLANGC_PARSE_OK,
  KLANGC_PARSE_NOPARSE,
  KLANGC_PARSE_ERROR,
} klangc_parse_result_t;
