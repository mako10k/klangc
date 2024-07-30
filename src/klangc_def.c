
#include "klangc_def.h"
#include "klangc.h"
#include "klangc_expr.h"
#include "klangc_hash.h"
#include "klangc_input.h"
#include "klangc_pattern.h"
#include <stdio.h>

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
  klangc_input_buf_t ib = klangc_input_save(input);
  int c;
  // Parse a definition
  klangc_pattern_t *pat = klangc_pattern_parse(input);
  if (pat == NULL) {
    fprintf(stderr, "Error parsing definition\n");
    fprintf(stderr, "pattern \"=\" expression \";\"\n");
    fprintf(stderr, "^^^^^^^ -> syntax error\n");
    return NULL;
  }

  c = klangc_getc_skipspaces(input);
  if (c != '=') {
    fprintf(stderr, "Expected '='\n");
    klangc_input_restore(input, ib);
    return NULL;
  }

  klangc_expr_t *expr = klangc_expr_parse(input);
  if (expr == NULL) {
    fprintf(stderr, "Error parsing expression\n");
    klangc_input_restore(input, ib);
    return NULL;
  }

  c = klangc_getc_skipspaces(input);
  if (c != ';') {
    fprintf(stderr, "Expected ';'\n");
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_def_t *def = klangc_malloc(sizeof(klangc_def_t));
  def->pat = pat;
  def->expr = expr;
  int ret = klangc_pattern_walkvars(defs, def, pat, klangc_def_bind);
  if (ret < 0) {
    fprintf(stderr, "Error binding variables\n");
    return NULL;
  }
  if (ret == 0) {
    fprintf(stderr, "Error: no variables in pattern\n");
    return NULL;
  }
  return defs;
}

void klangc_def_print_entry(const char *key, void *value, void *data) {
  FILE *fp = (FILE *)data;
  klangc_def_t *def = (klangc_def_t *)value;
  klangc_pattern_print(fp, def->pat);
  fprintf(fp, " = ");
  klangc_expr_print(fp, def->expr);
  fprintf(fp, ";\n");
}

void klangc_def_print(FILE *fp, klangc_hash_t *defs) {
  klangc_hash_walk(defs, klangc_def_print_entry, fp);
}