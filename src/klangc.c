#include "expr_env.h"
#include "input.h"
#include "output.h"
#include "program.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
  klangc_expr_env_t *prelude = klangc_expr_env_new(NULL);
  for (int i = 1; i < argc; i++) {
    FILE *fp = strcmp("-", argv[i]) == 0 ? stdin : fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      return EXIT_FAILURE;
    }
    klangc_input_t *input = klangc_input_new(fp, argv[i]);
    fclose(fp);
    klangc_program_t *program;
    klangc_ipos_t ipos = klangc_input_save(input);
    klangc_ipos_t ipos_ss = klangc_skipspaces(input);
    klangc_parse_result_t res = klangc_program_parse(input, &program);
    switch (res) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf_ipos_expects(kstderr, ipos_ss, "<program>",
                                 klangc_getc(input), "^<program>;\n");
      return EXIT_FAILURE;
    case KLANGC_PARSE_ERROR:
      return EXIT_FAILURE;
    }
    klangc_program_print(kstdout, program);
    klangc_program_bind(prelude, program);
    klangc_input_free(input);
  }
}