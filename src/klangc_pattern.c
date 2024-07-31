
#include "klangc_pattern.h"
#include "klangc_input.h"
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
klangc_pattern_t *klangc_pattern_new_symbol(const char *name) {
  assert(name != NULL);
  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_SYMBOL;
  pattern->kp_symbol = klangc_malloc(sizeof(klangc_pattern_symbol_t));
  pattern->kp_symbol->kps_name = klangc_strdup(name);
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
                                          klangc_pattern_t *arg) {
  assert(constr != NULL);
  assert(arg != NULL);
  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_APPL;
  pattern->kp_appl = klangc_malloc(sizeof(klangc_pattern_appl_t));
  pattern->kp_appl->kpap_constr = constr;
  pattern->kp_appl->kpap_arg = arg;
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
                                        klangc_pattern_t *pattern) {
  assert(var != NULL);
  assert(pattern != NULL);
  klangc_pattern_t *as = klangc_malloc(sizeof(klangc_pattern_t));
  as->kp_type = KLANGC_PTYPE_AS;
  as->kp_as = klangc_malloc(sizeof(klangc_pattern_as_t));
  as->kp_as->kpas_var = var;
  as->kp_as->kpas_pattern = pattern;
  return as;
}

/**
 * Creates a new klangc_pattern_t object with an integer value.
 *
 * @param value The integer value for the pattern.
 * @return A pointer to the newly created klangc_pattern_t object.
 */
klangc_pattern_t *klangc_pattern_new_int(int value) {
  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_INT;
  pattern->kp_int = klangc_malloc(sizeof(klangc_pattern_int_t));
  pattern->kp_int->kpi_value = value;
  return pattern;
}

/**
 * Creates a new klangc_pattern_t object from a string value.
 *
 * @param value The string value to create the pattern from.
 * @return A pointer to the newly created klangc_pattern_t object.
 */
klangc_pattern_t *klangc_pattern_new_string(const char *value) {
  assert(value != NULL);
  klangc_pattern_t *pattern = klangc_malloc(sizeof(klangc_pattern_t));
  pattern->kp_type = KLANGC_PTYPE_STRING;
  pattern->kp_string = klangc_malloc(sizeof(klangc_pattern_string_t));
  pattern->kp_string->kps_value = klangc_strdup(value);
  return pattern;
}

klangc_pattern_t *klangc_pattern_parse(klangc_input_t *input);

int klangc_pattern_issymbol(klangc_pattern_t *pattern) {
  assert(pattern != NULL);
  return pattern->kp_type == KLANGC_PTYPE_SYMBOL;
}

klangc_pattern_symbol_t *klangc_pattern_get_symbol(klangc_pattern_t *pattern) {
  assert(pattern != NULL);
  assert(klangc_pattern_issymbol(pattern));
  return pattern->kp_symbol;
}

klangc_pattern_t *klangc_pattern_parse_as(klangc_input_t *input,
                                          klangc_pattern_t *pattern) {
  assert(input != NULL);
  assert(pattern != NULL);
  assert(klangc_pattern_issymbol(pattern));
  int c;
  klangc_input_buf_t ib = klangc_input_save(input);
  c = klangc_getc_skipspaces(input);
  if (c == EOF)
    return NULL;
  if (c == '@') {
    klangc_pattern_t *pat = klangc_pattern_parse(input);
    if (pat == NULL) {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(input,
                         "expect <pattern>: [<symbol> '@' ^<pattern>]\n");
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }
    return klangc_pattern_new_as(klangc_pattern_get_symbol(pattern), pat);
  }
  klangc_input_restore(input, ib);
  return NULL;
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

klangc_pattern_t *klangc_pattern_parse_no_appl(klangc_input_t *input) {
  assert(input != NULL);
  int c;

  klangc_input_buf_t ib = klangc_input_save(input);
  c = klangc_getc_skipspaces(input);

  if (c == EOF) {
    klangc_input_restore(input, ib);
    return NULL;
  }

  if (c == '(') {
    klangc_pattern_t *ret = klangc_pattern_parse(input);
    if (ret == NULL) {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(input, "expect <pattern>: ['(' ^<pattern> ')']\n");
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }

    c = klangc_getc_skipspaces(input);
    if (c == ')')
      return ret;

    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(input, "expect ')' but get '%c': ['(' <pattern> ^')']\n",
                       c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_input_restore(input, ib);

  char *name = klangc_symbol_parse(input);
  if (name != NULL) {
    klangc_pattern_t *symbol = klangc_pattern_new_symbol(name);
    klangc_pattern_t *as = klangc_pattern_parse_as(input, symbol);
    if (as != NULL)
      return as;
    return symbol;
  }

  int intval;
  if (klangc_int_parse(input, &intval))
    return klangc_pattern_new_int(intval);

  char *strval = klangc_string_parse(input);
  if (strval != NULL)
    return klangc_pattern_new_string(strval);

  return NULL;
}

klangc_pattern_t *klangc_pattern_parse(klangc_input_t *input) {
  assert(input != NULL);
  klangc_pattern_t *ret;

  ret = klangc_pattern_parse_no_appl(input);
  if (ret == NULL) {
    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(input, "expect <pattern>: [^<pattern>]\n");
    return NULL;
  }

  while (1) {
    klangc_pattern_t *arg = klangc_pattern_parse_no_appl(input);
    if (arg == NULL)
      return ret;
    ret = klangc_pattern_new_appl(ret, arg);
  }
}

int klangc_pattern_walkvars(klangc_hash_t *defs, klangc_def_t *def,
                            klangc_pattern_t *pat,
                            int (*bind_fn)(klangc_hash_t *, const char *,
                                           klangc_def_t *)) {
  assert(defs != NULL);
  assert(def != NULL);
  assert(pat != NULL);
  assert(bind_fn != NULL);
  int ret = 0;
  int cnt = 0;
  if (pat->kp_type == KLANGC_PTYPE_SYMBOL) {
    ret = bind_fn(defs, pat->kp_symbol->kps_name, def);
    if (ret < 0)
      return ret;
    cnt++;
  } else if (pat->kp_type == KLANGC_PTYPE_APPL) {
    ret =
        klangc_pattern_walkvars(defs, def, pat->kp_appl->kpap_constr, bind_fn);
    if (ret < 0)
      return ret;
    cnt += ret;
    ret = klangc_pattern_walkvars(defs, def, pat->kp_appl->kpap_arg, bind_fn);
    if (ret < 0)
      return ret;
    cnt += ret;
  } else if (pat->kp_type == KLANGC_PTYPE_AS) {
    ret = bind_fn(defs, pat->kp_as->kpas_var->kps_name, def);
    if (ret < 0)
      return ret;
    cnt++;
    ret = klangc_pattern_walkvars(defs, def, pat->kp_as->kpas_pattern, bind_fn);
    if (ret < 0)
      return ret;
    cnt += ret;
  }
  return cnt;
}

void klangc_pattern_print(klangc_output_t *output, int prec,
                          klangc_pattern_t *pattern) {
  assert(output != NULL);
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
    klangc_pattern_print(output, KLANGC_PREC_LOWEST, pattern->kp_as->kpas_pattern);
    break;
  case KLANGC_PTYPE_INT:
    klangc_printf(output, "%d", pattern->kp_int->kpi_value);
    break;
  case KLANGC_PTYPE_STRING:
    klangc_printf(output, "\"%s\"", pattern->kp_string->kps_value);
    break;
  }
}