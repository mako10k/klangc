#include "klangc_input.h"

char *klangc_symbol_parse(klangc_input_t *input) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos2 = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (!isalpha(c) && c != '_') {
    klangc_input_restore(input, ipos);
    return NULL;
  }
  char *symbol = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  symbol[len++] = c;
  while (1) {
    ipos2 = klangc_input_save(input);
    c = klangc_getc(input);
    if (!isalnum(c) && c != '_') {
      klangc_input_restore(input, ipos2);
      break;
    }
    if (cap <= len + 1) {
      cap *= 2;
      symbol = (char *)klangc_realloc(symbol, cap);
    }
    symbol[len++] = c;
  }
  symbol[len] = '\0';
  symbol = (char *)klangc_realloc(symbol, len + 1);
  return symbol;
}

int klangc_int_parse(klangc_input_t *input, int *intval) {
  int val = 0;
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos2 = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (!isdigit(c)) {
    klangc_input_restore(input, ipos);
    return 0;
  }
  do {
    val = val * 10 + (c - '0');
    ipos2 = klangc_input_save(input);
    c = klangc_getc(input);
  } while (isdigit(c));
  klangc_input_restore(input, ipos2);
  *intval = val;
  return 1;
}

char *klangc_string_parse(klangc_input_t *input) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos2 = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '"') {
    klangc_input_restore(input, ipos);
    return NULL;
  }
  char *str = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  while (1) {
    c = klangc_getc(input);
    if (c == EOF) {
      klangc_printf(kstderr, "Unexpected EOF\n");
      klangc_input_restore(input, ipos);
      return NULL;
    }
    if (c == '"')
      break;
    if (cap <= len + 1) {
      cap *= 2;
      str = (char *)klangc_realloc(str, cap);
    }
    str[len++] = c;
  }
  str[len] = '\0';
  str = (char *)klangc_realloc(str, len + 1);
  return str;
}