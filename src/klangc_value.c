
#include "klangc.h"
#include <gc.h>

typedef struct klangc_value_symbol klangc_value_symbol_t;
typedef struct klangc_value_appl klangc_value_appl_t;
typedef struct klangc_value_lambda klangc_value_lambda_t;

typedef enum klangc_value_type {
  KLANGC_VTYPE_SYMBOL,
  KLANGC_VTYPE_APPL,
  KLANGC_VTYPE_LAMBDA,
} klangc_value_type_t;

struct klangc_value_symbol {
  char *name;
};

struct klangc_value_appl {
  klangc_value_t *function;
  klangc_value_t *argument;
};

struct klangc_value_lambda {
  klangc_pattern_t *pattern;
  klangc_expr_t *body;
};

struct klangc_value {
  klangc_value_type_t vtype;
  union {
    klangc_value_symbol_t symbol;
    klangc_value_appl_t application;
    klangc_value_lambda_t lambda;
  };
};

struct klangc_value_context {
  klangc_hash_t *symbols;
};
