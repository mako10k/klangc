
#include "klangc_def.h"
#include "klangc_expr.h"
#include "klangc_hash.h"
#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_pattern.h"
#include "klangc_types.h"
#include <stdio.h>
#include <stdlib.h>

struct klangc_def_ent {
  klangc_pattern_t *pat;
  klangc_expr_t *expr;
  klangc_ipos_t ipos;
};

struct klangc_def {
  klangc_hash_t *hash;
  klangc_ipos_t ipos;
};

int klangc_def_bind(klangc_def_t *def, const char *name,
                    klangc_def_ent_t *def_ent) {
  if (klangc_hash_get(def->hash, name, NULL) != 0) {
    klangc_printf(kstderr, "Duplicate definition: %s\n", name);
    return -1;
  }
  klangc_hash_put(def->hash, name, def_ent, NULL);
  return 0;
}

klangc_def_t *klangc_def_new(klangc_ipos_t ipos) {
  klangc_def_t *def = klangc_malloc(sizeof(klangc_def_t));
  def->hash = klangc_hash_new(16);
  def->ipos = ipos;
  return def;
}

/**
 * 定義をパースする
 * @param input 入力
 * @param pdef 定義
 * @return パース結果
 */
klangc_parse_result_t klangc_def_parse(klangc_input_t *input,
                                       klangc_def_t **pdef) {
  assert(input != NULL);
  assert(pdef != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_def_t *def = klangc_def_new(ipos_ss);
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
      klangc_message_reset(input);
      klangc_message_add_ipos(input, ipos_ss1);
      klangc_message_add(
          input, "expect '=' but get '%c': [<pattern> ^'=' <expr> ';']\n", c);
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss1 = klangc_skipspaces(input);
    klangc_expr_t *expr;
    switch (klangc_expr_parse(input, &expr)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_message_reset(input);
      klangc_message_add_ipos(input, ipos_ss1);
      klangc_message_add(input, "expect <expr>: [<pattern> '=' ^<expr> ';']\n");
      klangc_message_print(input, kstderr);
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss1 = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ';') {
      klangc_message_reset(input);
      klangc_message_add_ipos(input, ipos_ss1);
      klangc_message_add(
          input, "expect ';' but get '%c': [<pattern> '=' <expr> ^';']\n", c);
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    klangc_def_ent_t *def_ent = klangc_malloc(sizeof(klangc_def_ent_t));
    def_ent->pat = pat;
    def_ent->expr = expr;
    def_ent->ipos = ipos_ss1;
    int ret = klangc_pattern_walkvars(def, def_ent, pat, klangc_def_bind);
    if (ret < 0) {
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    if (ret == 0) {
      klangc_printf(kstderr, "Error: no variables in pattern\n");
      return KLANGC_PARSE_ERROR;
    }
  }
  *pdef = def;
  return KLANGC_PARSE_OK;
}

void klangc_def_print_entry(const char *key, void *value, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_def_ent_t *def = (klangc_def_ent_t *)value;
  klangc_pattern_print(output, KLANGC_PREC_LOWEST, def->pat);
  klangc_printf(output, " = ");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, def->expr);
  klangc_printf(output, ";\n");
}

void klangc_def_print(klangc_output_t *output, klangc_def_t *def) {
  klangc_hash_walk(def->hash, klangc_def_print_entry, output);
}