#ifndef __KLANGC_PATTERN_H__
#define __KLANGC_PATTERN_H__

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

#include "klangc.h"
#include "klangc_input.h"

#include <stdio.h>

klangc_pattern_t *klangc_pattern_parse(klangc_input_t *input);
void klangc_pattern_print(FILE *fp, klangc_pattern_t *pattern);

int klangc_pattern_walkvars(klangc_hash_t *defs, klangc_def_t *def,
                            klangc_pattern_t *pat,
                            int (*bind_fn)(klangc_hash_t *, const char *,
                                           klangc_def_t *));

#endif // __KLANGC_PATTERN_H__