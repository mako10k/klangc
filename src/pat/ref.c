#include "ref.h"
#include "../malloc.h"
#include "../ref.h"
#include <assert.h>

struct klangc_pattern_ref {
  klangc_ref_t *ref;
  int used;
};

klangc_pattern_ref_t *klangc_pattern_ref_new(klangc_ref_t *ref) {
  klangc_pattern_ref_t *pattern_ref =
      klangc_malloc(sizeof(klangc_pattern_ref_t));
  pattern_ref->ref = ref;
  pattern_ref->used = 0;
  return pattern_ref;
}

klangc_ref_t *klangc_pattern_ref_get_ref(klangc_pattern_ref_t *pattern_ref) {
  return pattern_ref->ref;
}

const char *klangc_pattern_ref_get_name(klangc_pattern_ref_t *pattern_ref) {
  return klangc_ref_get_name(pattern_ref->ref);
}

klangc_ipos_t klangc_pattern_ref_get_ipos(klangc_pattern_ref_t *pattern_ref) {
  return klangc_ref_get_ipos(pattern_ref->ref);
}

int klangc_pattern_ref_is_used(klangc_pattern_ref_t *pattern_ref) {
  return pattern_ref->used;
}

void klangc_pattern_ref_set_used(klangc_pattern_ref_t *pattern_ref) {
  pattern_ref->used = 1;
}

klangc_parse_result_t
klangc_pattern_ref_parse(klangc_input_t *input,
                         klangc_pattern_ref_t **ppattern_ref) {
  klangc_ref_t *ref;
  switch (klangc_ref_parse(input, &ref)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    return KLANGC_PARSE_NOPARSE;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }
  klangc_pattern_ref_t *pattern_ref = klangc_pattern_ref_new(ref);
  *ppattern_ref = pattern_ref;
  return KLANGC_PARSE_OK;
}

void klangc_pattern_ref_print(klangc_output_t *output,
                              klangc_pattern_ref_t *pattern_ref) {
  klangc_ref_print(output, pattern_ref->ref);
}