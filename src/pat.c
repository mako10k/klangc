
#include "pat.h"
#include "input.h"
#include "list.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "pat_alge.h"
#include "pat_as.h"
#include "pat_ref.h"
#include "str.h"
#include "tuple.h"
#include "types.h"
#include <assert.h>
#include <stdio.h>

// *******************************
// Pattern.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Pattern.
 */
struct klangc_pat {
  /** Type */
  klangc_pat_type_t kp_type;
  union {
    /** Reference */
    klangc_pat_ref_t *kp_pref;
    /** Algebraic */
    klangc_pat_alge_t *kp_alge;
    /** As */
    klangc_pat_as_t *kp_as;
    /** Integer value */
    int kp_intval;
    /** String value */
    const klangc_str_t *kp_strval;
  };
  /** Input position */
  klangc_ipos_t ipos;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_pat_t *klangc_pat_new_ref(klangc_pat_ref_t *pref, klangc_ipos_t ipos) {
  assert(pref != NULL);
  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_REF;
  pat->kp_pref = pref;
  pat->ipos = ipos;
  return pat;
}

klangc_pat_t *klangc_pat_new_alge(klangc_pat_alge_t *palge,
                                  klangc_ipos_t ipos) {
  assert(palge != NULL);
  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_ALGE;
  pat->kp_alge = palge;
  pat->ipos = ipos;
  return pat;
}

klangc_pat_t *klangc_pat_new_as(klangc_pat_ref_t *pref, klangc_pat_t *pat_as,
                                klangc_ipos_t ipos) {
  assert(pref != NULL);
  assert(pat_as != NULL);
  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_AS;
  pat->kp_as = klangc_pat_as_new(pref, pat_as);
  pat->ipos = ipos;
  return pat;
}

klangc_pat_t *klangc_pat_new_int(int intval, klangc_ipos_t ipos) {
  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_INT;
  pat->kp_intval = intval;
  pat->ipos = ipos;
  return pat;
}

klangc_pat_t *klangc_pat_new_string(const klangc_str_t *strval,
                                    klangc_ipos_t ipos) {
  assert(strval != NULL);
  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_STRING;
  pat->kp_strval = strval;
  pat->ipos = ipos;
  return pat;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_pat_type_t klangc_pat_get_type(klangc_pat_t *pat) {
  assert(pat != NULL);
  return pat->kp_type;
}

klangc_pat_ref_t *klangc_pat_get_ref(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(pat->kp_type == KLANGC_PTYPE_REF);
  return pat->kp_pref;
}

klangc_pat_alge_t *klangc_pat_get_alge(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(pat->kp_type == KLANGC_PTYPE_ALGE);
  return pat->kp_alge;
}

klangc_pat_as_t *klangc_pat_get_as(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(pat->kp_type == KLANGC_PTYPE_AS);
  return pat->kp_as;
}

int klangc_pat_get_int(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(pat->kp_type == KLANGC_PTYPE_INT);
  return pat->kp_intval;
}

const klangc_str_t *klangc_pat_get_str(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(pat->kp_type == KLANGC_PTYPE_STRING);
  return pat->kp_strval;
}

klangc_ipos_t klangc_pat_get_ipos(klangc_pat_t *pat) {
  assert(pat != NULL);
  return pat->ipos;
}

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a list of patterns.
 * @param input Input.
 * @param ppat Pattern.
 * @return Parse result.
 */
static klangc_parse_result_t klangc_pat_parse_list(klangc_input_t *input,
                                                   klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  klangc_parse_result_t res = klangc_expect(input, '[', NULL);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
  klangc_pat_alge_t *palge_nil = klangc_nil_pat_alge();
  klangc_pat_t *pat_nil = klangc_pat_new_alge(palge_nil, ipos_ss);
  res = klangc_expect(input, ']', NULL);
  switch (res) {
  case KLANGC_PARSE_OK:
    *ppat = pat_nil;
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_pat_t *pat;
  res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss2, "<pat> or ']'", klangc_getc(input),
        "<pat> ::= .. | '[' ^(<pat> (',' <pat>)*)? ']' | ..;\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  const klangc_symbol_t *sym_cons = klangc_cons_symbol();
  klangc_pat_alge_t *palge_hd = klangc_pat_alge_new(sym_cons);
  klangc_pat_alge_add_arg(palge_hd, pat);
  klangc_pat_t *pat_hd = klangc_pat_new_alge(palge_hd, ipos_ss2);
  klangc_pat_alge_t *palge_tl = palge_hd;
  klangc_pat_t *pat_tl = pat_hd;
  while (1) {
    ipos_ss2 = klangc_skipspaces(input);
    res = klangc_expect(input, ']', NULL);
    switch (res) {
    case KLANGC_PARSE_OK:
      klangc_pat_alge_add_arg(palge_tl, pat_nil);
      *ppat = pat_hd;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_NOPARSE:
      break;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    res = klangc_expect(input, ',', NULL);
    switch (res) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(
          kstderr, ipos_ss2, "',' or ']'", c,
          "<pat> ::= .. | '[' (<pat> ^(',' <pat>)*)? ']' | ..;\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    klangc_pat_t *pat;
    ipos_ss2 = klangc_skipspaces(input);
    res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat);
    switch (res) {
    case KLANGC_PARSE_OK: {
      klangc_pat_alge_t *palge_cons = klangc_pat_alge_new(sym_cons);
      klangc_pat_t *pat_cons = klangc_pat_new_alge(palge_cons, ipos_ss2);
      klangc_pat_alge_add_arg(palge_cons, pat);
      klangc_pat_alge_add_arg(palge_tl, pat_cons);
      palge_tl = palge_cons;
      pat_tl = pat_cons;
      break;
    }
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(
          kstderr, ipos_ss2, "<pat>", klangc_getc(input),
          "<pat> ::= .. | '[' (<pat> (',' ^<pat>)*)? ']' | ..;\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  }
}

static klangc_parse_result_t klangc_pat_parse_alge(klangc_input_t *input,
                                                   klangc_pat_t **ppat) {
  klangc_pat_alge_t *alge;
  klangc_parse_result_t res =
      klangc_pat_alge_parse(input, KLANGC_PAT_PARSE_NORMAL, &alge);
  if (res != KLANGC_PARSE_OK)
    return res;
  *ppat = klangc_pat_new_alge(alge, klangc_input_save(input));
  return KLANGC_PARSE_OK;
}

static klangc_parse_result_t klangc_pat_parse_ref(klangc_input_t *input,
                                                  klangc_pat_t **ppat) {
  klangc_pat_ref_t *ref;
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_pat_ref_parse(input, &ref);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  *ppat = klangc_pat_new_ref(ref, ipos_ss);
  return KLANGC_PARSE_OK;
}

static klangc_parse_result_t klangc_pat_parse_as(klangc_input_t *input,
                                                 klangc_pat_ref_t *ref,
                                                 klangc_pat_t **ppat) {
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_expect(input, '@', NULL);
  if (res != KLANGC_PARSE_OK)
    return KLANGC_PARSE_NOPARSE;
  klangc_pat_t *pat;
  res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NOARG, &pat);
  if (res != KLANGC_PARSE_OK)
    return res;
  *ppat = klangc_pat_new_as(ref, pat, ipos_ss);
  return KLANGC_PARSE_OK;
}

static klangc_parse_result_t klangc_pat_parse_int(klangc_input_t *input,
                                                  klangc_pat_t **ppat) {
  int intval;
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_int_parse(input, &intval);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  *ppat = klangc_pat_new_int(intval, ipos_ss);
  return KLANGC_PARSE_OK;
}

static klangc_parse_result_t klangc_pat_parse_string(klangc_input_t *input,
                                                     klangc_pat_t **ppat) {
  const klangc_str_t *strval;
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_str_parse(input, &strval);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  *ppat = klangc_pat_new_string(strval, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_pat_parse_nocons(klangc_input_t *input,
                                              klangc_pat_parse_opt_t ppopt,
                                              klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_parse_result_t res = klangc_pat_parse_tuple(input, ppat);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_pat_parse_list(input, ppat);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_pat_parse_alge(input, ppat);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  klangc_pat_t *pat_ref;
  res = klangc_pat_parse_ref(input, &pat_ref);
  switch (res) {
  case KLANGC_PARSE_OK:
    res = klangc_pat_parse_as(input, klangc_pat_get_ref(pat_ref), ppat);
    switch (res) {
    case KLANGC_PARSE_OK:
      return res;
    case KLANGC_PARSE_NOPARSE:
      *ppat = pat_ref;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }

  res = klangc_pat_parse_int(input, ppat);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_pat_parse_string(input, ppat);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  klangc_input_restore(input, ipos);
  return KLANGC_PARSE_NOPARSE;
}

static klangc_parse_result_t klangc_pat_parse_cons(klangc_input_t *input,
                                                   klangc_pat_t *pat_hd,
                                                   klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(pat_hd != NULL);
  assert(ppat != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_parse_result_t res = klangc_expect(input, ':', NULL);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  klangc_pat_t *pat_tl;
  res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat_tl);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(kstderr, ipos_ss, "<pat>", klangc_getc(input),
                               "<pat> ::= .. | <pat> ':' ^<pat> | ..;\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_pat_alge_t *palge = klangc_pat_alge_new(klangc_cons_symbol());
  klangc_pat_alge_add_arg(palge, pat_hd);
  klangc_pat_alge_add_arg(palge, pat_tl);
  *ppat = klangc_pat_new_alge(palge, klangc_input_save(input));
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_parse_opt_t ppopt,
                                       klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);
  klangc_pat_t *pat;
  klangc_parse_result_t res = klangc_pat_parse_nocons(input, ppopt, &pat);
  if (res != KLANGC_PARSE_OK)
    return res;
  res = klangc_pat_parse_cons(input, pat, ppat);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;
  *ppat = pat;
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_pat_print(klangc_output_t *output, int prec, klangc_pat_t *pat) {
  assert(output != NULL);
  assert(KLANGC_PREC_LOWEST <= prec);
  assert(prec <= KLANGC_PREC_HIGHEST);
  assert(pat != NULL);

  if (klangc_pat_is_tuple(pat)) {
    klangc_pat_print_tuple(output, pat);
    return;
  }
  switch (pat->kp_type) {
  case KLANGC_PTYPE_REF:
    klangc_pat_ref_print(output, pat->kp_pref);
    break;
  case KLANGC_PTYPE_ALGE:
    klangc_pat_alge_print(output, prec, pat->kp_alge);
    break;
  case KLANGC_PTYPE_AS:
    klangc_pat_as_print(output, pat->kp_as);
    break;
  case KLANGC_PTYPE_INT:
    klangc_printf(output, "%d", pat->kp_intval);
    break;
  case KLANGC_PTYPE_STRING:
    klangc_str_print(output, pat->kp_strval);
    break;
  }
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_pat_bind(klangc_expr_env_t *env, klangc_pat_t *pat,
                                     klangc_expr_ref_target_t *target) {
  assert(env != NULL);
  assert(pat != NULL);
  assert(target != NULL);
  switch (pat->kp_type) {
  case KLANGC_PTYPE_REF:
    return klangc_pat_ref_bind(env, pat->kp_pref, target);
  case KLANGC_PTYPE_ALGE:
    return klangc_pat_alge_bind(env, pat->kp_alge, target);
  case KLANGC_PTYPE_AS:
    return klangc_pat_as_bind(env, pat->kp_as, target);
  case KLANGC_PTYPE_INT:
  case KLANGC_PTYPE_STRING:
    return KLANGC_BIND_OK;
  }
  klangc_printf(kstderr, "UNKOWN ERROR at %s:%d\n", __FILE__, __LINE__);
  return KLANGC_BIND_ERROR;
}

// -------------------------------
// Misc.
// -------------------------------
int klangc_pat_foreach_ref(klangc_pat_t *pat,
                           klangc_pat_foreach_ref_func_t func, void *data) {
  assert(pat != NULL);
  assert(func != NULL);
  int ret = 0;
  int cnt = 0;
  if (pat->kp_type == KLANGC_PTYPE_REF) {
    ret = func(pat->kp_pref, data);
    if (ret < 0)
      return ret;
    cnt++;
  } else if (pat->kp_type == KLANGC_PTYPE_ALGE) {
    for (int i = 0; i < klangc_pat_alge_get_argc(pat->kp_alge); i++) {
      klangc_pat_t *arg = klangc_pat_alge_get_arg(pat->kp_alge, i);
      ret = klangc_pat_foreach_ref(arg, func, data);
      if (ret < 0)
        return ret;
      cnt += ret;
    }
  } else if (pat->kp_type == KLANGC_PTYPE_AS) {
    ret = func(klangc_pat_as_get_ref(pat->kp_as), data);
    if (ret < 0)
      return ret;
    cnt++;
    ret = klangc_pat_foreach_ref(klangc_pat_as_get_pat(pat->kp_as), func, data);
    if (ret < 0)
      return ret;
    cnt += ret;
  }
  return cnt;
}
