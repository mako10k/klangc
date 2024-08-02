
#include "klangc_def.h"
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

struct klangc_def_ent {
  klangc_pattern_t *pat;
  klangc_expr_t *expr;
  klangc_def_ent_t *next;
  klangc_ipos_t ipos;
};

struct klangc_def {
  klangc_hash_t *hash;
  klangc_def_ent_t *def_ent;
  klangc_def_t *enclosed_by;
  klangc_ipos_t ipos;
};

int klangc_def_get(klangc_def_t *def, const char *name,
                   klangc_def_ent_t **pdef_ent, klangc_def_t **pdef) {
  while (def != NULL) {
    if (klangc_hash_get(def->hash, name, (void **)pdef_ent) != 0) {
      if (pdef != NULL)
        *pdef = def;
      return 1;
    }
    def = def->enclosed_by;
  }
  return 0;
}

int klangc_def_put(klangc_def_t *def, const char *name,
                   klangc_def_ent_t *def_ent) {
  return klangc_hash_put(def->hash, name, def_ent, NULL);
}

int klangc_def_bind(klangc_def_t *def, const char *name,
                    klangc_def_ent_t *def_ent) {
  if (klangc_def_get(def, name, NULL, NULL) != 0) {
    klangc_printf(kstderr, "Duplicate definition: %s\n", name);
    return -1;
  }
  klangc_hash_put(def->hash, name, def_ent, NULL);
  return 0;
}

klangc_def_t *klangc_def_new(klangc_ipos_t ipos, klangc_def_t *enclosed_by) {
  klangc_def_t *def = klangc_malloc(sizeof(klangc_def_t));
  def->hash = klangc_hash_new(16);
  def->def_ent = NULL;
  def->ipos = ipos;
  def->enclosed_by = enclosed_by;
  return def;
}

int klangc_def_expr_varcheck(klangc_def_t *def, klangc_pattern_t *pat,
                             klangc_expr_t *expr, void *data) {
  return klangc_expr_varcheck(def, expr);
}

int klangc_def_walk(klangc_def_t *def,
                    int (*func)(klangc_def_t *, klangc_pattern_t *,
                                klangc_expr_t *, void *),
                    void *data) {
  klangc_def_ent_t *def_ent = def->def_ent;
  int cnt = 0, ret;
  while (def_ent != NULL) {
    ret = func(def, def_ent->pat, def_ent->expr, data);
    if (ret < 0)
      return -1;
    def_ent = def_ent->next;
    cnt += ret;
  }
  return cnt;
}

/**
 * 定義をパースする
 * @param input 入力
 * @param pdef 定義
 * @return パース結果
 */
klangc_parse_result_t klangc_def_parse(klangc_input_t *input,
                                       klangc_def_t *enclosed_by,
                                       klangc_def_t **pdef) {
  assert(input != NULL);
  assert(pdef != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_def_t *def = klangc_def_new(ipos_ss, enclosed_by);
  klangc_def_ent_t **pdef_ent = &def->def_ent;
  while (1) {
    klangc_ipos_t ipos_ss1 = klangc_skipspaces(input);
    klangc_pattern_t *pat;
    switch (klangc_pattern_parse(input, &pat)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      *pdef = def;
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
    switch (klangc_expr_parse(input, enclosed_by, &expr)) {
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
    klangc_def_ent_t *def_ent = klangc_malloc(sizeof(klangc_def_ent_t));
    def_ent->pat = pat;
    def_ent->expr = expr;
    def_ent->ipos = ipos_ss1;
    def_ent->next = NULL;
    int ret = klangc_pattern_walkvars(def, def_ent, pat, klangc_def_bind);
    if (ret < 0) {
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    if (ret == 0) {
      klangc_printf(kstderr, "Error: no variables in pattern\n");
      return KLANGC_PARSE_ERROR;
    }
    *pdef_ent = def_ent;
    pdef_ent = &def_ent->next;
  }
  *pdef = def;

  klangc_def_walk(def, klangc_def_expr_varcheck, NULL);
  return KLANGC_PARSE_OK;
}

int klangc_def_print_ent(klangc_def_t *def, klangc_pattern_t *pat,
                         klangc_expr_t *expr, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_pattern_print(output, KLANGC_PREC_LOWEST, pat);
  klangc_printf(output, " = ");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, expr);
  klangc_printf(output, ";\n");
  return 0;
}

void klangc_def_print(klangc_output_t *output, klangc_def_t *def) {
  klangc_def_walk(def, klangc_def_print_ent, output);
}