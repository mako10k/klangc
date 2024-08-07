#include "closure.h"
#include "../expr.h"
#include "../hash.h"
#include "../input.h"
#include "../malloc.h"
#include "../output.h"
#include "../parse.h"
#include "../pat.h"
#include "../pat/ref.h"
#include "../ref.h"
#include "closure/bare.h"
#include "closure/bind.h"
#include "closure/entry.h"
#include "lambda.h"

struct klangc_closure {
  klangc_hash_t *kc_bind_ref;
  klangc_closure_ent_t *kc_ent;
  klangc_closure_t *kc_upper;
  klangc_ipos_t kc_ipos;
};

klangc_closure_t *klangc_closure_new(klangc_ipos_t ipos,
                                     klangc_closure_t *upper) {
  klangc_closure_t *closure = klangc_malloc(sizeof(klangc_closure_t));
  closure->kc_bind_ref = klangc_hash_new(16);
  closure->kc_ent = NULL;
  closure->kc_ipos = ipos;
  closure->kc_upper = upper;
  return closure;
}

klangc_parse_result_t klangc_closure_parse(klangc_input_t *input,
                                           klangc_closure_t *upper,
                                           klangc_closure_t **pclosure) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, '{', &c)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_closure_t *closure;
  switch (klangc_expr_closure_bare_parse(input, upper, &closure)) {
  case KLANGC_PARSE_OK:
    break;

  case KLANGC_PARSE_NOPARSE:
    assert(0);

  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  ipos_ss = klangc_skipspaces(input);
  if (klangc_expect(input, '}', &c)) {
    *pclosure = closure;
    return KLANGC_PARSE_OK;
  }
  klangc_ipos_print(kstderr, ipos_ss);
  klangc_printf(kstderr, "expect '}' but get '%c': ['{' <closure> ^'}']\n", c);
  klangc_input_restore(input, ipos);
  return KLANGC_PARSE_ERROR;
}

void klangc_closure_print(klangc_output_t *output, klangc_closure_t *closure) {
  klangc_printf(output, "{\n");
  klangc_indent(output, 2);
  klangc_expr_closure_bare_print(output, closure);
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}

void klangc_closure_set_ent_first(klangc_closure_t *closure,
                                  klangc_closure_ent_t *ent) {
  closure->kc_ent = ent;
}

int klangc_closure_get_bind(klangc_closure_t *closure, klangc_ref_t *ref,
                            klangc_expr_closure_bind_t **pbind,
                            klangc_closure_t **pclosure) {
  while (closure != NULL) {
    if (klangc_hash_get(closure->kc_bind_ref, klangc_ref_get_name(ref),
                        (void **)pbind) != 0) {
      if (pclosure != NULL)
        *pclosure = closure;
      return 1;
    }
    closure = closure->kc_upper;
  }
  return 0;
}

int klangc_closure_put_bind(klangc_closure_t *closure, klangc_ref_t *ref,
                            klangc_expr_closure_bind_t *bind) {
  const char *name = klangc_ref_get_name(ref);
  klangc_expr_closure_bind_t *bind_ref;
  if (klangc_closure_get_bind(closure, ref, &bind_ref, NULL) != 0) {
    klangc_ipos_print(kstderr, klangc_ref_get_ipos(ref));
    klangc_printf(kstderr, "Duplicate definition: %s\n", name);
    klangc_ipos_print(kstderr, klangc_expr_closure_bind_get_ipos(bind_ref));
    klangc_printf(kstderr, "  previous definition: %s\n", name);
    return -1;
  }
  klangc_hash_put(closure->kc_bind_ref, name, bind, NULL);
  return 0;
}

int klangc_closure_foreach_ent(klangc_closure_t *closure,
                               klangc_closure_each_ent_func_t func,
                               void *data) {
  klangc_closure_ent_t *ent = closure->kc_ent;
  int cnt = 0, ret;
  while (ent != NULL) {
    ret = func(closure, ent, data);
    if (ret < 0)
      return -1;
    ent = klangc_closure_ent_get_next(ent);
    cnt += ret;
  }
  return cnt;
}

typedef struct klangc_closure_foreach_bind_data {
  klangc_closure_each_bind_func_t kcwbd_func;
  void *kcwbd_data;
} klangc_closure_walk_bind_data_t;

int klangc_closure_ent_foreach_bind(klangc_closure_t *closure,
                                    klangc_closure_ent_t *ent, void *data) {

  klangc_closure_walk_bind_data_t *fdata =
      (klangc_closure_walk_bind_data_t *)data;
  if (!klangc_closure_ent_isbind(ent))
    return 0;
  return fdata->kcwbd_func(closure, klangc_closure_ent_get_bind(ent),
                           fdata->kcwbd_data);
}

