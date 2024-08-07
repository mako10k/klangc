#pragma once

#include "klangc.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct klangc_input klangc_input_t;

klangc_input_t *klangc_input_new(FILE *fp, const char *name);

const char *klangc_input_get_name(klangc_input_t *input);

klangc_ipos_t klangc_input_save(klangc_input_t *input);

void klangc_input_restore(klangc_input_t *input, klangc_ipos_t ipos);

int klangc_procc(klangc_input_t *input, int c);

int klangc_getc(klangc_input_t *input);

int klangc_isspace(int c, int *in_comment);

int klangc_getc_skipspaces(klangc_input_t *input);

klangc_ipos_t klangc_skipspaces(klangc_input_t *input);

void klangc_ipos_print(klangc_output_t *output, klangc_ipos_t ipos);