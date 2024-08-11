#include "bind.h"
#include "expr_env.h"
#include "input.h"
#include "output.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
  klangc_expr_env_t *prelude = klangc_expr_env_new(NULL);
  for (int i = 1; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      return EXIT_FAILURE;
    }
    klangc_input_t *input = klangc_input_new(fp, argv[i]);
    klangc_bind_t *bind;
    switch (klangc_bind_parse(input, &bind)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf(kstderr, "No definition found\n");
      return EXIT_FAILURE;
    case KLANGC_PARSE_ERROR:
      return EXIT_FAILURE;
    }
    klangc_ipos_t ipos_ss = klangc_skipspaces(input);
    int c;
    if (klangc_expect(input, ';', &c) == 0) {
      klangc_printf_ipos_expects(kstderr, ipos_ss, "';'", c,
                                 "<program> ::= <bind> ^';';\n");
      return EXIT_FAILURE;
    }
    if (klangc_expect(input, EOF, &c) == 0) {
      klangc_printf_ipos_expects(kstderr, ipos_ss, "EOF", c,
                                 "<program> ::= <bind> ';'^;\n");
      return EXIT_FAILURE;
    }
    klangc_bind_print(kstdout, bind);
    klangc_printf(kstdout, ";\n");
    klangc_bind_bind(prelude, bind);
  }
}