int klangc_closure_foreach_bind(klangc_closure_t *closure,
                                klangc_closure_each_bind_func_t func,
                                void *data) {
  struct klangc_closure_foreach_bind_data fdata = {func, data};
  return klangc_closure_foreach_ent(closure, klangc_closure_ent_foreach_bind,
                                    &fdata);
}

typedef struct klangc_closure_foreach_lambda_data {
  klangc_closure_each_lambda_func_t kcwld_func;
  void *kcwld_data;
} klangc_closure_foreach_lambda_data_t;

int klangc_closure_ent_forach_lambda(klangc_closure_t *closure,
                                     klangc_closure_ent_t *ent, void *data) {
  klangc_closure_foreach_lambda_data_t *fdata =
      (klangc_closure_foreach_lambda_data_t *)data;
  if (!klangc_closure_ent_islambda(ent))
    return 0;
  return fdata->kcwld_func(closure, klangc_closure_ent_get_lambda(ent),
                           fdata->kcwld_data);
}

int klangc_closure_foreach_lambda(klangc_closure_t *closure,
                                  klangc_closure_each_lambda_func_t func,
                                  void *data) {
  struct klangc_closure_foreach_lambda_data fdata = {func, data};
  return klangc_closure_foreach_ent(closure, klangc_closure_ent_forach_lambda,
                                    &fdata);
}

typedef struct klangc_closure_foreach_bind_local_data {
  klangc_closure_t *closure;
  klangc_expr_closure_bind_t *bind;
} klangc_closure_foreach_bind_local_data_t;

int klangc_closure_put_bind_foreach(klangc_pattern_ref_t *ref, void *data) {
  klangc_closure_foreach_bind_local_data_t *fdata =
      (klangc_closure_foreach_bind_local_data_t *)data;
  if (klangc_pattern_ref_is_used(ref))
    return 0;
  int ret = klangc_closure_put_bind(
      fdata->closure, klangc_pattern_ref_get_ref(ref), fdata->bind);
  klangc_pattern_ref_set_used(ref);
  return ret;
}

int klangc_closure_bind_locals_foreach(klangc_closure_t *closure,
                                       klangc_expr_closure_bind_t *bind,
                                       void *data) {
  klangc_pattern_t *pat = klangc_expr_closure_bind_get_pat(bind);
  klangc_closure_foreach_bind_local_data_t fdata = {closure, bind};
  return klangc_pattern_foreach_ref(pat, klangc_closure_put_bind_foreach,
                                    &fdata);
}

int klangc_closure_bind_locals(klangc_closure_t *closure) {
  klangc_closure_foreach_bind(closure, klangc_closure_bind_locals_foreach,
                              NULL);
  return 0;
}

int klangc_closure_bind_inners_foreach(klangc_closure_t *closure,
                                       klangc_closure_ent_t *ent, void *data) {
  if (klangc_closure_ent_islambda(ent))
    return klangc_lambda_bind(closure, klangc_closure_ent_get_lambda(ent));
  if (klangc_closure_ent_isbind(ent))
    return klangc_expr_bind(closure, klangc_expr_closure_bind_get_expr(
                                         klangc_closure_ent_get_bind(ent)));
  return -1;
}

int klangc_closure_bind_inners(klangc_closure_t *closure) {
  klangc_closure_foreach_ent(closure, klangc_closure_bind_inners_foreach, NULL);
  return 0;
}

int klangc_closure_bind(klangc_closure_t *closure) {
  klangc_closure_bind_locals(closure);
  klangc_closure_bind_inners(closure);
  return 0;
}

int klangc_closure_check_unbound_foreach(klangc_closure_t *closure,
                                         klangc_closure_ent_t *ent,
                                         void *data) {
  klangc_output_t *output = data;
  if (klangc_closure_ent_islambda(ent)) {
    klangc_lambda_t *lambda = klangc_closure_ent_get_lambda(ent);
    klangc_expr_t *body = klangc_lambda_get_body(lambda);
    if (klangc_expr_check_unbound(output, closure, body) != 0)
      return 1;
  }
  if (klangc_closure_ent_isbind(ent)) {
    klangc_expr_closure_bind_t *bind = klangc_closure_ent_get_bind(ent);
    klangc_expr_t *expr = klangc_expr_closure_bind_get_expr(bind);
    if (klangc_expr_check_unbound(output, closure, expr) != 0)
      return 1;
  }
  return 0;
}

int klangc_closure_check_unbound(klangc_output_t *output,
                                 klangc_closure_t *closure) {
  klangc_closure_foreach_ent(closure, klangc_closure_check_unbound_foreach,
                             output);
  return 0;
}

klangc_closure_ent_t *klangc_closure_get_ent_first(klangc_closure_t *closure) {
  assert(closure != NULL);
  return closure->kc_ent;
}
