#include "klangc_eclosure.h"
#include "klangc_closure.h"
#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_types.h"

klangc_parse_result_t klangc_expr_closure_parse(klangc_input_t *input,
                                                klangc_closure_t *upper,
                                                klangc_closure_t **pclosure) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, '{', &c)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_closure_t *closure;
  switch (klangc_closure_parse(input, upper, &closure)) {
  case KLANGC_PARSE_OK:
    break;

  case KLANGC_PARSE_NOPARSE:
    assert(0);

  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  ipos_ss = klangc_skipspaces(input);
  if (klangc_expect(input, '}', &c)) {
    *pclosure = closure;
    return KLANGC_PARSE_OK;
  }
  klangc_ipos_print(kstderr, ipos_ss);
  klangc_printf(kstderr, "expect '}' but get '%c': ['{' <closure> ^'}']\n", c);
  klangc_input_restore(input, ipos);
  return KLANGC_PARSE_ERROR;
}

void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_closure_t *closure) {
  klangc_printf(output, "{\n");
  klangc_indent(output, 2);
  klangc_closure_print(output, closure);
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}