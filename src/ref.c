#include "ref.h"
#include "input.h"
#include "output.h"
#include "parse.h"
#include "symbol.h"
#include <assert.h>

klangc_parse_result_t klangc_ref_parse(klangc_input_t *input,
                                       klangc_symbol_t **psym) {
  assert(input != NULL);
  assert(psym != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_expect(input, '~', NULL);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  klangc_symbol_t *sym;
  ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_symbol_parse(input, &sym);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(kstderr, ipos_ss, "<symbol>", klangc_getc(input),
                               "<ref> ::= '~' ^<symbol>;");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *psym = sym;
  return KLANGC_PARSE_OK;
}

void klangc_ref_print(klangc_output_t *output, klangc_symbol_t *sym) {
  assert(output != NULL);
  assert(sym != NULL);
  klangc_printf(output, "~");
  klangc_symbol_print(output, sym);
}