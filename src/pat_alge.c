#include "pat_alge.h"
#include "list.h"
#include "malloc.h"
#include "output.h"
#include "pat.h"
#include "symbol.h"
#include "types.h"
#include <assert.h>

// *******************************
// Pattern Algebra.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Pattern algebra.
 */
struct klangc_pat_alge {
  /** Constructor */
  klangc_symbol_t *kpa_constr;
  /** Number of arguments */
  unsigned int kpa_argc;
  /** Arguments */
  klangc_pat_t **kpa_args;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_pat_alge_t *klangc_pat_alge_new(klangc_symbol_t *constr) {
  assert(constr != NULL);
  klangc_pat_alge_t *res = klangc_malloc(sizeof(klangc_pat_alge_t));
  res->kpa_constr = constr;
  res->kpa_argc = 0;
  res->kpa_args = NULL;
  return res;
}

void klangc_pat_alge_add_arg(klangc_pat_alge_t *palge, klangc_pat_t *arg) {
  assert(palge != NULL);
  assert(arg != NULL);
  klangc_pat_t **new_args = klangc_realloc(
      palge->kpa_args, sizeof(klangc_pat_t *) * (palge->kpa_argc + 1));
  new_args[palge->kpa_argc] = arg;
  palge->kpa_argc++;
  palge->kpa_args = new_args;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_symbol_t *klangc_pat_alge_get_constr(klangc_pat_alge_t *palge) {
  assert(palge != NULL);
  return palge->kpa_constr;
}

unsigned int klangc_pat_alge_get_argc(klangc_pat_alge_t *palge) {
  assert(palge != NULL);
  return palge->kpa_argc;
}

klangc_pat_t *klangc_pat_alge_get_arg(klangc_pat_alge_t *palge, int index) {
  assert(palge != NULL);
  assert(index >= 0);
  assert(index < palge->kpa_argc);
  return palge->kpa_args[index];
}

// -------------------------------
// Printers.
// -------------------------------
static void klangc_pat_alge_print_cons(klangc_output_t *output,
                                       klangc_pat_alge_t *palge) {
  assert(output != NULL);
  assert(palge != NULL);
  if (palge->kpa_constr == klangc_nil_symbol())
    return;
  klangc_pat_print(output, KLANGC_PREC_LOWEST, palge->kpa_args[0]);
  while (1) {
    palge = klangc_pat_get_alge(palge->kpa_args[1]);
    if (palge->kpa_constr == klangc_nil_symbol())
      return;
    klangc_printf(output, ", ");
    klangc_pat_print(output, KLANGC_PREC_LOWEST, palge->kpa_args[0]);
  }
}

static int klangc_pat_alge_is_list(klangc_pat_alge_t *palge) {
  assert(palge != NULL);
  while (1) {
    if (palge->kpa_constr == klangc_nil_symbol())
      return 1;
    if (palge->kpa_constr != klangc_cons_symbol())
      return 0;
    if (palge->kpa_argc != 2)
      return 0;
    klangc_pat_t *pat = palge->kpa_args[1];
    if (klangc_pat_get_type(pat) != KLANGC_PTYPE_ALGE)
      return 0;
    palge = klangc_pat_get_alge(pat);
  }
}

void klangc_pat_alge_print(klangc_output_t *output, int prec,
                           klangc_pat_alge_t *palge) {
  assert(output != NULL);
  assert(KLANGC_PREC_LOWEST <= prec);
  assert(prec <= KLANGC_PREC_HIGHEST);
  assert(palge != NULL);
  if (klangc_pat_alge_is_list(palge)) {
    klangc_printf(output, "[");
    klangc_pat_alge_print_cons(output, palge);
    klangc_printf(output, "]");
    return;
  }
  if (palge->kpa_argc == 0) {
    klangc_symbol_print(output, palge->kpa_constr);
    return;
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, "(");
  klangc_symbol_print(output, palge->kpa_constr);
  if (palge->kpa_argc == 0)
    return;
  for (int i = 0; i < palge->kpa_argc; i++) {
    klangc_printf(output, " ");
    klangc_pat_print(output, KLANGC_PREC_APPL + 1, palge->kpa_args[i]);
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, ")");
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_pat_alge_parse(klangc_input_t *input,
                                            klangc_pat_parse_opt_t ppopts,
                                            klangc_pat_alge_t **palge) {
  assert(input != NULL);
  assert(palge != NULL);
  klangc_parse_result_t res;
  klangc_symbol_t *constr;
  res = klangc_symbol_parse(input, &constr);
  if (res != KLANGC_PARSE_OK)
    return res;
  klangc_pat_alge_t *pat = klangc_pat_alge_new(constr);
  klangc_pat_t *arg;
  if (ppopts & KLANGC_PAT_PARSE_NOARG) {
    *palge = pat;
    return KLANGC_PARSE_OK;
  }
  while (1) {
    res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NOARG, &arg);
    if (res != KLANGC_PARSE_OK)
      break;
    klangc_pat_alge_add_arg(pat, arg);
  }
  if (res != KLANGC_PARSE_NOPARSE)
    return res;
  *palge = pat;
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_pat_alge_bind(klangc_expr_env_t *env,
                                          klangc_pat_alge_t *palge,
                                          klangc_expr_ref_target_t *target) {
  assert(env != NULL);
  assert(palge != NULL);
  assert(target != NULL);
  for (int i = 0; i < palge->kpa_argc; i++) {
    klangc_bind_result_t res = klangc_pat_bind(env, palge->kpa_args[i], target);
    if (res == KLANGC_BIND_ERROR)
      return res;
  }
  return KLANGC_BIND_OK;
}
