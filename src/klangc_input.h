#ifndef __KLANGC_INPUT_H__
#define __KLANGC_INPUT_H__

#include "klangc_malloc.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>

/** 入力 */
typedef struct klangc_input {
  FILE *fp;
  const char *name;
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
    if (isspace(c))
      continue;
    if (in_comment) {
      if (c == '\n')
        in_comment = 0;
      continue;
    } else if (c == '#') {
      in_comment = 1;
      continue;
    }
    return c;
  }
}

#endif // __KLANGC_INPUT_H__