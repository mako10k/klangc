#include "expr/closure.h"
#include "expr/closure/bare.h"
#include "input.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  klangc_closure_t *prelude = NULL;
  for (int i = 1; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      return EXIT_FAILURE;
    }
    klangc_input_t *input = klangc_input_new(fp, argv[i]);
    klangc_closure_t *closure;
    switch (klangc_expr_closure_bare_parse(input, prelude, &closure)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf(kstderr, "No definition found\n");
      return EXIT_FAILURE;
    case KLANGC_PARSE_ERROR:
      return EXIT_FAILURE;
    }
    klangc_expr_closure_bare_print(kstdout, closure);
    klangc_closure_bind(closure);
    klangc_closure_check_unbound(kstderr, closure);
  }
}