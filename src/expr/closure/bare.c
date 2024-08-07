
#include "bare.h"
#include "../../input.h"
#include "../closure.h"
#include "entry.h"
#include <assert.h>

klangc_parse_result_t
klangc_expr_closure_bare_parse(klangc_input_t *input, klangc_closure_t *upper,
                               klangc_closure_t **pclosure) {
  assert(input != NULL);
  assert(pclosure != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_closure_t *closure = NULL;
  klangc_expr_closure_entry_t *ent_prev = NULL;
  while (1) {
    klangc_expr_closure_entry_t *ent;
    klangc_ipos_t ipos2 = klangc_input_save(input);
    klangc_ipos_t ipos_ss = klangc_skipspaces(input);
    switch (klangc_expr_closure_entry_parse(input, upper, &ent)) {
    case KLANGC_PARSE_OK:
      if (closure == NULL)
        closure = klangc_closure_new(ipos_ss, upper);
      if (ent_prev != NULL)
        klangc_expr_closure_entry_set_next(ent, ent_prev);
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

static int klangc_expr_closure_entry_print_forall(
    klangc_closure_t *closure, klangc_expr_closure_entry_t *ent, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  klangc_expr_closure_entry_print(output, ent);
  return 0;
}

void klangc_expr_closure_bare_print(klangc_output_t *output,
                                    klangc_closure_t *closure) {
  klangc_closure_foreach_ent(closure, klangc_expr_closure_entry_print_forall,
                             output);
}
