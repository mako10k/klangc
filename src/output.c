#include "output.h"
#include "input.h"
#include "malloc.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// *******************************
// Output.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Output.
 */
struct klangc_output {
  /** File pointer */
  FILE *fp;
  /** File pointer cookie */
  FILE *fp_cookie;
  /** Indent */
  int indent;
  /** On head */
  int onhead;
};

// -------------------------------
// Constructors.
// -------------------------------
/** standard output */
klangc_output_t *kstdout = NULL;
/** standard error */
klangc_output_t *kstderr = NULL;

/** initializer */
__attribute__((constructor)) static void klangc_output_init() {
  kstdout = klangc_output_new(stdout);
  kstderr = klangc_output_new(stderr);
}

/** finalizer */
__attribute__((destructor)) static void klangc_output_fini() {
  klangc_output_free(kstdout);
  klangc_output_free(kstderr);
}

/**
 * output indented string.
 * @param cookie Cookie.
 * @param buf Buffer.
 * @param size Size.
 * @return Size.
 */
static ssize_t klangc_cookie_write(void *cookie, const char *buf, size_t size) {
  klangc_output_t *output = (klangc_output_t *)cookie;
  while (size > 0) {
    if (output->onhead)
      for (int i = 0; i < output->indent; i++)
        fputc(' ', output->fp);

    char *bufnl = memchr(buf, '\n', size);
    if (bufnl == NULL) {
      fwrite(buf, size, 1, output->fp);
      output->onhead = 0;
      break;
    }
    size_t len = bufnl - buf + 1;
    fwrite(buf, len, 1, output->fp);
    size -= len;
    buf += len;
    output->onhead = 1;
  }
  return size;
}

klangc_output_t *klangc_output_new(FILE *fp) {
  klangc_output_t *output =
      (klangc_output_t *)klangc_malloc(sizeof(klangc_output_t));
  static cookie_io_functions_t cookie_funcs = {
      .write = klangc_cookie_write,
  };
  output->fp = fp;
  output->fp_cookie = fopencookie(output, "w", cookie_funcs);
  output->indent = 0;
  return output;
}

// -------------------------------
// Destructors.
// -------------------------------
void klangc_output_free(klangc_output_t *output) {
  fclose(output->fp_cookie);
  klangc_free(output);
}

// -------------------------------
// Accessors.
// -------------------------------
/**
 * print formatted output.
 * @param output Output.
 * @param fmt Format.
 * @param ap Arguments.
 */
static int klangc_vfprintf(klangc_output_t *output, const char *fmt,
                           va_list ap) {
  return vfprintf(output->fp_cookie, fmt, ap);
}

/**
 * print formatted output with expected value.
 * @param output Output.
 * @param expect Expected value.
 * @param actual Actual char.
 * @param fmt Format.
 * @param ap Arguments.
 */
static int klangc_vfprintf_expects(klangc_output_t *output, const char *expect,
                                   int actual, const char *fmt, va_list ap) {
  int res = fprintf(output->fp_cookie, "expect %s", expect);
  if (res < 0)
    return res;
  if (actual == EOF)
    res = fprintf(output->fp_cookie, " but get EOF: ");
  else
    res = fprintf(output->fp_cookie, " but get '%c': ", actual);
  if (res < 0)
    return res;
  return klangc_vfprintf(output, fmt, ap);
}

int klangc_printf(klangc_output_t *output, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int res = klangc_vfprintf(output, fmt, args);
  va_end(args);
  return res;
}

int klangc_printf_expects(klangc_output_t *output, const char *expect,
                          int actual, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = klangc_vfprintf_expects(output, expect, actual, fmt, ap);
  va_end(ap);
  return res;
}

int klangc_printf_ipos(klangc_output_t *output, klangc_ipos_t ipos,
                       const char *fmt, ...) {
  int res = klangc_ipos_print(output, ipos);
  if (res < 0)
    return res;
  va_list ap;
  va_start(ap, fmt);
  res = klangc_vfprintf(output, fmt, ap);
  va_end(ap);
  return res;
}

int klangc_printf_ipos_expects(klangc_output_t *output, klangc_ipos_t ipos,
                               const char *expect, int actual, const char *fmt,
                               ...) {
  int res = klangc_ipos_print(output, ipos);
  if (res < 0)
    return res;
  va_list ap;
  va_start(ap, fmt);
  res = klangc_vfprintf_expects(output, expect, actual, fmt, ap);
  va_end(ap);
  return res;
}

void klangc_indent(klangc_output_t *output, int incr) {
  fflush(output->fp_cookie);
  output->indent += incr;
}
