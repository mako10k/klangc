
#include "klangc_expr.h"
#include "klangc_hash.h"
#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_pattern.h"
#include "klangc_types.h"
#include <stdio.h>
#include <stdlib.h>

struct klangc_def {
  klangc_pattern_t *pat;
  klangc_expr_t *expr;
};

int klangc_def_bind(klangc_hash_t *defs, const char *name, klangc_def_t *def) {
  if (klangc_hash_get(defs, name, NULL) != 0) {
    fprintf(stderr, "Duplicate definition: %s\n", name);
    return -1;
  }
  klangc_hash_put(defs, name, def, NULL);
  return 0;
}

klangc_hash_t *klangc_def_parse(klangc_input_t *input) {
  klangc_hash_t *defs = klangc_hash_new(16);
  while (1) {
    klangc_input_buf_t ib = klangc_input_save(input);
    klangc_pattern_t *pat = klangc_pattern_parse(input);
    if (pat == NULL)
      return defs;

    int c = klangc_getc_skipspaces(input);
    if (c != '=') {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(
          input, "expect '=' but get '%c': [<pattern> ^'=' <expr> ';']\n", c);
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }

    klangc_expr_t *expr = klangc_expr_parse(input);
    if (expr == NULL) {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(input, "expect <expr>: [<pattern> '=' ^<expr> ';']\n");
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }

    c = klangc_getc_skipspaces(input);
    if (c != ';') {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(
          input, "expect ';' but get '%c': [<pattern> '=' <expr> ^';']\n", c);
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }
    klangc_def_t *def = klangc_malloc(sizeof(klangc_def_t));
    def->pat = pat;
    def->expr = expr;
    int ret = klangc_pattern_walkvars(defs, def, pat, klangc_def_bind);
    if (ret < 0) {
      fprintf(stderr, "Error binding variables\n");
      exit(1);
    }
    if (ret == 0) {
      fprintf(stderr, "Error: no variables in pattern\n");
      return NULL;
    }
  }
  return defs;
}

void klangc_def_print_entry(const char *key, void *value, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_def_t *def = (klangc_def_t *)value;
  klangc_pattern_print(output, KLANGC_PREC_LOWEST, def->pat);
  klangc_printf(output, " = ");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, def->expr);
  klangc_printf(output, ";\n");
}

void klangc_def_print(klangc_output_t *output, klangc_hash_t *defs) {
  klangc_hash_walk(defs, klangc_def_print_entry, output);
}