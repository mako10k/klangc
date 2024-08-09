#include "pat_ref.h"
#include "expr_env.h"
#include "malloc.h"
#include "ref.h"
#include "types.h"
#include <assert.h>

struct klangc_pat_ref {
  klangc_ref_t *ref;
  int used;
};

klangc_pat_ref_t *klangc_pat_ref_new(klangc_ref_t *ref) {
  klangc_pat_ref_t *pat_ref = klangc_malloc(sizeof(klangc_pat_ref_t));
  pat_ref->ref = ref;
  pat_ref->used = 0;
  return pat_ref;
}

klangc_ref_t *klangc_pat_ref_get_ref(klangc_pat_ref_t *pat_ref) {
  return pat_ref->ref;
}

const char *klangc_pat_ref_get_name(klangc_pat_ref_t *pat_ref) {
  return klangc_ref_get_name(pat_ref->ref);
}

klangc_ipos_t klangc_pat_ref_get_ipos(klangc_pat_ref_t *pat_ref) {
  return klangc_ref_get_ipos(pat_ref->ref);
}

int klangc_pat_ref_is_used(klangc_pat_ref_t *pat_ref) { return pat_ref->used; }

void klangc_pat_ref_set_used(klangc_pat_ref_t *pat_ref) { pat_ref->used = 1; }

klangc_parse_result_t klangc_pat_ref_parse(klangc_input_t *input,
                                           klangc_pat_ref_t **ppat_ref) {
  klangc_ref_t *ref;
  switch (klangc_ref_parse(input, &ref)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    return KLANGC_PARSE_NOPARSE;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }
  klangc_pat_ref_t *pat_ref = klangc_pat_ref_new(ref);
  *ppat_ref = pat_ref;
  return KLANGC_PARSE_OK;
}

void klangc_pat_ref_print(klangc_output_t *output, klangc_pat_ref_t *pat_ref) {
  klangc_ref_print(output, pat_ref->ref);
}

klangc_bind_result_t klangc_pat_ref_bind(klangc_expr_env_t *env,
                                         klangc_pat_ref_t *pat_ref,
                                         klangc_expr_ref_target_t *target) {
  if (klangc_pat_ref_is_used(pat_ref))
    return KLANGC_BIND_OK;
  klangc_ref_t *ref = klangc_pat_ref_get_ref(pat_ref);
  klangc_expr_env_put_entry(env, ref, target);
  klangc_pat_ref_set_used(pat_ref);
  return KLANGC_BIND_OK;
}