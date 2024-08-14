#include "parse.h"
#include <assert.h>

// *******************************
// Parse.
// *******************************
// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_int_parse(klangc_input_t *input, int *pintval) {
  assert(input != NULL);
  assert(pintval != NULL);
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

klangc_parse_result_t klangc_expect(klangc_input_t *input, int c, int *pc) {
  assert(input != NULL);
  assert(c == EOF || (0 <= c && c <= 255));
  klangc_ipos_t ipos = klangc_input_save(input);
  int c2 = klangc_getc(input);
  if (c2 != c) {
    klangc_input_restore(input, ipos);
    if (pc != NULL)
      *pc = c2;
    return KLANGC_PARSE_NOPARSE;
  }
  return KLANGC_PARSE_OK;
}