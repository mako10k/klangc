#include "val.h"
#include "env.h"
#include "malloc.h"
#include "pat.h"
#include "pat_alge.h"
#include "pat_as.h"
#include "pat_ref.h"
#include "str.h"
#include "symbol.h"
#include "types.h"
#include "val_alge.h"
#include "val_appl.h"
#include <assert.h>
#include <string.h>

struct klangc_value {
  klangc_value_type_t kv_type;
  klangc_expr_t *kv_expr;
  union {
    klangc_value_alge_t *kv_alge;
    klangc_value_ref_t *kv_vref;
    klangc_value_appl_t *kv_appl;
    klangc_value_ioref_t *kv_ioref;
    klangc_value_data_t *kv_data;
    int kv_intval;
    const klangc_str_t *kv_strval;
  };
};

klangc_value_t *klangc_value_new(klangc_expr_t *expr) {
  assert(expr != NULL);
  klangc_value_t *value = klangc_malloc(sizeof(klangc_value_t));
  value->kv_type = KLANGC_VTYPE_EXPR;
  value->kv_expr = expr;
  return value;
}

klangc_value_t *klangc_value_new_from_expr(klangc_expr_t *expr) {
  assert(expr != NULL);
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_EXPR;
  val->kv_expr = expr;
  return val;
}

klangc_eval_result_t klangc_value_eval(klangc_value_t *val) {
  assert(val != NULL);
  switch (val->kv_type) {
  case KLANGC_VTYPE_EXPR:
    return klangc_value_eval(klangc_value_new_from_expr(val->kv_expr));
  case KLANGC_VTYPE_ALGE:
  default:
    return KLANGC_EVAL_RESULT_OK;
  }
}

klangc_eval_result_t klangc_value_eval_appl(klangc_value_t *val) {
  assert(val != NULL);
  assert(val->kv_type == KLANGC_VTYPE_APPL);
  klangc_value_appl_t *appl = val->kv_appl;
  klangc_value_t *func = klangc_value_appl_get_func(appl);
  klangc_value_t *arg = klangc_value_appl_get_arg(appl);
  klangc_eval_result_t res = klangc_value_eval(func);
  if (res == KLANGC_EVAL_RESULT_ERROR)
    return KLANGC_EVAL_RESULT_ERROR;
  /*
  res = klangc_value_eval_appl_2(val, func, arg);
  if (res == KLANGC_EVAL_RESULT_ERROR)
    return KLANGC_EVAL_RESULT_ERROR;
  klangc_value_push(val, val);
  */
  return KLANGC_EVAL_RESULT_OK;
}

