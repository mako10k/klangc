
#include "ref.h"
#include "malloc.h"
#include "output.h"
#include "symbol.h"

struct klangc_ref {
  klangc_symbol_t *kr_symbol;
  klangc_ipos_t kr_ipos;
};

klangc_ref_t *klangc_ref_new(klangc_symbol_t *symbol, klangc_ipos_t ipos) {
  assert(symbol != NULL);
  klangc_ref_t *ref = (klangc_ref_t *)klangc_malloc(sizeof(klangc_ref_t));
  ref->kr_symbol = symbol;
  ref->kr_ipos = ipos;
  return ref;
}

klangc_symbol_t *klangc_ref_get_symbol(klangc_ref_t *ref) {
  assert(ref != NULL);
  return ref->kr_symbol;
}

const char *klangc_ref_get_name(klangc_ref_t *ref) {
  assert(ref != NULL);
  return klangc_symbol_get_name(ref->kr_symbol);
}

klangc_ipos_t klangc_ref_get_ipos(klangc_ref_t *ref) {
  assert(ref != NULL);
  return ref->kr_ipos;
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
  klangc_ipos_t ipos_ss2 = klangc_input_save(input);
  klangc_symbol_t *symbol;
  switch (klangc_symbol_parse(input, &symbol)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss2);
    klangc_printf(kstderr, "Expected symbol : ['~' ^<symbol>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos_ss2);
    return KLANGC_PARSE_ERROR;
  }
  if (pref != NULL)
    *pref = klangc_ref_new(symbol, ipos_ss);
  return KLANGC_PARSE_OK;
}

void klangc_ref_print(klangc_output_t *output, klangc_ref_t *ref) {
  assert(output != NULL);
  assert(ref != NULL);

  klangc_printf(output, "~");
  klangc_symbol_print(output, ref->kr_symbol);
}