#include "klangc_bind.h"
#include "klangc_closure.h"
#include "klangc_expr.h"
#include "klangc_pattern.h"

#include <assert.h>

struct klangc_lambda {
  klangc_pattern_t *kvl_arg;
  klangc_expr_t *kvl_body;
  klangc_closure_t *kvl_closure;
  klangc_ipos_t kvl_ipos;
};

klangc_lambda_t *klangc_expr_lambda_new(klangc_pattern_t *arg,
                                        klangc_expr_t *body,
                                        klangc_closure_t *upper,
                                        klangc_ipos_t ipos) {
  assert(arg != NULL);
  assert(body != NULL);
  klangc_lambda_t *lambda = klangc_malloc(sizeof(klangc_lambda_t));
  lambda->kvl_arg = arg;
  lambda->kvl_body = body;
  lambda->kvl_closure = klangc_closure_new(ipos, upper);
  lambda->kvl_ipos = ipos;
  klangc_bind_t *bind = klangc_bind_new(arg, body, ipos);
  klangc_pattern_walkvars(lambda->kvl_closure, bind, arg,
                          klangc_closure_put_bind_by_name);
  return lambda;
}

void klangc_expr_lambda_print(klangc_output_t *output,
                              klangc_lambda_t *lambda) {
  klangc_printf(output, "\\");
  klangc_pattern_print(output, KLANGC_PREC_LOWEST, lambda->kvl_arg);
  klangc_printf(output, " -> ");
  klangc_expr_print(output, KLANGC_PREC_CHOICE + 1, lambda->kvl_body);
}

klangc_pattern_t *klangc_expr_lambda_get_arg(klangc_lambda_t *lambda) {
  return lambda->kvl_arg;
}

klangc_expr_t *klangc_expr_lambda_get_body(klangc_lambda_t *lambda) {
  return lambda->kvl_body;
}

klangc_closure_t *klangc_expr_lambda_get_upper(klangc_lambda_t *lambda) {
  return lambda->kvl_closure;
}

klangc_ipos_t klangc_expr_lambda_get_ipos(klangc_lambda_t *lambda) {
  return lambda->kvl_ipos;
}