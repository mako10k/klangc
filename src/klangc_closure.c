
#include "klangc_closure.h"
#include "klangc_bind.h"
#include "klangc_expr.h"
#include "klangc_hash.h"
#include "klangc_input.h"
#include "klangc_message.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_pattern.h"
#include "klangc_types.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum klangc_closure_ent_type {
  KLANGC_CLOSURE_ENT_BIND,
  KLANGC_CLOSURE_ENT_LAMBDA,
} klangc_closure_ent_type_t;

typedef struct klangc_closure_ent klangc_closure_ent_t;

struct klangc_closure_ent {
  klangc_closure_ent_type_t kce_type;
  union {
    klangc_bind_t *kce_bind;
    klangc_closure_t *kce_lambda;
  };
  klangc_closure_ent_t *kce_next;
};

struct klangc_closure {
  klangc_hash_t *kc_bind_ref;
  klangc_closure_ent_t *kc_ent;
  klangc_closure_t *kc_upper;
  klangc_ipos_t kc_ipos;
};

int klangc_closure_get_bind(klangc_closure_t *closure, const char *name,
                            klangc_bind_t **pbind,
                            klangc_closure_t **pclosure) {
  while (closure != NULL) {
    if (klangc_hash_get(closure->kc_bind_ref, name, (void **)pbind) != 0) {
      if (pclosure != NULL)
        *pclosure = closure;
      return 1;
    }
    closure = closure->kc_upper;
  }
  return 0;
}

int klangc_closure_makeref_bind(klangc_closure_t *closure, const char *name,
                                klangc_bind_t *bind) {
  if (klangc_closure_get_bind(closure, name, NULL, NULL) != 0) {
    klangc_printf(kstderr, "Duplicate definition: %s\n", name);
    return -1;
  }
  klangc_hash_put(closure->kc_bind_ref, name, bind, NULL);
  return 0;
}

klangc_closure_t *klangc_closure_new(klangc_ipos_t ipos,
                                     klangc_closure_t *upper) {
  klangc_closure_t *closure = klangc_malloc(sizeof(klangc_closure_t));
  closure->kc_bind_ref = klangc_hash_new(16);
  closure->kc_ent = NULL;
  closure->kc_ipos = ipos;
  closure->kc_upper = upper;
  return closure;
}

int klangc_closure_walk_bind(klangc_closure_t *closure,
                             int (*func)(klangc_closure_t *, klangc_pattern_t *,
                                         klangc_expr_t *, void *),
                             void *data) {
  klangc_closure_ent_t *ent = closure->kc_ent;
  int cnt = 0, ret;
  while (ent != NULL) {
    if (ent->kce_type != KLANGC_CLOSURE_ENT_BIND) {
      ent = ent->kce_next;
      continue;
    }
    klangc_pattern_t *pat = klangc_bind_get_pat(ent->kce_bind);
    klangc_expr_t *expr = klangc_bind_get_expr(ent->kce_bind);
    ret = func(closure, pat, expr, data);
    if (ret < 0)
      return -1;
    ent = ent->kce_next;
    cnt += ret;
  }
  return cnt;
}

static int klangc_def_expr_bind(klangc_closure_t *closure,
                                klangc_pattern_t *pat, klangc_expr_t *expr,
                                void *data) {
  return klangc_expr_bind(closure, expr);
}

/**
 * 定義をパースする
 * @param input 入力
 * @param pclosure 定義
 * @return パース結果
 */
klangc_parse_result_t klangc_closure_parse(klangc_input_t *input,
                                           klangc_closure_t *pupper,
                                           klangc_closure_t **pclosure) {
  assert(input != NULL);
  assert(pclosure != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_closure_t *closure = klangc_closure_new(ipos_ss, pupper);
  klangc_closure_ent_t **pent = &closure->kc_ent;
  while (1) {
    klangc_ipos_t ipos_ss1 = klangc_skipspaces(input);
    klangc_pattern_t *pat;
    switch (klangc_pattern_parse(input, &pat)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      *pclosure = closure;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss1 = klangc_skipspaces(input);
    int c = klangc_getc(input);
    if (c != '=') {
      klangc_print_ipos(kstderr, ipos_ss1);
      klangc_printf(
          kstderr, "expect '=' but get '%c': [<pattern> ^'=' <expr> ';']\n", c);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss1 = klangc_skipspaces(input);
    klangc_expr_t *expr;
    switch (klangc_expr_parse(input, closure, &expr)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_print_ipos(kstderr, ipos_ss1);
      klangc_printf(kstderr, "expect <expr>: [<pattern> '=' ^<expr> ';']\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss1 = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ';') {
      klangc_print_ipos(kstderr, ipos_ss1);
      klangc_printf(
          kstderr, "expect ';' but get '%c': [<pattern> '=' <expr> ^';']\n", c);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    klangc_bind_t *bind = klangc_bind_new(pat, expr, ipos_ss1);
    int ret = klangc_pattern_walkvars(closure, bind, pat,
                                      klangc_closure_makeref_bind);
    if (ret < 0) {
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    if (ret == 0) {
      klangc_printf(kstderr, "Error: no variables in pattern\n");
      return KLANGC_PARSE_ERROR;
    }
    klangc_closure_ent_t *ent = klangc_malloc(sizeof(klangc_closure_ent_t));
    ent->kce_type = KLANGC_CLOSURE_ENT_BIND;
    ent->kce_bind = bind;
    ent->kce_next = NULL;
    *pent = ent;
    pent = &ent->kce_next;
  }
  *pclosure = closure;

  klangc_closure_walk_bind(closure, klangc_def_expr_bind, NULL);
  return KLANGC_PARSE_OK;
}

static int klangc_closure_print_ent(klangc_closure_t *closure,
                                    klangc_pattern_t *pat, klangc_expr_t *expr,
                                    void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_pattern_print(output, KLANGC_PREC_LOWEST, pat);
  klangc_printf(output, " = ");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, expr);
  klangc_printf(output, ";\n");
  return 0;
}

void klangc_closure_print(klangc_output_t *output, klangc_closure_t *closure) {
  klangc_closure_walk_bind(closure, klangc_closure_print_ent, output);
}