#include "klangc_output.h"

klangc_output_t *kstdout = NULL;
klangc_output_t *kstderr = NULL;

__attribute__((constructor)) static void klangc_output_init() {
  kstdout = klangc_output_new(stdout);
  kstderr = klangc_output_new(stderr);
}

__attribute__((destructor)) static void klangc_output_fini() {
  klangc_output_free(kstdout);
  klangc_output_free(kstderr);
}