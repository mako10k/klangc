#include "bind.h"
#include "expr.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "pat.h"
#include "ref_target.h"
#include "types.h"
#include <assert.h>

/** bind要素 */
struct klangc_expr_closure_bind {
  /** 左パターン */
  klangc_pat_t *kb_pat;
  /** 右式 */
  klangc_expr_t *kb_expr;
  /** 次のbind要素 */
  klangc_bind_t *kb_next;
  /** 位置情報 */
  klangc_ipos_t kb_ipos;
};

klangc_bind_t *klangc_bind_new(klangc_pat_t *pat, klangc_expr_t *expr,
                               klangc_bind_t *next, klangc_ipos_t ipos) {
  klangc_bind_t *bind = klangc_malloc(sizeof(klangc_bind_t));
  bind->kb_pat = pat;
  bind->kb_expr = expr;
  bind->kb_next = next;
  bind->kb_ipos = ipos;
  return bind;
}

klangc_pat_t *klangc_bind_get_pat(klangc_bind_t *bind) { return bind->kb_pat; }

klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind) {
  return bind->kb_expr;
}

klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind) {
  return bind->kb_ipos;
}

klangc_bind_t *klangc_bind_get_next(klangc_bind_t *bind) {
  return bind->kb_next;
}

klangc_parse_result_t klangc_bind_parse(klangc_input_t *input,
                                        klangc_bind_t **pbind) {
  assert(input != NULL);
  assert(pbind != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_pat_t *pat;
  switch (klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, '=', &c)) {
    klangc_printf_ipos_expects(
        kstderr, ipos_ss2, "'='", c,
        "<bind> ::= <pat> ^'=' <expr> ( ';' <bind> )*;\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss2 = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss2, "<expr>", klangc_getc(input),
        "<bind> ::= <pat> '=' ^<expr> ( ';' <bind> )*;\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_ipos_t ipos3 = klangc_input_save(input);
  klangc_skipspaces(input);
  if (!klangc_expect(input, ';', NULL)) {
    // ';' の前まで入力を巻き戻す
    klangc_input_restore(input, ipos3);
    *pbind = klangc_bind_new(pat, expr, NULL, ipos_ss);
    return KLANGC_PARSE_OK;
  }
  klangc_bind_t *next = NULL;
  switch (klangc_bind_parse(input, &next)) {
  case KLANGC_PARSE_NOPARSE:
    // ';' の前まで入力を巻き戻す
    klangc_input_restore(input, ipos3);
  case KLANGC_PARSE_OK:
    *pbind = klangc_bind_new(pat, expr, next, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }
  klangc_printf(kstderr, "UNKOWN ERROR at %s:%d\n", __FILE__, __LINE__);
  return KLANGC_PARSE_ERROR;
}

void klangc_bind_print(klangc_output_t *output, klangc_bind_t *bind) {
  assert(output != NULL);
  assert(bind != NULL);
  while (1) {
    klangc_pat_print(output, KLANGC_PREC_LOWEST, bind->kb_pat);
    klangc_printf(output, " = ");
    klangc_expr_print(output, KLANGC_PREC_LOWEST, bind->kb_expr);
    bind = bind->kb_next;
    if (bind == NULL)
      break;
    klangc_printf(output, ";\n");
  }
}

klangc_bind_result_t klangc_bind_bind(klangc_expr_env_t *env,
                                      klangc_bind_t *bind) {
  klangc_expr_ref_target_t *target = klangc_expr_ref_target_new_bind(bind);
  for (klangc_bind_t *b = bind; b != NULL; b = b->kb_next) {
    klangc_bind_result_t ret = klangc_pat_bind(env, b->kb_pat, target);
    switch (ret) {
    case KLANGC_BIND_OK:
      break;
    default:
      return ret;
    }
  }
  for (klangc_bind_t *b = bind; b != NULL; b = b->kb_next) {
    klangc_bind_result_t ret = klangc_expr_bind(env, b->kb_expr);
    switch (ret) {
    case KLANGC_BIND_OK:
      break;
    default:
      return ret;
    }
  }
  klangc_printf(kstderr, "UNKOWN ERROR at %s:%d\n", __FILE__, __LINE__);
  return KLANGC_BIND_ERROR;
}

klangc_unbound_result_t klangc_bind_check_unbound(klangc_bind_t *bind) {
  for (klangc_bind_t *b = bind; b != NULL; b = b->kb_next) {
    switch (klangc_expr_check_unbound(b->kb_expr)) {
    case KLANGC_UNBOUND_OK:
      break;
    case KLANGC_UNBOUND_ERROR:
      return KLANGC_UNBOUND_ERROR;
    }
  }
  klangc_printf(kstderr, "UNKOWN ERROR at %s:%d\n", __FILE__, __LINE__);
  return KLANGC_UNBOUND_ERROR;
}