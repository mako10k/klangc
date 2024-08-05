#include "klangc_malloc.h"
#include "klangc_types.h"
#include <assert.h>

typedef enum klangc_closure_ent_type {
  KLANGC_CLOSURE_ENT_BIND,
  KLANGC_CLOSURE_ENT_LAMBDA,
} klangc_closure_ent_type_t;

struct klangc_closure_ent {
  klangc_closure_ent_type_t kce_type;
  union {
    klangc_bind_t *kce_bind;
    klangc_expr_lambda_t *kce_lambda;
  };
  klangc_closure_ent_t *kce_next;
};

klangc_closure_ent_t *klangc_closure_ent_new_bind(klangc_bind_t *bind) {
  assert(bind != NULL);
  klangc_closure_ent_t *ent = klangc_malloc(sizeof(klangc_closure_ent_t));
  ent->kce_type = KLANGC_CLOSURE_ENT_BIND;
  ent->kce_bind = bind;
  ent->kce_next = NULL;
  return ent;
}

klangc_closure_ent_t *
klangc_closure_ent_new_lambda(klangc_expr_lambda_t *lambda) {
  assert(lambda != NULL);
  klangc_closure_ent_t *ent = klangc_malloc(sizeof(klangc_closure_ent_t));
  ent->kce_type = KLANGC_CLOSURE_ENT_LAMBDA;
  ent->kce_lambda = lambda;
  ent->kce_next = NULL;
  return ent;
}

int klangc_closure_ent_isbind(klangc_closure_ent_t *ent) {
  assert(ent != NULL);
  return ent->kce_type == KLANGC_CLOSURE_ENT_BIND;
}

int klangc_closure_ent_islambda(klangc_closure_ent_t *ent) {
  assert(ent != NULL);
  return ent->kce_type == KLANGC_CLOSURE_ENT_LAMBDA;
}

klangc_bind_t *klangc_closure_ent_get_bind(klangc_closure_ent_t *ent) {
  assert(ent != NULL);
  assert(klangc_closure_ent_isbind(ent));
  return ent->kce_bind;
}

klangc_expr_lambda_t *klangc_closure_ent_get_lambda(klangc_closure_ent_t *ent) {
  assert(ent != NULL);
  assert(klangc_closure_ent_islambda(ent));
  return ent->kce_lambda;
}

klangc_closure_ent_t *klangc_closure_ent_get_next(klangc_closure_ent_t *ent) {
  assert(ent != NULL);
  return ent->kce_next;
}

void klangc_closure_ent_set_next(klangc_closure_ent_t *ent,
                                 klangc_closure_ent_t *ent_prev) {
  assert(ent != NULL);
  ent_prev->kce_next = ent;
}