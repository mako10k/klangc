#ifndef __KLANGC_PARSE_H__
#define __KLANGC_PARSE_H__

#include "klangc_input.h"

#define klangc_parse_paren(parser, input)                                      \
  ({                                                                           \
    klangc_input_t *_input = (input);                                          \
    klangc_input_buf_t ib = klangc_input_save(_input);                         \
    __auto_type _ret = NULL;                                                   \
    do {                                                                       \
      int c = klangc_getc_skipspaces(_input);                                  \
      if (c != '(')                                                            \
        break;                                                                 \
      __auto_type _ret2 = (parser)(_input);                                    \
      if (_ret2 == NULL)                                                       \
        break;                                                                 \
      c = klangc_getc_skipspaces(_input);                                      \
      if (c == EOF)                                                            \
        break;                                                                 \
      if (c != ')')                                                            \
        break;                                                                 \
      _ret = _ret2;                                                            \
    } while (0);                                                               \
    klangc_input_restore(_input, ib);                                          \
    _ret;                                                                      \
  })

char *klangc_symbol_parse(klangc_input_t *input);
int klangc_int_parse(klangc_input_t *input, int *intval);
char *klangc_string_parse(klangc_input_t *input);

#endif // __KLANGC_PARSE_H__