#ifndef __KLANGC_PATTERN_H__
#define __KLANGC_PATTERN_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

typedef struct klangc_pattern_symbol klangc_pattern_symbol_t;
typedef struct klangc_pattern_appl klangc_pattern_appl_t;
typedef struct klangc_pattern_as klangc_pattern_as_t;
typedef struct klangc_pattern_int klangc_pattern_int_t;
typedef struct klangc_pattern_string klangc_pattern_string_t;

typedef enum {
  KLANGC_PTYPE_SYMBOL,
  KLANGC_PTYPE_APPL,
  KLANGC_PTYPE_AS,
  KLANGC_PTYPE_INT,
  KLANGC_PTYPE_STRING,
} klangc_pattern_type_t;

klangc_parse_result_t klangc_pattern_parse(klangc_input_t *input,
                                           klangc_pattern_t **ppattern);

void klangc_pattern_print(klangc_output_t *output, int prec,
                          klangc_pattern_t *pattern);

int klangc_pattern_walkvars(klangc_def_t *def, klangc_def_ent_t *def_ent,
                            klangc_pattern_t *pat,
                            int (*bind_fn)(klangc_def_t *, const char *,
                                           klangc_def_ent_t *));

#endif // __KLANGC_PATTERN_H__