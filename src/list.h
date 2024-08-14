#pragma once

#include "expr_alge.h"
#include "klangc.h"
#include "pat_alge.h"
#include "str.h"
#include "symbol.h"

// *******************************
// List.
// *******************************

__attribute__((unused)) static const klangc_str_t *klangc_nil_str(void) {
  static const klangc_str_t *nil_str = NULL;
  if (nil_str == NULL)
    nil_str = klangc_str_new("[]", 2);
  return nil_str;
}

__attribute__((unused)) static klangc_symbol_t *klangc_nil_symbol(void) {
  static klangc_symbol_t *nil_sym = NULL;
  if (nil_sym == NULL)
    nil_sym = klangc_symbol_new(klangc_nil_str());
  return nil_sym;
}

__attribute__((unused)) static klangc_pat_alge_t *klangc_nil_pat_alge(void) {
  static klangc_pat_alge_t *nil_pat_alge = NULL;
  if (nil_pat_alge == NULL) {
    nil_pat_alge = klangc_pat_alge_new(klangc_nil_symbol());
  }
  return nil_pat_alge;
}

__attribute__((unused)) static klangc_expr_alge_t *klangc_nil_expr_alge(void) {
  static klangc_expr_alge_t *nil_expr_alge = NULL;
  if (nil_expr_alge == NULL) {
    nil_expr_alge = klangc_expr_alge_new(klangc_nil_symbol());
  }
  return nil_expr_alge;
}

__attribute__((unused)) static const klangc_str_t *klangc_cons_str(void) {
  static const klangc_str_t *cons_str = NULL;
  if (cons_str == NULL)
    cons_str = klangc_str_new(":", 1);
  return cons_str;
}

__attribute__((unused)) static klangc_symbol_t *klangc_cons_symbol(void) {
  static klangc_symbol_t *cons_sym = NULL;
  if (cons_sym == NULL)
    cons_sym = klangc_symbol_new(klangc_cons_str());
  return cons_sym;
}