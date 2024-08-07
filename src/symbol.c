#include "symbol.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include <assert.h>

struct klangc_symbol {
  const char *ks_name;
};

klangc_symbol_t *klangc_symbol_new(const char *name) {
  assert(name != NULL);
  klangc_symbol_t *symbol =
      (klangc_symbol_t *)klangc_malloc(sizeof(klangc_symbol_t));
  symbol->ks_name = klangc_strdup(name);
  return symbol;
}

klangc_parse_result_t klangc_symbol_parse(klangc_input_t *input,
                                          klangc_symbol_t **psymbol) {
  assert(input != NULL);
  assert(psymbol != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  int c = klangc_getc_skipspaces(input);
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

const char *klangc_symbol_get_name(klangc_symbol_t *symbol) {
  assert(symbol != NULL);
  return symbol->ks_name;
}

void klangc_symbol_print(klangc_output_t *output, klangc_symbol_t *symbol) {
  assert(output != NULL);
  assert(symbol != NULL);
  klangc_printf(output, "%s", symbol->ks_name);
}

int klangc_symbol_eq(klangc_symbol_t *s1, klangc_symbol_t *s2) {
  assert(s1 != NULL);
  assert(s2 != NULL);
  return strcmp(s1->ks_name, s2->ks_name) == 0;
}