#include "program.h"
#include "expr_closure.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "types.h"

struct klangc_program {
  klangc_expr_closure_t *kp_closure;
};

klangc_program_t *klangc_program_new(klangc_expr_closure_t *closure) {
  klangc_program_t *program = klangc_malloc(sizeof(klangc_program_t));
  program->kp_closure = closure;
  return program;
}

klangc_parse_result_t klangc_program_parse(klangc_input_t *input,
                                           klangc_program_t **pprogram) {
  klangc_expr_closure_t *closure;
  klangc_parse_result_t res =
      klangc_expr_closure_parse_nobrace(input, &closure);
  if (res != KLANGC_PARSE_OK)
    return res;
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  res = klangc_expect(input, EOF, &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_printf_ipos_expects(kstderr, ipos_ss, "EOF", c,
                               "<program> ::= <closure_bare> ';'^;\n");
    return KLANGC_PARSE_ERROR;
  }
  *pprogram = klangc_program_new(closure);
  return KLANGC_PARSE_OK;
}

void klangc_program_print(klangc_output_t *output, klangc_program_t *program) {
  klangc_expr_closure_print_nobrace(output, program->kp_closure);
}

klangc_bind_result_t klangc_program_bind(klangc_expr_env_t *env,
                                         klangc_program_t *program) {
  return klangc_expr_closure_bind(env, program->kp_closure);
}