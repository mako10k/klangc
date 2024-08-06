#include "klangc_bare_closure.h"
#include "klangc_expr.h"
#include "klangc_input.h"
#include <gc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  klangc_bare_closure_t *prelude = NULL;
  for (int i = 1; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      return EXIT_FAILURE;
    }
    klangc_input_t *input = klangc_input_new(fp, argv[i]);
    klangc_bare_closure_t *closure;
    switch (klangc_closure_parse(input, prelude, &closure)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf(kstderr, "No definition found\n");
      return EXIT_FAILURE;
    case KLANGC_PARSE_ERROR:
      return EXIT_FAILURE;
    }
    klangc_closure_print(kstdout, closure);
    klangc_closure_walk_bind(closure, klangc_expr_bind_for_walk, kstdout);
    klangc_closure_walk_bind(closure, klangc_expr_check_unbound_for_walk,
                             kstdout);
  }
}