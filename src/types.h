#pragma once

#include "klangc.h"
#include <stdio.h>

typedef struct klangc_expr_closure_bind klangc_expr_closure_bind_t;

typedef struct klangc_expr_closure klangc_expr_closure_t;

typedef struct klangc_expr_closure_entry klangc_expr_closure_entry_t;

typedef struct klangc_pat klangc_pat_t;

typedef struct klangc_pat_ref klangc_pat_ref_t;

typedef struct klangc_pat_appl klangc_pat_appl_t;

typedef struct klangc_pat_as klangc_pat_as_t;

typedef struct klangc_pat_int klangc_pat_int_t;

typedef struct klangc_pat_string klangc_pat_string_t;

typedef struct klangc_expr klangc_expr_t;

typedef struct klangc_expr_ref klangc_expr_ref_t;

typedef struct klangc_expr_appl klangc_expr_appl_t;

typedef struct klangc_lambda klangc_lambda_t;

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
