#ifndef __KLANGC_MESSAGE_H__
#define __KLANGC_MESSAGE_H__

#include "klangc_input.h"
#include "klangc_malloc.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

__attribute__((unused)) static void klangc_print_ipos(klangc_output_t *output,
                                                      klangc_ipos_t ipos) {
  klangc_printf(output, "%s(%d,%d): ", ipos.input->name, ipos.line + 1,
                ipos.col + 1);
}

#endif // __KLANGC_MESSAGE_H__