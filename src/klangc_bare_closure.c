
#include "klangc_bare_closure.h"
#include "klangc_closure.h"
#include "klangc_closure_ent.h"
#include "klangc_expr.h"
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
        klangc_closure_set_ent(closure, ent);
      ent_prev = ent;
      break;

    case KLANGC_PARSE_NOPARSE:
      if (closure == NULL)
        closure = klangc_closure_new(ipos_ss, upper);
      klangc_input_restore(input, ipos2);
      *pclosure = closure;
      klangc_closure_walk_bind(closure, klangc_expr_bind_for_walk, NULL);
      return KLANGC_PARSE_OK;

    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  }
}

static int klangc_closure_ent_print_for_walk(klangc_closure_t *closure,
                                             klangc_closure_ent_t *ent,
                                             void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_closure_ent_print(output, ent);
  return 0;
}

void klangc_bare_closure_print(klangc_output_t *output,
                               klangc_closure_t *closure) {
  klangc_closure_walk(closure, klangc_closure_ent_print_for_walk, output);
}
