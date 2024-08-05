#include "klangc_symbol.h"

struct klangc_symbol {
  char *ks_name;
};

klangc_symbol_t *klangc_symbol_new(const char *name) {
  assert(name != NULL);
  klangc_symbol_t *symbol =
      (klangc_symbol_t *)klangc_malloc(sizeof(klangc_symbol_t));
  symbol->ks_name = klangc_strdup(name);
  return symbol;
}

void klangc_symbol_free(klangc_symbol_t *symbol) {
  assert(symbol != NULL);
  klangc_free(symbol->ks_name);
  klangc_free(symbol);
}

klangc_parse_result_t klangc_symbol_parse(klangc_input_t *input,
                                          klangc_symbol_t **psymbol) {
  assert(input != NULL);
  assert(psymbol != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  (void)ipos_ss;
  int c = klangc_getc(input);
  if (!isalpha(c) && c != '_') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  char *name = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  name[len++] = c;
  while (1) {
    klangc_ipos_t ipos2 = klangc_input_save(input);
    c = klangc_getc(input);
    if (!isalnum(c) && c != '_') {
      klangc_input_restore(input, ipos2);
      break;
    }
    if (cap <= len + 1) {
      cap *= 2;
      name = (char *)klangc_realloc(name, cap);
    }
    name[len++] = c;
  }
  name[len] = '\0';
  name = (char *)klangc_realloc(name, len + 1);
  *psymbol = klangc_symbol_new(name);
  return KLANGC_PARSE_OK;
}

char *klangc_symbol_get_name(klangc_symbol_t *symbol) {
  assert(symbol != NULL);
  return symbol->ks_name;
}

void klangc_symbol_print(klangc_output_t *output, klangc_symbol_t *symbol) {
  assert(output != NULL);
  assert(symbol != NULL);
  klangc_printf(output, "%s", symbol->ks_name);
}