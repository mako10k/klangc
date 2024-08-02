
#include "klangc_pattern.h"
#include "klangc_input.h"
#include "klangc_message.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include <assert.h>
#include <stdio.h>

struct klangc_pattern {
  klangc_pattern_type_t kp_type;
  union {
    klangc_pattern_symbol_t *kp_symbol;
    klangc_pattern_appl_t *kp_appl;
    klangc_pattern_as_t *kp_as;
    klangc_pattern_int_t *kp_int;
    klangc_pattern_string_t *kp_string;
  };
  klangc_ipos_t ipos;
};

struct klangc_pattern_symbol {
  char *kps_name;
};

struct klangc_pattern_appl {
  klangc_pattern_t *kpap_constr;
  klangc_pattern_t *kpap_arg;
};

struct klangc_pattern_as {
  klangc_pattern_symbol_t *kpas_var;
  klangc_pattern_t *kpas_pattern;
};

struct klangc_pattern_int {
  int kpi_value;
};

struct klangc_pattern_string {
  char *kps_value;
};

/**
 * Creates a new pattern object with the given symbol name.
 *
 * @param name The symbol name for the pattern.
 * @return A pointer to the newly created pattern object.
 */
klangc_pattern_t *klangc_pattern_new_symbol(const char *name,
                                            klangc_ipos_t ipos) {
  assert(name != NULL);

  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_SYMBOL;
  pattern->kp_symbol = klangc_malloc(sizeof(klangc_pattern_symbol_t));
  pattern->kp_symbol->kps_name = klangc_strdup(name);
  pattern->ipos = ipos;
  return pattern;
}

/**
 * Creates a new pattern by applying a constructor pattern to another pattern.
 *
 * @param constr The constructor pattern to apply.
 * @param arg    Additional arguments for the constructor pattern.
 * @return       A pointer to the newly created pattern.
 */
klangc_pattern_t *klangc_pattern_new_appl(klangc_pattern_t *constr,
                                          klangc_pattern_t *arg,
                                          klangc_ipos_t ipos) {
  assert(constr != NULL);
  assert(arg != NULL);

  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_APPL;
  pattern->kp_appl = klangc_malloc(sizeof(klangc_pattern_appl_t));
  pattern->kp_appl->kpap_constr = constr;
  pattern->kp_appl->kpap_arg = arg;
  pattern->ipos = ipos;
  return pattern;
}

/**
 * Creates a new klangc_pattern_t object with the specified symbol.
 *
 * @param var The symbol for the pattern.
 * @param pattern The pattern to apply the symbol to.
 * @return A pointer to the newly created klangc_pattern_t object.
 */
klangc_pattern_t *klangc_pattern_new_as(klangc_pattern_symbol_t *var,
                                        klangc_pattern_t *pattern,
                                        klangc_ipos_t ipos) {
  assert(var != NULL);
  assert(pattern != NULL);

  klangc_pattern_t *as = klangc_malloc(sizeof(klangc_pattern_t));
  as->kp_type = KLANGC_PTYPE_AS;
  as->kp_as = klangc_malloc(sizeof(klangc_pattern_as_t));
  as->kp_as->kpas_var = var;
  as->kp_as->kpas_pattern = pattern;
  as->ipos = ipos;
  return as;
}

/**
 * Creates a new klangc_pattern_t object with an integer value.
 *
 * @param value The integer value for the pattern.
 * @return A pointer to the newly created klangc_pattern_t object.
 */
klangc_pattern_t *klangc_pattern_new_int(int value, klangc_ipos_t ipos) {
  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_INT;
  pattern->kp_int = klangc_malloc(sizeof(klangc_pattern_int_t));
  pattern->kp_int->kpi_value = value;
  pattern->ipos = ipos;
  return pattern;
}

/**
 * Creates a new klangc_pattern_t object from a string value.
 *
 * @param value The string value to create the pattern from.
 * @return A pointer to the newly created klangc_pattern_t object.
 */
klangc_pattern_t *klangc_pattern_new_string(const char *value,
                                            klangc_ipos_t ipos) {
  assert(value != NULL);

  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_STRING;
  pattern->kp_string = klangc_malloc(sizeof(klangc_pattern_string_t));
  pattern->kp_string->kps_value = klangc_strdup(value);
  pattern->ipos = ipos;
  return pattern;
}

klangc_parse_result_t klangc_pattern_parse(klangc_input_t *input,
                                           klangc_pattern_t **ppattern);

int klangc_pattern_issymbol(klangc_pattern_t *pattern) {
  assert(pattern != NULL);

  return pattern->kp_type == KLANGC_PTYPE_SYMBOL;
}

klangc_pattern_symbol_t *klangc_pattern_get_symbol(klangc_pattern_t *pattern) {
  assert(pattern != NULL);

  assert(klangc_pattern_issymbol(pattern));
  return pattern->kp_symbol;
}

int klangc_pattern_isappl(klangc_pattern_t *pattern) {
  assert(pattern != NULL);

  return pattern->kp_type == KLANGC_PTYPE_APPL;
}

klangc_pattern_t *klangc_pattern_appl_get_constr(klangc_pattern_t *pattern) {
  assert(pattern != NULL);
  assert(klangc_pattern_isappl(pattern));

  return pattern->kp_appl->kpap_constr;
}

klangc_pattern_t *klangc_pattern_appl_get_arg(klangc_pattern_t *pattern) {
  assert(pattern != NULL);
  assert(klangc_pattern_isappl(pattern));

  return pattern->kp_appl->kpap_arg;
}

