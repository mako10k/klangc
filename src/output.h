#pragma once

#include "klangc.h"

// *******************************
// Output.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new output.
 * @param fp File pointer.
 * @return New output.
 */
klangc_output_t *klangc_output_new(FILE *fp);

// -------------------------------
// Destructors.
// -------------------------------
/**
 * Free an output.
 * @param output Output.
 */
void klangc_output_free(klangc_output_t *output);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * print formatted output.
 * @param output Output.
 * @param fmt Format.
 * @param ... Arguments.
 */
int klangc_printf(klangc_output_t *output, const char *fmt, ...)
    __attribute__((unused, format(printf, 2, 3)));

/**
 * print formatted output with expected value.
 * @param output Output.
 * @param expect Expected value.
 * @param actual Actual char.
 * @param fmt Format.
 * @param ... Arguments.
 */
int klangc_printf_expects(klangc_output_t *output, const char *expect,
                          int actual, const char *fmt, ...)
    __attribute__((unused, format(printf, 4, 5)));

/**
 * print formatted output with position.
 * @param output Output.
 * @param ipos Position.
 * @param fmt Format.
 * @param ... Arguments.
 */
int klangc_printf_ipos(klangc_output_t *output, klangc_ipos_t ipos,
                       const char *fmt, ...)
    __attribute__((unused, format(printf, 3, 4)));

/**
 * print formatted output with position and expected value.
 * @param output Output.
 * @param ipos Position.
 * @param expect Expected value.
 * @param actual Actual char.
 * @param fmt Format.
 * @param ... Arguments.
 */
int klangc_printf_ipos_expects(klangc_output_t *output, klangc_ipos_t ipos,
                               const char *expect, int actual, const char *fmt,
                               ...)
    __attribute__((unused, format(printf, 5, 6)));

/**
 * increment the indentation.
 * @param output Output.
 * @param incr Increment.
 */
void klangc_indent(klangc_output_t *output, int incr);

/** standard output */
extern klangc_output_t *kstdout;
/** standard error */
extern klangc_output_t *kstderr;

enum {
  KLANGC_PREC_LOWEST,
  KLANGC_PREC_LAMBDAS,
  KLANGC_PREC_LAMBDA,
  KLANGC_PREC_CONS,
  KLANGC_PREC_APPL,
  KLANGC_PREC_HIGHEST,
};
