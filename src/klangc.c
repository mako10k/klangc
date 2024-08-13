#include "expr_closure.h"
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
    klangc_expr_closure_t *closure;
    switch (klangc_expr_closure_parse_nobrace(input, &closure)) {
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
    if (klangc_expect(input, EOF, &c) == 0) {
      klangc_printf_ipos_expects(kstderr, ipos_ss, "EOF", c,
                                 "<program> ::= <closure'> ';'^;\n");
      return EXIT_FAILURE;
    }
    klangc_expr_closure_print_nobrace(kstdout, closure);
    klangc_expr_closure_bind(prelude, closure);
  }
}