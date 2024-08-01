#include "klangc_dict.h"
#include "klangc_def.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

struct klangc_dict {
  klangc_def_t *def;
  klangc_ipos_t ipos;
};

klangc_dict_t *klangc_dict_new(klangc_def_t *def, klangc_ipos_t ipos) {
  klangc_dict_t *dict = klangc_malloc(sizeof(klangc_dict_t));
  dict->def = def;
  dict->ipos = ipos;
  return dict;
}

klangc_parse_result_t klangc_dict_parse(klangc_input_t *input,
                                        klangc_dict_t **pdict) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '{') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_def_t *def;
  switch (klangc_def_parse(input, &def)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(input, "expect <def>: ['{' ^<def> '}']\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  ipos_ss = klangc_skipspaces(input);
  c = klangc_getc(input);
  if (c != '}') {
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(input, "expect '}' but get '%c': ['{' <def> ^'}']\n", c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *pdict = klangc_dict_new(def, ipos_ss);
  return KLANGC_PARSE_OK;
}

void klangc_dict_print(klangc_output_t *output, klangc_dict_t *dict) {
  klangc_printf(output, "{\n");
  klangc_indent(output, 2);
  klangc_def_print(output, dict->def);
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}