klangc_parse_result_t
klangc_pattern_parse_no_appl(klangc_input_t *input,
                             klangc_pattern_t **ppattern) {
  assert(input != NULL);
  assert(ppattern != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c == '(') {
    klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
    klangc_pattern_t *pat;
    switch (klangc_pattern_parse(input, &pat)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_print_ipos(kstderr, ipos_ss2);
      klangc_printf(kstderr, "expect <pattern>: ['(' ^<pattern> ')']\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss2 = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ')') {
      klangc_print_ipos(kstderr, ipos_ss2);
      klangc_printf(kstderr, "expect ')' but get '%c': ['(' <pattern> ^')']\n",
                    c);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *ppattern = pat;
    return KLANGC_PARSE_OK;
  }
  klangc_input_restore(input, ipos_ss);

  char *name;
  switch (klangc_symbol_parse(input, &name)) {
  case KLANGC_PARSE_OK: {
    klangc_pattern_t *symbol = klangc_pattern_new_symbol(name, ipos_ss);
    klangc_ipos_t ipos_symend = klangc_input_save(input);
    klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != '@') {
      klangc_input_restore(input, ipos_symend);
      *ppattern = symbol;
      return KLANGC_PARSE_OK;
    }
    klangc_pattern_t *pat;
    ipos_ss2 = klangc_skipspaces(input);
    switch (klangc_pattern_parse(input, &pat)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_print_ipos(kstderr, ipos_ss2);
      klangc_printf(kstderr, "expect <pattern>: [<symbol> '@' ^<pattern>]\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *ppattern = klangc_pattern_new_as(symbol->kp_symbol, pat, ipos_ss);
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
    *ppattern = klangc_pattern_new_int(intval, ipos_ss);
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
    *ppattern = klangc_pattern_new_string(strval, ipos_ss);
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

klangc_parse_result_t klangc_pattern_parse(klangc_input_t *input,
                                           klangc_pattern_t **ppattern) {
  assert(input != NULL);
  assert(ppattern != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_pattern_t *pat;
  switch (klangc_pattern_parse_no_appl(input, &pat)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }

  while (1) {
    klangc_pattern_t *arg;
    klangc_ipos_t ipos2 = klangc_input_save(input);
    klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
    (void)ipos_ss2;
    switch (klangc_pattern_parse_no_appl(input, &arg)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_input_restore(input, ipos2);
      *ppattern = pat;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    pat = klangc_pattern_new_appl(pat, arg, ipos_ss);
  }
}

int klangc_pattern_walkvars(klangc_def_t *def, klangc_def_ent_t *def_ent,
                            klangc_pattern_t *pat,
                            int (*bind_fn)(klangc_def_t *, const char *,
                                           klangc_def_ent_t *)) {
  assert(def != NULL);
  assert(def_ent != NULL);
  assert(pat != NULL);
  assert(bind_fn != NULL);

  int ret = 0;
  int cnt = 0;
  if (pat->kp_type == KLANGC_PTYPE_SYMBOL) {
    ret = bind_fn(def, pat->kp_symbol->kps_name, def_ent);
    if (ret < 0)
      return ret;
    cnt++;
  } else if (pat->kp_type == KLANGC_PTYPE_APPL) {
    ret = klangc_pattern_walkvars(def, def_ent, pat->kp_appl->kpap_constr,
                                  bind_fn);
    if (ret < 0)
      return ret;
    cnt += ret;
    ret =
        klangc_pattern_walkvars(def, def_ent, pat->kp_appl->kpap_arg, bind_fn);
    if (ret < 0)
      return ret;
    cnt += ret;
  } else if (pat->kp_type == KLANGC_PTYPE_AS) {
    ret = bind_fn(def, pat->kp_as->kpas_var->kps_name, def_ent);
    if (ret < 0)
      return ret;
    cnt++;
    ret = klangc_pattern_walkvars(def, def_ent, pat->kp_as->kpas_pattern,
                                  bind_fn);
    if (ret < 0)
      return ret;
    cnt += ret;
  }
  return cnt;
}

void klangc_pattern_print(klangc_output_t *output, int prec,
                          klangc_pattern_t *pattern) {
  assert(output != NULL);
  assert(KLANGC_PREC_LOWEST <= prec);
  assert(prec <= KLANGC_PREC_HIGHEST);
  assert(pattern != NULL);

  switch (pattern->kp_type) {
  case KLANGC_PTYPE_SYMBOL:
    klangc_printf(output, "%s", pattern->kp_symbol->kps_name);
    break;
  case KLANGC_PTYPE_APPL:
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, "(");
    klangc_pattern_print(output, KLANGC_PREC_APPL,
                         pattern->kp_appl->kpap_constr);
    klangc_printf(output, " ");
    klangc_pattern_print(output, KLANGC_PREC_APPL + 1,
                         pattern->kp_appl->kpap_arg);
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, ")");
    break;
  case KLANGC_PTYPE_AS:
    klangc_printf(output, "%s@", pattern->kp_as->kpas_var->kps_name);
    klangc_pattern_print(output, KLANGC_PREC_LOWEST,
                         pattern->kp_as->kpas_pattern);
    break;
  case KLANGC_PTYPE_INT:
    klangc_printf(output, "%d", pattern->kp_int->kpi_value);
    break;
  case KLANGC_PTYPE_STRING:
    klangc_printf(output, "\"%s\"", pattern->kp_string->kps_value);
    break;
  }
}