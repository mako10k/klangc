#include "tuple.h"
#include "expr.h"
#include "input.h"
#include "output.h"
#include "parse.h"
#include "types.h"
#include <assert.h>

klangc_parse_result_t klangc_pat_parse_tuple(klangc_input_t *input,
                                             klangc_pat_t **ppat) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_expect(input, '(', NULL);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return res;
  }
  klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
  res = klangc_expect(input, ')', NULL);
  switch (res) {
  case KLANGC_PARSE_OK:
    *ppat = klangc_pat_new_alge(klangc_unit_pat_alge(), ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_pat_t *pat;
  res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss2, "')' or <pat>", klangc_getc(input),
        "<pat> ::= .. | '(' ^(<pat> (',' <pat>)*)? ')' | ..\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_pat_alge_t *palge = NULL;
  while (1) {
    ipos_ss2 = klangc_skipspaces(input);
    res = klangc_expect(input, ')', NULL);
    switch (res) {
    case KLANGC_PARSE_OK:
      *ppat = pat;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_NOPARSE:
      break;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    int c;
    res = klangc_expect(input, ',', &c);
    switch (res) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(
          kstderr, ipos_ss2, "')' or ','", c,
          "<pat> ::= .. | '(' (<pat> (^',' <pat>)*)? ')' | ..\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    klangc_pat_t *pat2;
    res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat2);
    switch (res) {
    case KLANGC_PARSE_OK:
      if (palge == NULL) {
        palge = klangc_pat_alge_new(klangc_tuple_symbol());
        klangc_pat_alge_add_arg(palge, pat);
        pat = klangc_pat_new_alge(palge, ipos_ss);
      }
      klangc_pat_alge_add_arg(palge, pat2);
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(
          kstderr, ipos_ss2, "')' or ','", klangc_getc(input),
          "<pat> ::= .. | '(' (<pat> (',' ^<pat>)*)? ')' | ..\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  }
}

klangc_parse_result_t klangc_expr_parse_tuple(klangc_input_t *input,
                                              klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_expect(input, '(', NULL);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return res;
  }
  klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
  res = klangc_expect(input, ')', NULL);
  switch (res) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_alge(klangc_unit_expr_alge(), ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_expr_t *expr;
  res = klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss2, "')' or <expr>", klangc_getc(input),
        "<expr> ::= .. | '(' ^(<expr> (',' <expr>)*)? ')' | ..\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_expr_alge_t *ealge = NULL;
  while (1) {
    ipos_ss2 = klangc_skipspaces(input);
    res = klangc_expect(input, ')', NULL);
    switch (res) {
    case KLANGC_PARSE_OK:
      *pexpr = expr;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_NOPARSE:
      break;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    int c;
    res = klangc_expect(input, ',', &c);
    switch (res) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(
          kstderr, ipos_ss2, "')' or ','", c,
          "<expr> ::= .. | '(' (<expr> (^',' <expr>)*)? ')' | ..\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    klangc_expr_t *expr2;
    res = klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr2);
    switch (res) {
    case KLANGC_PARSE_OK:
      if (ealge == NULL) {
        ealge = klangc_expr_alge_new(klangc_tuple_symbol());
        klangc_expr_alge_add_arg(ealge, expr);
        expr = klangc_expr_new_alge(ealge, ipos_ss);
      }
      klangc_expr_alge_add_arg(ealge, expr2);
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(
          kstderr, ipos_ss2, "')' or ','", klangc_getc(input),
          "<expr> ::= .. | '(' (<expr> (',' ^<expr>)*)? ')' | ..\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  }
}

int klangc_pat_is_tuple(klangc_pat_t *pat) {
  assert(pat != NULL);
  if (klangc_pat_get_type(pat) != KLANGC_PTYPE_ALGE)
    return 0;
  klangc_pat_alge_t *palge = klangc_pat_get_alge(pat);
  return klangc_pat_alge_get_constr(palge) == klangc_tuple_symbol();
}

int klangc_expr_is_tuple(klangc_expr_t *expr) {
  assert(expr != NULL);
  if (klangc_expr_get_type(expr) != KLANGC_ETYPE_ALGE)
    return 0;
  klangc_expr_alge_t *ealge = klangc_expr_get_alge(expr);
  return klangc_expr_alge_get_constr(ealge) == klangc_tuple_symbol();
}

void klangc_pat_print_tuple(klangc_output_t *output, klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(klangc_pat_is_tuple(pat));
  klangc_pat_alge_t *palge = klangc_pat_get_alge(pat);
  unsigned int argc = klangc_pat_alge_get_argc(palge);
  klangc_printf(output, "(");
  for (unsigned int i = 0; i < argc; i++) {
    if (i > 0)
      klangc_printf(output, ", ");
    klangc_pat_print(output, KLANGC_PREC_LOWEST,
                     klangc_pat_alge_get_arg(palge, i));
  }
  klangc_printf(output, ")");
}

void klangc_expr_print_tuple(klangc_output_t *output, klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(klangc_expr_is_tuple(expr));
  klangc_expr_alge_t *ealge = klangc_expr_get_alge(expr);
  unsigned int argc = klangc_expr_alge_get_argc(ealge);
  klangc_printf(output, "(");
  for (unsigned int i = 0; i < argc; i++) {
    if (i > 0)
      klangc_printf(output, ", ");
    klangc_expr_print(output, KLANGC_PREC_LOWEST,
                      klangc_expr_alge_get_arg(ealge, i));
  }
  klangc_printf(output, ")");
}