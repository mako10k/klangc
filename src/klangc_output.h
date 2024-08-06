#pragma once

#include "klangc.h"
#include "klangc_malloc.h"

klangc_output_t *klangc_output_new(FILE *fp);

void klangc_output_free(klangc_output_t *output);

__attribute__((unused, format(printf, 2, 3))) void
klangc_printf(klangc_output_t *output, const char *fmt, ...);

void klangc_indent(klangc_output_t *output, int incr);

extern klangc_output_t *kstdout;
extern klangc_output_t *kstderr;

enum {
  KLANGC_PREC_LOWEST,
  KLANGC_PREC_APPL,
  KLANGC_PREC_HIGHEST,
};
