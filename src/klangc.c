#include "klangc_def.h"
#include "klangc_expr.h"
#include "klangc_input.h"
#include <gc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  klangc_def_t *prelude = NULL;
  for (int i = 1; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      return EXIT_FAILURE;
    }
    klangc_input_t *input = klangc_input_new(fp, argv[i]);
    klangc_def_t *def;
    switch (klangc_def_parse(input, prelude, &def)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_printf(kstderr, "No definition found\n");
      return EXIT_FAILURE;
    case KLANGC_PARSE_ERROR:
      return EXIT_FAILURE;
    }
    klangc_def_print(kstdout, def);
    klangc_def_walk(def, klangc_expr_check_unbound, kstdout);
  }
}