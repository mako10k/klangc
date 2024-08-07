#include "lambda.h"
#include "../expr.h"
#include "../input.h"
#include "../malloc.h"
#include "../output.h"
#include "../parse.h"
#include "../pat.h"
#include "closure.h"
#include "closure/bind.h"
#include "closure/entry.h"
#include <assert.h>

struct klangc_lambda {
  klangc_pat_t *kvl_arg;
  klangc_expr_t *kvl_body;
  klangc_closure_t *kvl_closure;
  klangc_ipos_t kvl_ipos;
};

klangc_lambda_t *klangc_lambda_new(klangc_pat_t *arg, klangc_expr_t *body,
                                   klangc_closure_t *upper,
                                   klangc_ipos_t ipos) {
  assert(arg != NULL);
  assert(body != NULL);
  klangc_lambda_t *lambda = klangc_malloc(sizeof(klangc_lambda_t));
  lambda->kvl_arg = arg;
  lambda->kvl_body = body;
  lambda->kvl_closure = klangc_closure_new(ipos, upper);
  lambda->kvl_ipos = ipos;
  return lambda;
}

klangc_parse_result_t klangc_lambda_parse(klangc_input_t *input,
                                          klangc_closure_t *upper,
                                          klangc_lambda_t **plambda) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, '\\', &c)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_pat_t *arg;
  switch (klangc_pat_parse(input, &arg)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <pat>: ['\\' ^<pat> '->' <expr>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, '-', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect '-' but get '%c': ['\\' <pat> ^'->' <expr>]\n", c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  if (!klangc_expect(input, '>', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect '->' but get '-%c': ['\\' <pat> ^'->' "
                  "<expr>]\n",
                  c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *body;
  switch (klangc_expr_parse(input, upper, &body)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <expr>: ['\\' <pat> '->' ^<expr>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  *plambda = klangc_lambda_new(arg, body, upper, ipos_ss);
  return KLANGC_PARSE_OK;
}

void klangc_lambda_print(klangc_output_t *output, klangc_lambda_t *lambda) {
  klangc_printf(output, "\\");
  klangc_pat_print(output, KLANGC_PREC_LOWEST, lambda->kvl_arg);
  klangc_printf(output, " -> ");
  klangc_expr_print(output, KLANGC_PREC_LOWEST, lambda->kvl_body);
}

klangc_pat_t *klangc_lambda_get_arg(klangc_lambda_t *lambda) {
  return lambda->kvl_arg;
}

klangc_expr_t *klangc_lambda_get_body(klangc_lambda_t *lambda) {
  return lambda->kvl_body;
}

klangc_closure_t *klangc_lambda_get_upper(klangc_lambda_t *lambda) {
  return lambda->kvl_closure;
}

klangc_ipos_t klangc_lambda_get_ipos(klangc_lambda_t *lambda) {
  return lambda->kvl_ipos;
}

typedef struct klangc_lambda_put_arg_foreach_data {
  klangc_closure_t *closure;
  klangc_expr_closure_bind_t *bind;
} klangc_lambda_put_arg_foreach_data_t;

int klangc_lambda_put_arg_foreach(klangc_ref_t *ref, void *data) {
  klangc_lambda_put_arg_foreach_data_t *fdata = data;
  klangc_closure_put_bind(fdata->closure, ref, fdata->bind);
  return 1;
}

int klangc_lambda_bind(klangc_closure_t *closure, klangc_lambda_t *lambda) {
  klangc_pat_t *arg = lambda->kvl_arg;
  klangc_expr_t *body = lambda->kvl_body;
  klangc_expr_closure_bind_t *bind =
      klangc_expr_closure_bind_new(arg, body, lambda->kvl_ipos);
  klangc_closure_ent_t *ent = klangc_closure_ent_new_bind(bind);
  klangc_closure_set_ent_first(closure, ent);
  klangc_closure_bind(closure);
  klangc_expr_bind(closure, body);
  return 1;
}

int klangc_lambda_check_unbound(klangc_output_t *output,
                                klangc_lambda_t *lambda) {
  klangc_expr_t *body = klangc_lambda_get_body(lambda);
  return klangc_expr_check_unbound(output, klangc_lambda_get_upper(lambda),
                                   body);
}