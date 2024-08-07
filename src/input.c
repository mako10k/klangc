#include "input.h"
#include "malloc.h"
#include "output.h"

struct klangc_input {
  FILE *kip_stream;
  const char *kip_name;
  off_t kip_offset;
  int kip_line;
  int kip_col;
};

klangc_input_t *klangc_input_new(FILE *fp, const char *name) {
  assert(fp != NULL);
  assert(name != NULL);
  klangc_input_t *input =
      (klangc_input_t *)klangc_malloc(sizeof(klangc_input_t));
  input->kip_stream = fp;
  input->kip_name = klangc_strdup(name);
  input->kip_offset = ftell(fp);
  input->kip_line = 0;
  input->kip_col = 0;
  return input;
}

const char *klangc_input_get_name(klangc_input_t *input) {
  return input->kip_name;
}

klangc_ipos_t klangc_input_save(klangc_input_t *input) {
  klangc_ipos_t ipos;
  ipos.kip_input = input;
  ipos.kip_offset = input->kip_offset;
  ipos.kip_line = input->kip_line;
  ipos.kip_col = input->kip_col;
  return ipos;
}

void klangc_input_restore(klangc_input_t *input, klangc_ipos_t ipos) {
  assert(input == ipos.kip_input);
  if (input->kip_offset != ipos.kip_offset) {
    fseek(input->kip_stream, ipos.kip_offset, SEEK_SET);
    input->kip_offset = ipos.kip_offset;
  }
  input->kip_line = ipos.kip_line;
  input->kip_col = ipos.kip_col;
}

int klangc_procc(klangc_input_t *input, int c) {
  if (c == EOF)
    return EOF;
  input->kip_offset++;
  if (c == '\n') {
    input->kip_line++;
    input->kip_col = 0;
  } else
    input->kip_col++;
  return c;
}

int klangc_getc(klangc_input_t *input) {
  return klangc_procc(input, fgetc(input->kip_stream));
}

int klangc_isspace(int c, int *in_comment) {
  if (c == EOF)
    return 0;
  if (*in_comment) {
    if (c == '\n')
      *in_comment = 0;
    return 1;
  }
  if (isspace(c))
    return 1;
  if (c == '#') {
    *in_comment = 1;
    return 1;
  }
  return 0;
}

int klangc_getc_skipspaces(klangc_input_t *input) {
  assert(input != NULL);
  int in_comment = 0;
  while (1) {
    int c = klangc_getc(input);
    if (!klangc_isspace(c, &in_comment))
      return c;
  }
}

klangc_ipos_t klangc_skipspaces(klangc_input_t *input) {
  int in_comment = 0;
  while (1) {
    int c = fgetc(input->kip_stream);
    if (!klangc_isspace(c, &in_comment)) {
      if (c != EOF)
        ungetc(c, input->kip_stream);
      return klangc_input_save(input);
    }
    klangc_procc(input, c);
  }
}

void klangc_ipos_print(klangc_output_t *output, klangc_ipos_t ipos) {
  klangc_printf(output, "%s(%d,%d): ", klangc_input_get_name(ipos.kip_input),
                ipos.kip_line + 1, ipos.kip_col + 1);
}
