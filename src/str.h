#pragma once

#include "klangc.h"

// **********************
// ** String
// **********************
const klangc_str_t *klangc_str_new(const char *str, unsigned int len);

const char *klangc_str_get_str(const klangc_str_t *str);

unsigned int klangc_str_get_len(const klangc_str_t *str);

int klangc_str_cmp(const klangc_str_t *str1, const klangc_str_t *str2);

const char *klangc_str_get_cstr(const klangc_str_t *str);

klangc_parse_result_t klangc_str_parse(klangc_input_t *input,
                                       const klangc_str_t **pstrval);

const klangc_str_t* klangc_str_parse_str(const char *str, size_t slen);

void klangc_str_print(klangc_output_t *output, const klangc_str_t *str);