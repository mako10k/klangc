
#include "klangc_bare_closure.h"
#include "klangc_closure.h"
#include "klangc_closure_ent.h"
#include "klangc_input.h"
#include "klangc_types.h"
#include <stdio.h>
#include <stdlib.h>

klangc_parse_result_t klangc_bare_closure_parse(klangc_input_t *input,
                                                klangc_closure_t *upper,
                                                klangc_closure_t **pclosure) {
  assert(input != NULL);
  assert(pclosure != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_closure_t *closure = NULL;
  klangc_closure_ent_t *ent_prev = NULL;
  while (1) {
    klangc_closure_ent_t *ent;
    klangc_ipos_t ipos2 = klangc_input_save(input);
    klangc_ipos_t ipos_ss = klangc_skipspaces(input);
    switch (klangc_closure_ent_parse(input, upper, &ent)) {
    case KLANGC_PARSE_OK:
      if (closure == NULL)
        closure = klangc_closure_new(ipos_ss, upper);
      if (ent_prev != NULL)
        klangc_closure_ent_set_next(ent, ent_prev);
      else
        klangc_closure_set_ent_first(closure, ent);
      ent_prev = ent;
      break;

    case KLANGC_PARSE_NOPARSE:
      if (closure == NULL)
        closure = klangc_closure_new(ipos_ss, upper);
      klangc_input_restore(input, ipos2);
      *pclosure = closure;
      return KLANGC_PARSE_OK;

    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  }
}

static int klangc_closure_ent_print_forall(klangc_closure_t *closure,
                                           klangc_closure_ent_t *ent,
                                           void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_closure_ent_print(output, ent);
  return 0;
}

void klangc_bare_closure_print(klangc_output_t *output,
                               klangc_closure_t *closure) {
  klangc_closure_foreach_ent(closure, klangc_closure_ent_print_forall, output);
}
