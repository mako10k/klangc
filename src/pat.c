
#include "pat.h"
#include "input.h"
#include "output.h"
#include "parse.h"
#include "pat/appl.h"
#include "pat/as.h"
#include "pat/ref.h"
#include "symbol.h"
#include <assert.h>
#include <stdio.h>

struct klangc_pat {
  klangc_pat_type_t kp_type;
  union {
    klangc_symbol_t *kp_symbol;
    klangc_pat_ref_t *kp_ref;
    klangc_pat_appl_t *kp_appl;
    klangc_pat_as_t *kp_as;
    klangc_pat_int_t *kp_int;
    klangc_pat_string_t *kp_string;
  };
  klangc_ipos_t ipos;
};

struct klangc_pat_int {
  int kpi_value;
};

struct klangc_pat_string {
  char *kps_value;
};

static klangc_pat_int_t *klangc_pat_int_new(int value) {
  klangc_pat_int_t *integer = klangc_malloc(sizeof(klangc_pat_int_t));
  integer->kpi_value = value;
  return integer;
}

static klangc_pat_string_t *klangc_pat_string_new(const char *value) {
  assert(value != NULL);

  klangc_pat_string_t *string = klangc_malloc(sizeof(klangc_pat_string_t));
  string->kps_value = klangc_strdup(value);
  return string;
}

/**
 * Creates a new pattern object with the given symbol name.
 *
 * @param name The symbol name for the pattern.
 * @return A pointer to the newly created pattern object.
 */
klangc_pat_t *klangc_pat_new_symbol(klangc_symbol_t *symbol,
                                    klangc_ipos_t ipos) {
  assert(symbol != NULL);

  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_SYMBOL;
  pat->kp_symbol = symbol;
  pat->ipos = ipos;
  return pat;
}

klangc_pat_t *klangc_pat_new_ref(klangc_pat_ref_t *ref, klangc_ipos_t ipos) {
  assert(ref != NULL);

  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_REF;
  pat->kp_ref = ref;
  pat->ipos = ipos;
  return pat;
}

/**
 * Creates a new pattern by applying a constructor pattern to another pattern.
 *
 * @param constr The constructor pattern to apply.
 * @param arg    Additional arguments for the constructor pattern.
 * @return       A pointer to the newly created pattern.
 */
klangc_pat_t *klangc_pat_new_appl(klangc_pat_t *constr, klangc_pat_t *arg,
                                  klangc_ipos_t ipos) {
  assert(constr != NULL);
  assert(arg != NULL);

  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_APPL;
  pat->kp_appl = klangc_pat_appl_new(constr, arg);
  pat->ipos = ipos;
  return pat;
}

/**
 * Creates a new klangc_pat_t object with the specified symbol.
 *
 * @param var The symbol for the pattern.
 * @param pat The pattern to apply the symbol to.
 * @return A pointer to the newly created klangc_pattern_t object.
 */
klangc_pat_t *klangc_pat_new_as(klangc_pat_ref_t *var, klangc_pat_t *pat,
                                klangc_ipos_t ipos) {
  assert(var != NULL);
  assert(pat != NULL);

  klangc_pat_t *as = klangc_malloc(sizeof(klangc_pat_t));
  as->kp_type = KLANGC_PTYPE_AS;
  as->kp_as = klangc_pat_as_new(var, pat);
  as->ipos = ipos;
  return as;
}

/**
 * Creates a new klangc_pat_t object with an integer value.
 *
 * @param value The integer value for the pattern
 * @return A pointer to the newly created klangc_pat_t object.
 */
klangc_pat_t *klangc_pat_new_int(int value, klangc_ipos_t ipos) {
  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_INT;
  pat->kp_int = klangc_pat_int_new(value);
  pat->ipos = ipos;
  return pat;
}

/**
 * Creates a new klangc_pat_t object from a string value.
 *
 * @param value The string value to create the pattern from.
 * @return A pointer to the newly created klangc_pat_t object.
 */
klangc_pat_t *klangc_pat_new_string(const char *value, klangc_ipos_t ipos) {
  assert(value != NULL);

  klangc_pat_t *pat = klangc_malloc(sizeof(klangc_pat_t));
  pat->kp_type = KLANGC_PTYPE_STRING;
  pat->kp_string = klangc_pat_string_new(value);
  pat->ipos = ipos;
  return pat;
}

klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_t **ppat);

int klangc_pat_issymbol(klangc_pat_t *pat) {
  assert(pat != NULL);
  return pat->kp_type == KLANGC_PTYPE_SYMBOL;
}

klangc_symbol_t *klangc_pat_get_symbol(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(klangc_pat_issymbol(pat));
  return pat->kp_symbol;
}

