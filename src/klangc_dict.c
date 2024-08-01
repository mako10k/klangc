#include "klangc_dict.h"
#include "klangc_def.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
#include "klangc_output.h"
#include "klangc_types.h"

struct klangc_dict {
  klangc_hash_t *defs;
};

klangc_dict_t *klangc_dict_new(klangc_hash_t *defs) {
  klangc_dict_t *dict = klangc_malloc(sizeof(klangc_dict_t));
  dict->defs = defs;
  return dict;
}

klangc_dict_t *klangc_dict_parse(klangc_input_t *input) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos2 = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '{') {
    klangc_input_restore(input, ipos);
    return NULL;
  }
  ipos2 = klangc_skipspaces(input);
  klangc_hash_t *defs = klangc_def_parse(input);
  if (defs == NULL) {
    klangc_message_reset(input);
    klangc_message_add_ipos(input, &ipos2);
    klangc_message_add(input, "expect <def>: ['{' ^<def> '}']\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return NULL;
  }
  ipos2 = klangc_skipspaces(input);
  c = klangc_getc(input);
  if (c != '}') {
    klangc_message_reset(input);
    klangc_message_add_ipos(input, &ipos2);
    klangc_message_add(input, "expect '}' but get '%c': ['{' <def> ^'}']\n", c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return NULL;
  }
  return klangc_dict_new(defs);
}

void klangc_dict_print(klangc_output_t *output, klangc_dict_t *dict) {
  klangc_printf(output, "{\n");
  klangc_indent(output, 2);
  klangc_def_print(output, dict->defs);
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}