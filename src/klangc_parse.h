#ifndef __KLANGC_PARSE_H__
#define __KLANGC_PARSE_H__

#include "klangc.h"

#include "klangc_input.h"

typedef enum klangc_parse_result {
  KLANGC_PARSE_OK,
  KLANGC_PARSE_NOPARSE,
  KLANGC_PARSE_ERROR,
} klangc_parse_result_t;

klangc_parse_result_t klangc_symbol_parse(klangc_input_t *input,
                                          char **psymbol);
klangc_parse_result_t klangc_int_parse(klangc_input_t *input, int *pintval);
klangc_parse_result_t klangc_string_parse(klangc_input_t *input,
                                          char **pstrval);

#endif // __KLANGC_PARSE_H__