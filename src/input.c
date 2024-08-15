#include "input.h"
#include "malloc.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>

// *******************************
// Input.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Input.
 */
struct klangc_input {
  /** Name */
  const char *kip_name;
  /** buffer */
  char *kip_buffer;
  /** buffer size */
  size_t kip_bufsize;
  /** Filesize */
  off_t kip_filesize;
  /** Offset */
  off_t kip_offset;
  /** Line */
  int kip_line;
  /** Column */
  int kip_col;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_input_t *klangc_input_new(FILE *fp, const char *name) {
  assert(fp != NULL);
  assert(name != NULL);
  klangc_input_t *input =
      (klangc_input_t *)klangc_malloc(sizeof(klangc_input_t));
  input->kip_name = klangc_strdup(name);
  input->kip_buffer = NULL;
  input->kip_bufsize = 0;
  input->kip_filesize = 0;
  input->kip_offset = 0;
  input->kip_line = 0;
  input->kip_col = 0;
  while (1) {
    while (input->kip_bufsize <= input->kip_filesize) {
      input->kip_bufsize += 4096;
      input->kip_buffer =
          (char *)realloc(input->kip_buffer, input->kip_bufsize);
    }
    size_t n = fread(input->kip_buffer + input->kip_filesize, 1,
                     input->kip_bufsize - input->kip_filesize, fp);
    if (n == 0)
      break;
    input->kip_filesize += n;
  }
  return input;
}

// -------------------------------
// Destructors.
// -------------------------------
void klangc_input_free(klangc_input_t *input) {
  free(input->kip_buffer);
  klangc_free((void *)input->kip_name);
  klangc_free(input);
}

// -------------------------------
// Accessors.
// -------------------------------
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
  input->kip_offset = ipos.kip_offset;
  input->kip_line = ipos.kip_line;
  input->kip_col = ipos.kip_col;
}

/**
 * Process to update internal state in input.
 * @param input Input.
 * @param c Character.
 * @return Character.
 */
static int klangc_procc(klangc_input_t *input, int c) {
  assert(0 <= c && c <= 255);
  if (c == '\n') {
    input->kip_line++;
    input->kip_col = 0;
  } else
    input->kip_col++;
  return c;
}

int klangc_getc(klangc_input_t *input) {
  assert(input != NULL);
  if (input->kip_offset >= input->kip_filesize)
    return EOF;
  return klangc_procc(input, input->kip_buffer[input->kip_offset++]);
}

int klangc_isspace(int c, int *in_comment) {
  if (c == EOF)
    return 0;
  if (*in_comment) {
    if (c == '\n' || c == '#')
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
  off_t offset = input->kip_offset;
  while (offset < input->kip_filesize &&
         klangc_isspace(input->kip_buffer[offset], &in_comment))
    klangc_procc(input, input->kip_buffer[offset++]);
  input->kip_offset = offset;
  return klangc_input_save(input);
}

// -------------------------------
// Printers.
// -------------------------------
int klangc_ipos_print(klangc_output_t *output, klangc_ipos_t ipos) {
  return klangc_printf(output,
                       "%s:%d:%d: ", klangc_input_get_name(ipos.kip_input),
                       ipos.kip_line + 1, ipos.kip_col + 1);
}
