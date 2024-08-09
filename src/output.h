#pragma once

#include "klangc.h"
#include "malloc.h"

klangc_output_t *klangc_output_new(FILE *fp);

void klangc_output_free(klangc_output_t *output);

__attribute__((unused, format(printf, 2, 3))) void
klangc_printf(klangc_output_t *output, const char *fmt, ...);

__attribute__((unused, format(printf, 4, 5))) void
klangc_printf_expects(klangc_output_t *output, const char *expect, int actual,
                      const char *fmt, ...);

__attribute__((unused, format(printf, 3, 4))) void
klangc_printf_ipos(klangc_output_t *output, klangc_ipos_t ipos, const char *fmt,
                   ...);

__attribute__((unused, format(printf, 5, 6))) void
klangc_printf_ipos_expects(klangc_output_t *output, klangc_ipos_t ipos,
                           const char *expect, int actual, const char *fmt,
                           ...);

void klangc_indent(klangc_output_t *output, int incr);

extern klangc_output_t *kstdout;
extern klangc_output_t *kstderr;

enum {
  KLANGC_PREC_LOWEST,
  KLANGC_PREC_LAMBDA,
  KLANGC_PREC_APPL,
  KLANGC_PREC_HIGHEST,
};
