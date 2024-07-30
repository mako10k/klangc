#include "klangc_input.h"
char *klangc_symbol_parse(klangc_input_t *input) {
  klangc_input_buf_t ib = klangc_input_save(input);
  int c = klangc_getc_skipspaces(input);
  if (!isalpha(c) && c != '_') {
    klangc_input_restore(input, ib);
    return NULL;
  }
  char *symbol = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  symbol[len++] = c;
  while (1) {
    klangc_input_buf_t ib = klangc_input_save(input);
    c = klangc_getc(input);
    if (!isalnum(c) && c != '_') {
      klangc_input_restore(input, ib);
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
  klangc_input_buf_t ib = klangc_input_save(input);
  int c = klangc_getc_skipspaces(input);
  if (!isdigit(c)) {
    klangc_input_restore(input, ib);
    return 0;
  }
  do {
    val = val * 10 + (c - '0');
    ib = klangc_input_save(input);
    c = klangc_getc(input);
  } while (isdigit(c));
  klangc_input_restore(input, ib);
  *intval = val;
  return 1;
}

char *klangc_string_parse(klangc_input_t *input) {
  klangc_input_buf_t ib = klangc_input_save(input);
  int c = klangc_getc_skipspaces(input);
  if (c != '"') {
    klangc_input_restore(input, ib);
    return NULL;
  }
  char *str = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  while (1) {
    c = klangc_getc(input);
    if (c == EOF) {
      fprintf(stderr, "Unexpected EOF\n");
      klangc_input_restore(input, ib);
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