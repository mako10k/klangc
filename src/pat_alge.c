#include "pat_alge.h"
#include "malloc.h"
#include "output.h"
#include "pat.h"
#include "symbol.h"
#include "types.h"
#include <assert.h>

struct klangc_pat_alge {
  klangc_symbol_t *kpa_constr;
  int kpa_argc;
  klangc_pat_t **kpa_args;
};

klangc_pat_alge_t *klangc_pat_alge_new(klangc_symbol_t *constr) {
  assert(constr != NULL);
  klangc_pat_alge_t *res = klangc_malloc(sizeof(klangc_pat_alge_t));
  res->kpa_constr = constr;
  res->kpa_argc = 0;
  res->kpa_args = NULL;
  return res;
}

void klangc_pat_alge_add_arg(klangc_pat_alge_t *base, int argc,
                             klangc_pat_t **args) {
  assert(base != NULL);
  assert(base->kpa_argc + argc >= 0);
  assert(argc <= 0 || args != NULL);
  if (argc == 0)
    return;
  int old_argc = base->kpa_argc;
  int new_argc = base->kpa_argc + argc;
  klangc_pat_t **new_args =
      klangc_realloc(base->kpa_args, new_argc * sizeof(klangc_pat_t *));
  for (int i = 0; i < argc; i++)
    new_args[old_argc + i] = args[i];
  base->kpa_argc = new_argc;
  base->kpa_args = new_args;
}

klangc_symbol_t *klangc_pat_alge_get_constr(klangc_pat_alge_t *alge) {
  assert(alge != NULL);
  return alge->kpa_constr;
}

int klangc_pat_alge_get_argc(klangc_pat_alge_t *alge) {
  assert(alge != NULL);
  return alge->kpa_argc;
}

klangc_pat_t *klangc_pat_alge_get_arg(klangc_pat_alge_t *alge, int index) {
  assert(alge != NULL);
  assert(index >= 0);
  assert(index < alge->kpa_argc);
  return alge->kpa_args[index];
}

klangc_bind_result_t klangc_pat_alge_bind(klangc_expr_env_t *env,
                                          klangc_pat_alge_t *pat_appl,
                                          klangc_expr_ref_target_t *target) {
  assert(env != NULL);
  assert(pat_appl != NULL);
  assert(target != NULL);
  for (int i = 0; i < pat_appl->kpa_argc; i++) {
    klangc_bind_result_t res =
        klangc_pat_bind(env, pat_appl->kpa_args[i], target);
    if (res == KLANGC_BIND_ERROR)
      return res;
  }
  return KLANGC_BIND_OK;
}

void klangc_pat_alge_print(klangc_output_t *output, int prec,
                           klangc_pat_alge_t *pat_alge) {
  assert(output != NULL);
  assert(KLANGC_PREC_LOWEST <= prec);
  assert(prec <= KLANGC_PREC_HIGHEST);
  assert(pat_alge != NULL);
  if (pat_alge->kpa_argc == 0) {
    klangc_symbol_print(output, pat_alge->kpa_constr);
    return;
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, "(");
  klangc_symbol_print(output, pat_alge->kpa_constr);
  if (pat_alge->kpa_argc == 0)
    return;
  for (int i = 0; i < pat_alge->kpa_argc; i++) {
    klangc_printf(output, " ");
    klangc_pat_print(output, KLANGC_PREC_APPL + 1, pat_alge->kpa_args[i]);
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, ")");
}

klangc_parse_result_t klangc_pat_alge_parse(klangc_input_t *input, int noarg,
                                            klangc_pat_alge_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);
  klangc_parse_result_t res;
  klangc_symbol_t *constr;
  res = klangc_symbol_parse(input, &constr);
  if (res != KLANGC_PARSE_OK)
    return res;
  klangc_pat_alge_t *pat = klangc_pat_alge_new(constr);
  klangc_pat_t *arg;
  if (noarg) {
    *ppat = pat;
    return KLANGC_PARSE_OK;
  }
  while (1) {
    res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NOARG, &arg);
    if (res != KLANGC_PARSE_OK)
      break;
    klangc_pat_alge_add_arg(pat, 1, &arg);
  }
  if (res != KLANGC_PARSE_NOPARSE)
    return res;
  *ppat = pat;
  return KLANGC_PARSE_OK;
}