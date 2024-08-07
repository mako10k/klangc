#include "entry.h"
#include "../../malloc.h"
#include "../../output.h"
#include "../../parse.h"
#include "../lambda.h"
#include "bind.h"
#include <assert.h>

typedef enum klangc_expr_closure_entry_type {
  KLANGC_ECETYPE_BIND,
  KLANGC_ECETYPE_LAMBDA,
} klangc_expr_closure_entry_type_t;

struct klangc_expr_closure_entry {
  klangc_expr_closure_entry_type_t kce_type;
  union {
    klangc_expr_closure_bind_t *kce_bind;
    klangc_lambda_t *kce_lambda;
  };
  klangc_expr_closure_entry_t *kce_next;
};

klangc_expr_closure_entry_t *
klangc_expr_closure_entry_new_bind(klangc_expr_closure_bind_t *bind) {
  assert(bind != NULL);
  klangc_expr_closure_entry_t *ent =
      klangc_malloc(sizeof(klangc_expr_closure_entry_t));
  ent->kce_type = KLANGC_ECETYPE_BIND;
  ent->kce_bind = bind;
  ent->kce_next = NULL;
  return ent;
}

klangc_expr_closure_entry_t *
klangc_expr_closure_entry_new_lambda(klangc_lambda_t *lambda) {
  assert(lambda != NULL);
  klangc_expr_closure_entry_t *ent =
      klangc_malloc(sizeof(klangc_expr_closure_entry_t));
  ent->kce_type = KLANGC_ECETYPE_LAMBDA;
  ent->kce_lambda = lambda;
  ent->kce_next = NULL;
  return ent;
}

int klangc_expr_closure_entry_isbind(klangc_expr_closure_entry_t *ent) {
  assert(ent != NULL);
  return ent->kce_type == KLANGC_ECETYPE_BIND;
}

int klangc_expr_closure_entry_islambda(klangc_expr_closure_entry_t *ent) {
  assert(ent != NULL);
  return ent->kce_type == KLANGC_ECETYPE_LAMBDA;
}

klangc_expr_closure_bind_t *
klangc_expr_closure_entry_get_bind(klangc_expr_closure_entry_t *ent) {
  assert(ent != NULL);
  assert(klangc_expr_closure_entry_isbind(ent));
  return ent->kce_bind;
}

klangc_lambda_t *
klangc_expr_closure_entry_get_lambda(klangc_expr_closure_entry_t *ent) {
  assert(ent != NULL);
  assert(klangc_expr_closure_entry_islambda(ent));
  return ent->kce_lambda;
}

klangc_expr_closure_entry_t *
klangc_expr_closure_entry_get_next(klangc_expr_closure_entry_t *ent) {
  assert(ent != NULL);
  return ent->kce_next;
}

void klangc_expr_closure_entry_set_next(klangc_expr_closure_entry_t *ent,
                                        klangc_expr_closure_entry_t *ent_prev) {
  assert(ent != NULL);
  ent_prev->kce_next = ent;
}

klangc_parse_result_t
klangc_expr_closure_entry_parse(klangc_input_t *input,
                                klangc_expr_closure_t *upper,
                                klangc_expr_closure_entry_t **pent) {
  assert(input != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_skipspaces(input);
  klangc_expr_closure_entry_t *ent = NULL;
  klangc_expr_closure_bind_t *bind;
  switch (klangc_expr_closure_bind_parse(input, &bind)) {
  case KLANGC_PARSE_OK:
    ent = klangc_expr_closure_entry_new_bind(bind);
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  if (ent == NULL) {
    klangc_lambda_t *lambda;
    switch (klangc_lambda_parse(input, upper, &lambda)) {
    case KLANGC_PARSE_OK:
      ent = klangc_expr_closure_entry_new_lambda(lambda);
    case KLANGC_PARSE_NOPARSE:
      break;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  }

  if (ent == NULL) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }

  int c;
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, ';', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "Expected ';', got '%c'\n", c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  if (pent != NULL)
    *pent = ent;
  return KLANGC_PARSE_OK;
}

void klangc_expr_closure_entry_print(klangc_output_t *output,
                                     klangc_expr_closure_entry_t *ent) {
  switch (ent->kce_type) {
  case KLANGC_ECETYPE_BIND:
    klangc_expr_closure_bind_print(output, ent->kce_bind);
    break;
  case KLANGC_ECETYPE_LAMBDA:
    klangc_lambda_print(output, ent->kce_lambda);
    break;
  }
  klangc_printf(output, ";\n");
}
