#include "expr.h"
#include "malloc.h"
#include "pat.h"
#include "symbol.h"
#include <assert.h>
#include <string.h>

typedef enum klangc_value_type {
  KLANGC_VTYPE_EXPR,
  KLANGC_VTYPE_SYMBOL,
  KLANGC_VTYPE_REF,
  KLANGC_VTYPE_APPL,
  KLANGC_VTYPE_INT,
  KLANGC_VTYPE_STRING,
} klangc_value_type_t;

struct klangc_value {
  klangc_value_type_t kv_type;
  klangc_expr_t *kv_expr;
  union {
    klangc_symbol_t *kv_symbol;
    klangc_value_ref_t *kv_ref;
    klangc_value_appl_t *kv_appl;
    int kv_intval;
    const char *kv_strval;
  };
};

klangc_value_t *klangc_value_new(klangc_expr_t *expr) {
  assert(expr != NULL);
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_EXPR;
  value->kv_expr = expr;
  return value;
}

int klangc_value_bind(klangc_env_t *env, klangc_pat_t *pat,
                      klangc_expr_t *expr) {
  assert(expr != NULL);
  switch (klangc_pat_get_type(pat)) {
  case KLANGC_PTYPE_SYMBOL:
    switch (klangc_expr_get_type(expr)) {
    case KLANGC_ETYPE_SYMBOL:
      return strcmp(klangc_symbol_get_name(klangc_pat_get_symbol(pat)),
                    klangc_symbol_get_name(klangc_expr_get_symbol(expr))) == 0;
    default:
      return 0;
    }
  default:
    return 0;
  }
}