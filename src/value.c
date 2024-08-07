#include "value.h"
#include "expr.h"
#include "expr/closure.h"
#include "expr/closure/bind.h"
#include "expr/closure/entry.h"
#include "expr/lambda.h"
#include "expr/ref.h"
#include "malloc.h"
#include <assert.h>

typedef struct klangc_value klangc_value_t;
typedef struct klangc_value_ref klangc_value_ref_t;
typedef struct klangc_value_appl klangc_value_appl_t;

typedef enum klangc_value_type {
  KLANGC_VTYPE_SYMBOL,
  KLANGC_VTYPE_REF,
  KLANGC_VTYPE_APPL,
  KLANGC_VTYPE_INT,
  KLANGC_VTYPE_STRING,
} klangc_value_type_t;

struct klangc_value {
  klangc_value_type_t kv_type;
  union {
    klangc_symbol_t *kv_symbol;
    klangc_value_ref_t *kv_ref;
    klangc_value_appl_t *kv_appl;
    int kv_intval;
    const char *kv_strval;
  };
};

struct klangc_value_ref {
  klangc_ref_t *ref;
};

struct klangc_value_appl {
  klangc_value_t *func;
  klangc_value_t *arg;
};

klangc_value_t *klangc_value_new_symbol(klangc_symbol_t *symbol) {
  assert(symbol != NULL);
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_SYMBOL;
  value->kv_symbol = symbol;
  return value;
}

klangc_value_ref_t *klangc_value_ref_from_expr_ref(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_value_ref_t *value_ref = klangc_malloc(sizeof(klangc_value_ref_t));
  value_ref->ref = ref;
  return value_ref;
}

klangc_value_t *klangc_value_new_ref(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_REF;
  value->kv_ref = klangc_value_ref_from_expr_ref(ref);
  return value;
}

klangc_value_appl_t *klangc_value_appl_new(klangc_value_t *func,
                                           klangc_value_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_value_appl_t *value_appl = klangc_malloc(sizeof(klangc_value_appl_t));
  value_appl->func = func;
  value_appl->arg = arg;
  return value_appl;
}

klangc_value_t *klangc_value_new_appl(klangc_value_t *func,
                                      klangc_value_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_APPL;
  value->kv_appl = klangc_value_appl_new(func, arg);
  return value;
}

klangc_value_t *klangc_value_new_int(int intval) {
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_INT;
  value->kv_intval = intval;
  return value;
}

klangc_value_t *klangc_value_new_string(const char *strval) {
  assert(strval != NULL);
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_STRING;
  value->kv_strval = klangc_strdup(strval);
  return value;
}

klangc_value_t *klangc_value_from_expr(klangc_expr_t *expr) {
  assert(expr != NULL);
  switch (klangc_expr_get_type(expr)) {
  case KLANGC_ETYPE_SYMBOL: {
    klangc_symbol_t *symbol = klangc_expr_get_symbol(expr);
    return klangc_value_new_symbol(symbol);
  }
  case KLANGC_ETYPE_REF: {
    klangc_expr_ref_t *eref = klangc_expr_get_ref(expr);
    klangc_ref_t *ref = klangc_expr_ref_get_ref(eref);
    return klangc_value_new_ref(ref);
  }
  case KLANGC_ETYPE_APPL: {
    klangc_expr_appl_t *appl = klangc_expr_get_appl(expr);
    klangc_expr_t *efunc = klangc_expr_appl_get_func(appl);
    klangc_expr_t *earg = klangc_expr_appl_get_arg(appl);
    klangc_value_t *vfunc = klangc_value_from_expr(efunc);
    klangc_value_t *varg = klangc_value_from_expr(earg);
    return klangc_value_new_appl(vfunc, varg);
  }
  case KLANGC_ETYPE_INT: {
    int intval = klangc_expr_get_int(expr);
    return klangc_value_new_int(intval);
  }
  case KLANGC_ETYPE_STRING: {
    const char *strval = klangc_expr_get_string(expr);
    return klangc_value_new_string(strval);
  }
  case KLANGC_ETYPE_LAMBDA: {
    klangc_expr_lambda_t *lambda = klangc_expr_get_lambda(expr);
    klangc_pat_t *arg = klangc_expr_lambda_get_arg(lambda);
    klangc_expr_t *ebody = klangc_expr_lambda_get_body(lambda);
    (void)arg;
    (void)ebody;
  }
  case KLANGC_ETYPE_CLOSURE: {
    klangc_expr_closure_t *closure = klangc_expr_get_closure(expr);
    klangc_expr_closure_entry_t *ent =
        klangc_expr_closure_get_ent_first(closure);
    while (ent != NULL) {
      if (klangc_expr_closure_entry_isbind(ent)) {
        klangc_expr_closure_bind_t *bind =
            klangc_expr_closure_entry_get_bind(ent);
        klangc_pat_t *pat = klangc_expr_closure_bind_get_pat(bind);
        klangc_expr_t *expr = klangc_expr_closure_bind_get_expr(bind);
        (void)pat;
        (void)expr;
      } else if (klangc_expr_closure_entry_islambda(ent)) {
        klangc_expr_lambda_t *lambda =
            klangc_expr_closure_entry_get_lambda(ent);
        klangc_pat_t *arg = klangc_expr_lambda_get_arg(lambda);
        klangc_expr_t *body = klangc_expr_lambda_get_body(lambda);
        (void)arg;
        (void)body;
      }
      ent = klangc_expr_closure_entry_get_next(ent);
    }
  }
  }
  return NULL;
}