int klangc_value_match(klangc_env_t *env, klangc_pat_t *pat,
                       klangc_value_t *val) {
  assert(env != NULL);
  assert(pat != NULL);
  assert(val != NULL);
  klangc_pat_type_t ptype = klangc_pat_get_type(pat);
  klangc_value_type_t vtype = val->kv_type;
  switch (ptype) {
  case KLANGC_PTYPE_REF: {
    // パターンが参照の場合 (一切評価せずに環境に登録)
    klangc_pat_ref_t *pref = klangc_pat_get_ref(pat);
    const klangc_symbol_t *sym = klangc_pat_ref_get_symbol(pref);
    const klangc_str_t *name = klangc_symbol_get_name(sym);
    klangc_env_put(env, name, val);
  }
  case KLANGC_PTYPE_ALGE: {
    // パターンが代数的データの場合
    // 値も既に代数的値に簡約化されている場合はコンストラクタと引数を比較
    // 値が適用の場合は適用を評価してから比較
    klangc_pat_alge_t *palge = klangc_pat_get_alge(pat);
    const klangc_symbol_t *pconstr = klangc_pat_alge_get_constr(palge);
    if (vtype == KLANGC_VTYPE_APPL) {
      klangc_value_eval_appl(val);
      vtype = val->kv_type;
    }
    if (vtype == KLANGC_VTYPE_ALGE) {
      klangc_value_alge_t *valge = val->kv_alge;
      const klangc_symbol_t *vconstr = klangc_value_alge_get_constr(valge);
      if (klangc_str_cmp(klangc_symbol_get_name(pconstr),
                         klangc_symbol_get_name(vconstr)) != 0)
        return 0;
      int pargc = klangc_pat_alge_get_argc(palge);
      int vargc = klangc_value_alge_get_argc(valge);
      if (pargc != vargc)
        return 0;
      for (int i = 0; i < pargc; i++) {
        klangc_pat_t *parg = klangc_pat_alge_get_arg(palge, i);
        klangc_value_t *varg = klangc_value_alge_get_arg(valge, i);
        if (!klangc_value_match(env, parg, varg))
          return 0;
      }
      return 1;
    }
  }
  case KLANGC_PTYPE_AS: {
    // パターンが AS パターンの場合
    klangc_pat_as_t *pas = klangc_pat_get_as(pat);
    klangc_pat_t *pinner = klangc_pat_as_get_pat(pas);
    int ret = klangc_value_match(env, pinner, val);
    if (ret) {
      klangc_pat_ref_t *pref = klangc_pat_as_get_ref(pas);
      const klangc_symbol_t *sym = klangc_pat_ref_get_symbol(pref);
      const klangc_str_t *name = klangc_symbol_get_name(sym);
      klangc_env_put(env, name, val);
    }
  }
  case KLANGC_PTYPE_INT: {
    // パターンが整数の場合
    if (vtype != KLANGC_VTYPE_INT)
      return 0;
    int pval = klangc_pat_get_int(pat);
    int vval = val->kv_intval;
    if (pval != vval)
      return 0;
    return 1;
  }
  case KLANGC_PTYPE_STRING: {
    // パターンが文字列の場合
    if (vtype != KLANGC_VTYPE_STRING)
      return 0;
    const klangc_str_t *pval = klangc_pat_get_str(pat);
    const klangc_str_t *vval = val->kv_strval;
    if (klangc_str_cmp(pval, vval) != 0)
      return 0;
    return 1;
  }
  }
  return 0;
}

klangc_value_t *klangc_value_new_error(const char *message) {
  assert(message != NULL);
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_ERROR;
  val->kv_strval = klangc_str_new(message, strlen(message));
  return val;
}

klangc_value_type_t klangc_value_get_type(klangc_value_t *val) {
  assert(val != NULL);
  return val->kv_type;
}

const klangc_str_t *klangc_value_get_string(klangc_value_t *val) {
  assert(val != NULL);
  assert(val->kv_type == KLANGC_VTYPE_STRING);
  return val->kv_strval;
}

klangc_value_t *klangc_value_new_ioref(klangc_value_ioref_t *ioref) {
  assert(ioref != NULL);
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_IOREF;
  val->kv_ioref = ioref;
  return val;
}

klangc_value_alge_t *klangc_value_get_alge(klangc_value_t *val) {
  assert(val != NULL);
  assert(val->kv_type == KLANGC_VTYPE_ALGE);
  return val->kv_alge;
}

klangc_value_t *klangc_value_new_alge(klangc_value_alge_t *alge) {
  assert(alge != NULL);
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_ALGE;
  val->kv_alge = alge;
  return val;
}

klangc_value_t *klangc_value_new_data(klangc_value_data_t *data) {
  assert(data != NULL);
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_DATA;
  val->kv_data = data;
  return val;
}

klangc_value_data_t *klangc_value_get_data(klangc_value_t *val) {
  assert(val != NULL);
  assert(val->kv_type == KLANGC_VTYPE_DATA);
  return val->kv_data;
}

int klangc_value_get_int(klangc_value_t *val) {
  assert(val != NULL);
  assert(val->kv_type == KLANGC_VTYPE_INT);
  return val->kv_intval;
}

const klangc_str_t *klangc_value_get_str(klangc_value_t *val) {
  assert(val != NULL);
  assert(val->kv_type == KLANGC_VTYPE_STRING);
  return val->kv_strval;
}

klangc_value_t *klangc_value_new_int(int intval) {
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_INT;
  val->kv_intval = intval;
  return val;
}

klangc_value_t *klangc_value_new_str(const klangc_str_t *str) {
  klangc_value_t *val = klangc_malloc(sizeof(klangc_value_t));
  val->kv_type = KLANGC_VTYPE_STRING;
  val->kv_strval = str;
  return val;
}