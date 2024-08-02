#ifndef __KLANGC_DICT_H__
#define __KLANGC_DICT_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

klangc_parse_result_t klangc_dict_parse(klangc_input_t *input,
                                        klangc_def_t *enclosed_by,
                                        klangc_dict_t **pdict);

void klangc_dict_print(klangc_output_t *output, klangc_dict_t *dict);

klangc_def_t *klangc_dict_get_def(klangc_dict_t *dict);

#endif // __KLANGC_DICT_H__