#include "klangc_closure.h"
#include "klangc_bare_closure.h"
#include "klangc_closure_ent.h"
#include "klangc_hash.h"
#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_ref.h"
#include "klangc_types.h"

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
  switch (klangc_bare_closure_parse(input, upper, &closure)) {
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
  klangc_bare_closure_print(output, closure);
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}

void klangc_closure_set_ent_first(klangc_closure_t *closure,
                                  klangc_closure_ent_t *ent) {
  closure->kc_ent = ent;
}

int klangc_closure_get_bind(klangc_closure_t *closure, klangc_ref_t *ref,
                            klangc_bind_t **pbind,
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
                            klangc_bind_t *bind) {
  const char *name = klangc_ref_get_name(ref);
  if (klangc_closure_get_bind(closure, ref, NULL, NULL) != 0) {
    klangc_printf(kstderr, "Duplicate definition: %s\n", name);
    return -1;
  }
  klangc_hash_put(closure->kc_bind_ref, name, bind, NULL);
  return 0;
}

int klangc_closure_walk(klangc_closure_t *closure,
                        klangc_bare_closure_walk_func_t func, void *data) {
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

typedef struct klangc_closure_walk_bind_data {
  klangc_bare_closure_walk_bind_func_t kcwbd_func;
  void *kcwbd_data;
} klangc_closure_walk_bind_data_t;

int klangc_closure_walk_bind_for_walk(klangc_closure_t *closure,
                                      klangc_closure_ent_t *ent, void *data) {

  struct klangc_closure_walk_bind_data *fdata =
      (struct klangc_closure_walk_bind_data *)data;
  if (!klangc_closure_ent_isbind(ent))
    return 0;
  return fdata->kcwbd_func(closure, klangc_closure_ent_get_bind(ent),
                           fdata->kcwbd_data);
}

int klangc_closure_walk_bind(klangc_closure_t *closure,
                             klangc_bare_closure_walk_bind_func_t func,
                             void *data) {
  struct klangc_closure_walk_bind_data fdata = {func, data};
  return klangc_closure_walk(closure, klangc_closure_walk_bind_for_walk,
                             &fdata);
}

typedef struct klangc_closure_walk_lambda_data {
  klangc_bare_closure_walk_lambda_func_t kcwld_func;
  void *kcwld_data;
} klangc_closure_walk_lambda_data_t;

int klangc_closure_walk_lambda_for_walk(klangc_closure_t *closure,
                                        klangc_closure_ent_t *ent, void *data) {
  struct klangc_closure_walk_lambda_data *fdata =
      (struct klangc_closure_walk_lambda_data *)data;
  if (!klangc_closure_ent_islambda(ent))
    return 0;
  return fdata->kcwld_func(closure, klangc_closure_ent_get_lambda(ent),
                           fdata->kcwld_data);
}

int klangc_closure_walk_lambda(klangc_closure_t *closure,
                               klangc_bare_closure_walk_lambda_func_t func,
                               void *data) {
  struct klangc_closure_walk_lambda_data fdata = {func, data};
  return klangc_closure_walk(closure, klangc_closure_walk_lambda_for_walk,
                             &fdata);
}
