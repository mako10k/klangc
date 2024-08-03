#include "klangc_dict.h"
#include "klangc_closure.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
#include "klangc_message.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

struct klangc_dict {
  klangc_closure_t *kd_closure;
  klangc_ipos_t kd_ipos;
};

klangc_closure_t *klangc_dict_get_closure(klangc_dict_t *dict) {
  return dict->kd_closure;
}

klangc_dict_t *klangc_dict_new(klangc_closure_t *closure, klangc_ipos_t ipos) {
  klangc_dict_t *dict = klangc_malloc(sizeof(klangc_dict_t));
  dict->kd_closure = closure;
  dict->kd_ipos = ipos;
  return dict;
}

klangc_parse_result_t klangc_dict_parse(klangc_input_t *input,
                                        klangc_closure_t *upper,
                                        klangc_dict_t **pdict) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '{') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_closure_t *closure;
  switch (klangc_closure_parse(input, upper, &closure)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <def>: ['{' ^<def> '}']\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  ipos_ss = klangc_skipspaces(input);
  c = klangc_getc(input);
  if (c != '}') {
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect '}' but get '%c': ['{' <def> ^'}']\n", c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *pdict = klangc_dict_new(closure, ipos_ss);
  return KLANGC_PARSE_OK;
}

void klangc_dict_print(klangc_output_t *output, klangc_dict_t *dict) {
  klangc_printf(output, "{\n");
  klangc_indent(output, 2);
  klangc_closure_print(output, dict->kd_closure);
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}