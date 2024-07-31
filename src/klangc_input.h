#ifndef __KLANGC_INPUT_H__
#define __KLANGC_INPUT_H__

#include "klangc.h"

#include "klangc_malloc.h"
#include "klangc_output.h"

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** 入力 */
typedef struct klangc_input {
  FILE *fp;
  const char *name;
  char *message;
  off_t off;
  int line;
  int col;
} klangc_input_t;

/** 入力バッファ */
typedef struct klangc_input_buf {
  off_t off;
  int line;
  int col;
} klangc_input_buf_t;

__attribute__((unused)) static void klangc_input_free(klangc_input_t *input) {
  klangc_free((void *)input->name);
  klangc_free(input);
}

__attribute__((unused)) static klangc_input_t *
klangc_input_new(FILE *fp, const char *name) {
  assert(fp != NULL);
  assert(name != NULL);
  klangc_input_t *input =
      (klangc_input_t *)klangc_malloc(sizeof(klangc_input_t));
  input->fp = fp;
  input->name = klangc_strdup(name);
  input->message = NULL;
  input->off = ftell(fp);
  input->line = 0;
  input->col = 0;
  return input;
}

__attribute__((unused)) static klangc_input_buf_t
klangc_input_save(klangc_input_t *input) {
  klangc_input_buf_t buf;
  buf.off = input->off;
  buf.line = input->line;
  buf.col = input->col;
  return buf;
}

__attribute__((unused)) static void
klangc_input_restore(klangc_input_t *input, klangc_input_buf_t buf) {
  if (input->off != buf.off) {
    fseek(input->fp, buf.off, SEEK_SET);
    input->off = buf.off;
  }
  input->line = buf.line;
  input->col = buf.col;
}

__attribute__((unused)) static int klangc_getc(klangc_input_t *input) {
  int c = fgetc(input->fp);
  if (c == EOF)
    return EOF;
  input->off++;
  if (c == '\n') {
    input->line++;
    input->col = 0;
  } else
    input->col++;
  return c;
}

__attribute__((unused)) static int
klangc_getc_skipspaces(klangc_input_t *input) {
  int in_comment = 0;
  while (1) {
    int c = klangc_getc(input);
    if (c == EOF)
      return EOF;
    if (in_comment) {
      if (c == '\n')
        in_comment = 0;
      continue;
    }
    if (isspace(c))
      continue;
    if (c == '#') {
      in_comment = 1;
      continue;
    }
    return c;
  }
}

__attribute__((unused, format(printf, 2, 3))) static void
klangc_message_add(klangc_input_t *input, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int newlen = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  int oldlen = 0;
  if (input->message != NULL)
    oldlen = strlen(input->message);
  input->message = (char *)klangc_realloc(input->message, newlen + oldlen + 1);
  va_start(ap, fmt);
  vsnprintf(input->message + oldlen, newlen + 1, fmt, ap);
  va_end(ap);
}

__attribute__((unused)) static void
klangc_message_reset(klangc_input_t *input) {
  klangc_free(input->message);
  input->message = NULL;
}

__attribute__((unused)) static void
klangc_message_add_buf(klangc_input_t *input, klangc_input_buf_t *ib) {
  if (ib != NULL)
    klangc_message_add(input, "%s(%d,%d): ", input->name, ib->line + 1,
                       ib->col + 1);
  else
    klangc_message_add(input, "%s(%d,%d): ", input->name, input->line + 1,
                       input->col + 1);
}

__attribute__((unused)) static void
klangc_message_print(klangc_input_t *input, klangc_output_t *output) {
  if (input->message != NULL)
    klangc_printf(output, "%s", input->message);
}
#endif // __KLANGC_INPUT_H__