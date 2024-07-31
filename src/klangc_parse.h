#ifndef __KLANGC_PARSE_H__
#define __KLANGC_PARSE_H__

#include "klangc.h"

#include "klangc_input.h"

char *klangc_symbol_parse(klangc_input_t *input);
int klangc_int_parse(klangc_input_t *input, int *intval);
char *klangc_string_parse(klangc_input_t *input);

#endif // __KLANGC_PARSE_H__