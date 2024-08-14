#include "symbol.h"
#include "hash.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "str.h"
#include <assert.h>

struct klangc_symbol {
  const klangc_str_t *ks_name;
};

static klangc_hash_t *g_symbol_hash = NULL;

__attribute__((constructor)) static void klangc_symbol_init() {
  g_symbol_hash = klangc_hash_new(16);
}

const klangc_symbol_t *klangc_symbol_new(const klangc_str_t *name) {
  assert(name != NULL);
  klangc_symbol_t *symbol;
  if (klangc_hash_get(g_symbol_hash, name, (void **)&symbol))
    return symbol;
  symbol = (klangc_symbol_t *)klangc_malloc(sizeof(klangc_symbol_t));
  symbol->ks_name = name;
  klangc_hash_put(g_symbol_hash, name, symbol, NULL);
  return symbol;
}

klangc_parse_result_t klangc_symbol_parse(klangc_input_t *input,
                                          const klangc_symbol_t **psymbol) {
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
  const klangc_str_t *name_str = klangc_str_new(name, len);
  *psymbol = klangc_symbol_new(name_str);
  return KLANGC_PARSE_OK;
}

const klangc_str_t *klangc_symbol_get_name(const klangc_symbol_t *symbol) {
  assert(symbol != NULL);
  return symbol->ks_name;
}

void klangc_symbol_print(klangc_output_t *output,
                         const klangc_symbol_t *symbol) {
  assert(output != NULL);
  assert(symbol != NULL);
  klangc_printf(output, "%s", klangc_str_get_cstr(symbol->ks_name));
}
