#include "klangc_output.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct klangc_output {
  FILE *fp;
  FILE *fp_cookie;
  int indent;
  int onhead;
};

klangc_output_t *kstdout = NULL;
klangc_output_t *kstderr = NULL;

__attribute__((constructor)) static void klangc_output_init() {
  kstdout = klangc_output_new(stdout);
  kstderr = klangc_output_new(stderr);
}

__attribute__((destructor)) static void klangc_output_fini() {
  klangc_output_free(kstdout);
  klangc_output_free(kstderr);
}

static ssize_t klangc_cookie_write(void *cookie, const char *buf, size_t size) {
  klangc_output_t *output = (klangc_output_t *)cookie;
  while (size > 0) {
    if (output->onhead)
      for (int i = 0; i < output->indent; i++)
        fputc(' ', output->fp);

    char *bufnl = (char *)memchr(buf, '\n', size);
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

void klangc_output_free(klangc_output_t *output) {
  fclose(output->fp_cookie);
  klangc_free(output);
}

void klangc_printf(klangc_output_t *output, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(output->fp_cookie, fmt, args);
  va_end(args);
}

void klangc_indent(klangc_output_t *output, int incr) {
  fflush(output->fp_cookie);
  output->indent += incr;
}
