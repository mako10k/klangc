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

// *******************************
// Binding sub-expression.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Binding sub-expression.
 */
struct klangc_expr_closure_bind {
  /** Reference pattern */
  klangc_pat_t *kb_pat;
  /** Referenced expression */
  klangc_expr_t *kb_expr;
  /** Next binding */
  klangc_bind_t *kb_next;
  /** Position information */
  klangc_ipos_t kb_ipos;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_bind_t *klangc_bind_new(klangc_pat_t *pat, klangc_expr_t *expr,
                               klangc_bind_t *next, klangc_ipos_t ipos) {
  klangc_bind_t *bind = klangc_malloc(sizeof(klangc_bind_t));
  bind->kb_pat = pat;
  bind->kb_expr = expr;
  bind->kb_next = next;
  bind->kb_ipos = ipos;
  return bind;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_pat_t *klangc_bind_get_pat(klangc_bind_t *bind) { return bind->kb_pat; }

klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind) {
  return bind->kb_expr;
}

klangc_bind_t *klangc_bind_get_next(klangc_bind_t *bind) {
  return bind->kb_next;
}

klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind) {
  return bind->kb_ipos;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_bind_parse(klangc_input_t *input,
                                        klangc_bind_t **pbind) {
  assert(input != NULL);
  assert(pbind != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_pat_t *pat;
  klangc_parse_result_t res;
  res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat);
  if (res != KLANGC_PARSE_OK)
    return res;

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
  res = klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr);
  switch (res) {
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
  if (!klangc_expect(input, ';', &c)) {
    // ';' の前まで入力を巻き戻す
    klangc_input_restore(input, ipos3);
    *pbind = klangc_bind_new(pat, expr, NULL, ipos_ss);
    return KLANGC_PARSE_OK;
  }
  klangc_bind_t *next = NULL;
  res = klangc_bind_parse(input, &next);
  switch (res) {
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

// -------------------------------
// Printers.
// -------------------------------
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

// -------------------------------
// Binders.
// -------------------------------
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
  return KLANGC_BIND_OK;
}
