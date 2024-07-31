#include "klangc_def.h"
#include "klangc_input.h"
#include <gc.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      return 1;
    }
    klangc_input_t *input = klangc_input_new(fp, argv[i]);
    klangc_hash_t *defs = klangc_def_parse(input);
    if (defs == NULL)
      return 1;
    klangc_def_print(kstdout, defs);
  }
}
