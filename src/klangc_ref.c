
#include "klangc_ref.h"
#include "klangc_symbol.h"

struct klangc_ref {
  klangc_symbol_t *kr_symbol;
};

klangc_ref_t *klangc_ref_new(klangc_symbol_t *symbol) {
  assert(symbol != NULL);
  klangc_ref_t *ref = (klangc_ref_t *)klangc_malloc(sizeof(klangc_ref_t));
  ref->kr_symbol = symbol;
  return ref;
}

void klangc_ref_free(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_symbol_free(ref->kr_symbol);
  klangc_free(ref);
}

klangc_symbol_t *klangc_ref_get_symbol(klangc_ref_t *ref) {
  assert(ref != NULL);
  return ref->kr_symbol;
}

char *klangc_ref_get_name(klangc_ref_t *ref) {
  assert(ref != NULL);
  return klangc_symbol_get_name(ref->kr_symbol);
}

klangc_parse_result_t klangc_ref_parse(klangc_input_t *input,
                                       klangc_ref_t **pref) {
  assert(input != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '~') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_input_save(input);
  klangc_symbol_t *symbol;
  switch (klangc_symbol_parse(input, &symbol)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "Expected symbol : ['~' ^<symbol>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos_ss);
    return KLANGC_PARSE_ERROR;
  }
  if (pref != NULL)
    *pref = klangc_ref_new(symbol);
  return KLANGC_PARSE_OK;
}

void klangc_ref_print(klangc_output_t *output, klangc_ref_t *ref) {
  assert(output != NULL);
  assert(ref != NULL);

  klangc_printf(output, "~");
  klangc_symbol_print(output, ref->kr_symbol);
}