#include "parse.h"
#include "malloc.h"
#include "output.h"

klangc_parse_result_t klangc_int_parse(klangc_input_t *input, int *pintval) {
  int intval = 0;
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (!isdigit(c)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  do {
    intval = intval * 10 + (c - '0');
    ipos_ss = klangc_input_save(input);
    c = klangc_getc(input);
  } while (isdigit(c));
  klangc_input_restore(input, ipos_ss);
  *pintval = intval;
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_string_parse(klangc_input_t *input,
                                          char **pstrval) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '"') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  char *strval = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  while (1) {
    c = klangc_getc(input);
    if (c == EOF) {
      klangc_printf(kstderr, "Unexpected EOF\n");
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    if (c == '"')
      break;
    if (cap <= len + 1) {
      cap *= 2;
      strval = (char *)klangc_realloc(strval, cap);
    }
    strval[len++] = c;
  }
  strval[len] = '\0';
  strval = (char *)klangc_realloc(strval, len + 1);
  *pstrval = strval;
  return KLANGC_PARSE_OK;
}

int klangc_expect(klangc_input_t *input, int c, int *pc) {
  assert(input != NULL);
  assert(c == EOF || (0 <= c && c <= 255));
  klangc_ipos_t ipos = klangc_input_save(input);
  int c2 = klangc_getc(input);
  if (c2 != c) {
    klangc_input_restore(input, ipos);
    if (pc != NULL)
      *pc = c2;
    return 0;
  }
  return 1;
}