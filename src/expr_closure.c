#include "expr_closure.h"
#include "bind.h"
#include "env.h"
#include "expr.h"
#include "expr_env.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "types.h"

struct klangc_expr_closure {
  klangc_expr_t *kc_expr;
  klangc_bind_t *kc_bind;
};

klangc_expr_closure_t *klangc_expr_closure_new(klangc_expr_t *expr,
                                               klangc_bind_t *bind) {
  klangc_expr_closure_t *closure = klangc_malloc(sizeof(klangc_expr_closure_t));
  closure->kc_expr = expr;
  closure->kc_bind = bind;
  return closure;
}

klangc_expr_t *klangc_expr_closure_get_expr(klangc_expr_closure_t *closure) {
  return closure->kc_expr;
}

klangc_bind_t *klangc_expr_closure_get_bind(klangc_expr_closure_t *closure) {
  return closure->kc_bind;
}

klangc_parse_result_t
klangc_expr_closure_parse(klangc_input_t *input,
                          klangc_expr_closure_t **pclosure) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, '{', NULL)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr)) {
  case KLANGC_PARSE_OK:
    break;

  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "<expr>", klangc_getc(input),
        "<closure> ::= '{' ^<expr> ';' ( <bind> ';' )? '}';\n");

  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, ';', &c)) {
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "';'", c,
        "<closure> ::= '{' <expr> ^';' ( <bind> ';' )? '}'\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_bind_t *bind = NULL;
  ipos_ss = klangc_skipspaces(input);
  switch (klangc_bind_parse(input, &bind)) {
  case KLANGC_PARSE_OK:
    ipos_ss = klangc_skipspaces(input);
    if (klangc_expect(input, ';', &c))
      break;
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "';'", c,
        "<closure> ::= '{' <expr> ';' ( <bind> ^';' )? '}'\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;

  case KLANGC_PARSE_NOPARSE:
    break;

  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  if (klangc_expect(input, '}', &c)) {
    *pclosure = klangc_expr_closure_new(expr, bind);
    return KLANGC_PARSE_OK;
  }
  klangc_printf_ipos_expects(
      kstderr, ipos_ss, "}", klangc_getc(input),
      "<closure> ::= '{' <expr> ';' ( <bind> ';' )? ^'}'\n");
  klangc_input_restore(input, ipos);
  return KLANGC_PARSE_ERROR;
}

void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_expr_closure_t *closure) {
  klangc_printf(output, "{");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, closure->kc_expr);
  klangc_printf(output, ";\n");
  klangc_indent(output, 2);
  if (closure->kc_bind != NULL) {
    klangc_bind_print(output, closure->kc_bind);
    klangc_printf(output, ";\n");
  }
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}

klangc_bind_result_t klangc_expr_closure_bind(klangc_expr_env_t *env,
                                              klangc_expr_closure_t *closure) {
  klangc_expr_env_t *env_inner = klangc_expr_env_new(env);
  if (closure->kc_bind != NULL)
    if (klangc_bind_bind(env_inner, closure->kc_bind) == -1)
      return -1;
  return klangc_expr_bind(env_inner, closure->kc_expr);
}