int klangc_pat_isref(klangc_pat_t *pat) {
  assert(pat != NULL);
  return pat->kp_type == KLANGC_PTYPE_REF;
}

klangc_ref_t *klangc_pat_get_ref(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(klangc_pat_isref(pat));
  return klangc_pat_ref_get_ref(pat->kp_ref);
}

klangc_pat_appl_t *klangc_pat_get_appl(klangc_pat_t *pat) {
  assert(pat != NULL);
  assert(pat->kp_type == KLANGC_PTYPE_APPL);
  return pat->kp_appl;
}

klangc_parse_result_t klangc_pat_parse_no_appl(klangc_input_t *input,
                                               klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c == '(') {
    klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
    klangc_pat_t *pat;
    switch (klangc_pat_parse(input, &pat)) {
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

  klangc_symbol_t *sym;
  switch (klangc_symbol_parse(input, &sym)) {
  case KLANGC_PARSE_OK:
    *ppat = klangc_pat_new_symbol(sym, ipos_ss);
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
    switch (klangc_pat_parse(input, &pat)) {
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

  char *strval;
  switch (klangc_string_parse(input, &strval)) {
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

klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_t **ppat) {
  assert(input != NULL);
  assert(ppat != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_pat_t *pat;
  switch (klangc_pat_parse_no_appl(input, &pat)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }

  while (1) {
    klangc_pat_t *arg;
    klangc_ipos_t ipos2 = klangc_input_save(input);
    switch (klangc_pat_parse_no_appl(input, &arg)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_input_restore(input, ipos2);
      *ppat = pat;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    pat = klangc_pat_new_appl(pat, arg, ipos_ss);
  }
}

int klangc_pat_foreach_ref(klangc_pat_t *pat,
                           klangc_pat_foreach_ref_func_t bind_fn, void *data) {
  assert(pat != NULL);
  assert(bind_fn != NULL);

  int ret = 0;
  int cnt = 0;
  if (pat->kp_type == KLANGC_PTYPE_REF) {
    ret = bind_fn(pat->kp_ref, data);
    if (ret < 0)
      return ret;
    cnt++;
  } else if (pat->kp_type == KLANGC_PTYPE_APPL) {
    klangc_pat_t *constr = klangc_pat_appl_get_constr(pat->kp_appl);
    ret = klangc_pat_foreach_ref(constr, bind_fn, data);
    if (ret < 0)
      return ret;
    cnt += ret;
    klangc_pat_t *arg = klangc_pat_appl_get_arg(pat->kp_appl);
    ret = klangc_pat_foreach_ref(arg, bind_fn, data);
    if (ret < 0)
      return ret;
    cnt += ret;
  } else if (pat->kp_type == KLANGC_PTYPE_AS) {
    ret = bind_fn(klangc_pat_as_get_ref(pat->kp_as), data);
    if (ret < 0)
      return ret;
    cnt++;
    ret = klangc_pat_foreach_ref(klangc_pat_as_get_pat(pat->kp_as), bind_fn,
                                 data);
    if (ret < 0)
      return ret;
    cnt += ret;
  }
  return cnt;
}

void klangc_pat_print(klangc_output_t *output, int prec, klangc_pat_t *pat) {
  assert(output != NULL);
  assert(KLANGC_PREC_LOWEST <= prec);
  assert(prec <= KLANGC_PREC_HIGHEST);
  assert(pat != NULL);

  switch (pat->kp_type) {
  case KLANGC_PTYPE_SYMBOL:
    klangc_symbol_print(output, pat->kp_symbol);
    break;
  case KLANGC_PTYPE_REF:
    klangc_pat_ref_print(output, pat->kp_ref);
    break;
  case KLANGC_PTYPE_APPL:
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, "(");
    klangc_pat_print(output, KLANGC_PREC_APPL,
                     klangc_pat_appl_get_constr(pat->kp_appl));
    klangc_printf(output, " ");
    klangc_pat_print(output, KLANGC_PREC_APPL + 1,
                     klangc_pat_appl_get_arg(pat->kp_appl));
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, ")");
    break;
  case KLANGC_PTYPE_AS:
    klangc_printf(output, "%s@",
                  klangc_pat_ref_get_name(klangc_pat_as_get_ref(pat->kp_as)));
    klangc_pat_print(output, KLANGC_PREC_LOWEST,
                     klangc_pat_as_get_pat(pat->kp_as));
    break;
  case KLANGC_PTYPE_INT:
    klangc_printf(output, "%d", pat->kp_int->kpi_value);
    break;
  case KLANGC_PTYPE_STRING:
    klangc_printf(output, "\"%s\"", pat->kp_string->kps_value);
    break;
  }
}

klangc_pat_type_t klangc_pat_get_type(klangc_pat_t *pat) {
  assert(pat != NULL);
  return pat->kp_type;
}