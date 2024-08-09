#include "ref_target.h"
#include "malloc.h"

struct klangc_expr_ref_target {
  klangc_expr_ref_target_type_t kert_type;
  union {
    klangc_bind_t *kert_bind;
    klangc_expr_lambda_t *kert_lambda;
  };
};

klangc_expr_ref_target_t *klangc_expr_ref_target_new_bind(klangc_bind_t *bind) {
  klangc_expr_ref_target_t *target =
      klangc_malloc(sizeof(klangc_expr_ref_target_t));
  target->kert_type = KLANGC_ERTTYPE_BIND;
  target->kert_bind = bind;
  return target;
}

klangc_expr_ref_target_t *
klangc_expr_ref_target_new_lambda(klangc_expr_lambda_t *lambda) {
  klangc_expr_ref_target_t *target =
      klangc_malloc(sizeof(klangc_expr_ref_target_t));
  target->kert_type = KLANGC_ERTTYPE_LAMBDA;
  target->kert_lambda = lambda;
  return target;
}

klangc_expr_ref_target_type_t
klangc_expr_ref_target_get_type(klangc_expr_ref_target_t *target) {
  return target->kert_type;
}

klangc_bind_t *
klangc_expr_ref_target_get_bind(klangc_expr_ref_target_t *target) {
  return target->kert_bind;
}

klangc_expr_lambda_t *
klangc_expr_ref_target_get_lambda(klangc_expr_ref_target_t *target) {
  return target->kert_lambda;
}