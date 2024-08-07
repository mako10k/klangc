#include "bind.h"
#include "../../expr.h"
#include "../../input.h"
#include "../../malloc.h"
#include "../../output.h"
#include "../../pat.h"
#include <assert.h>

struct klangc_bind {
  klangc_pattern_t *kb_pat;
  klangc_expr_t *kb_expr;
  klangc_ipos_t kb_ipos;
};

klangc_bind_t *klangc_bind_new(klangc_pattern_t *pat, klangc_expr_t *expr,
                               klangc_ipos_t ipos) {
  klangc_bind_t *bind = klangc_malloc(sizeof(klangc_bind_t));
  bind->kb_pat = pat;
  bind->kb_expr = expr;
  bind->kb_ipos = ipos;
  return bind;
}

klangc_pattern_t *klangc_bind_get_pat(klangc_bind_t *bind) {
  return bind->kb_pat;
}

klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind) {
  return bind->kb_expr;
}

klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind) {
  return bind->kb_ipos;
}

klangc_parse_result_t klangc_bind_parse(klangc_input_t *input,
                                        klangc_bind_t **pbind) {
  assert(input != NULL);
  assert(pbind != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_pattern_t *pat;
  switch (klangc_pattern_parse(input, &pat)) {
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
  int c = klangc_getc(input);
  if (c != '=') {
    klangc_ipos_print(kstderr, ipos_ss2);
    klangc_printf(kstderr,
                  "expect '=' but get '%c': [<pattern> ^'=' <expr> ';']\n", c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss2 = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse(input, NULL, &expr)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss2);
    klangc_printf(kstderr, "expect <expr>: [<pattern> '=' ^<expr> ';']\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_bind_t *bind = klangc_bind_new(pat, expr, ipos_ss);
  *pbind = bind;
  return KLANGC_PARSE_OK;
}

void klangc_bind_print(klangc_output_t *output, klangc_bind_t *bind) {
  klangc_pattern_print(output, KLANGC_PREC_LOWEST, bind->kb_pat);
  klangc_printf(output, " = ");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, bind->kb_expr);
}