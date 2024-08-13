
#include "pat.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "pat_alge.h"
#include "pat_as.h"
#include "pat_ref.h"
#include "str.h"
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
    klangc_pat_ref_t *kp_ref;
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
  pat->kp_ref = pref;
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
  return pat->kp_ref;
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
klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_parse_opt_t ppopt,
                                       klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c == '(') {
    klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
    klangc_pat_t *pat;
    switch (klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &pat)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_ipos_print(kstderr, ipos_ss2);
      klangc_printf(kstderr, "expect <pat>: ['(' ^<pat> ')']\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss2 = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ')') {
      klangc_ipos_print(kstderr, ipos_ss2);
      klangc_printf(kstderr, "expect ')' but get '%c': ['(' <pat> ^')']\n", c);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *ppat = pat;
    return KLANGC_PARSE_OK;
  }
  klangc_input_restore(input, ipos_ss);

  klangc_pat_alge_t *alge;
  switch (klangc_pat_alge_parse(input, ppopt, &alge)) {
  case KLANGC_PARSE_OK:
    *ppat = klangc_pat_new_alge(alge, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_input_restore(input, ipos_ss);

  klangc_pat_ref_t *ref;
  switch (klangc_pat_ref_parse(input, &ref)) {
  case KLANGC_PARSE_OK: {
    klangc_pat_t *reference = klangc_pat_new_ref(ref, ipos_ss);
    klangc_ipos_t ipos_refend = klangc_input_save(input);
    klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != '@') {
      klangc_input_restore(input, ipos_refend);
      *ppat = reference;
      return KLANGC_PARSE_OK;
    }
    klangc_pat_t *pat;
    ipos_ss2 = klangc_skipspaces(input);
    switch (klangc_pat_parse(input, KLANGC_PAT_PARSE_NOARG, &pat)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_ipos_print(kstderr, ipos_ss2);
      klangc_printf(kstderr, "expect <pat>: [<ref> '@' ^<pat>]\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *ppat = klangc_pat_new_as(ref, pat, ipos_ss);
    return KLANGC_PARSE_OK;
  }
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  int intval;
  switch (klangc_int_parse(input, &intval)) {
  case KLANGC_PARSE_OK:
    *ppat = klangc_pat_new_int(intval, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  const klangc_str_t *strval;
  switch (klangc_str_parse(input, &strval)) {
  case KLANGC_PARSE_OK:
    *ppat = klangc_pat_new_string(strval, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_input_restore(input, ipos);
  return KLANGC_PARSE_NOPARSE;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_pat_print(klangc_output_t *output, int prec, klangc_pat_t *pat) {
  assert(output != NULL);
  assert(KLANGC_PREC_LOWEST <= prec);
  assert(prec <= KLANGC_PREC_HIGHEST);
  assert(pat != NULL);

  switch (pat->kp_type) {
  case KLANGC_PTYPE_REF:
    klangc_pat_ref_print(output, pat->kp_ref);
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
    return klangc_pat_ref_bind(env, pat->kp_ref, target);
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
    ret = func(pat->kp_ref, data);
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
