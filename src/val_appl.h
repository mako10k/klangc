#pragma once

#include "klangc.h"
#include "types.h"

klangc_value_appl_t *klangc_value_appl_new(klangc_value_t *func);

void klangc_value_appl_add_arg(klangc_value_appl_t *appl, klangc_value_t *varg);

klangc_value_t *klangc_value_appl_get_func(klangc_value_appl_t *appl);

unsigned int klangc_value_appl_get_argc(klangc_value_appl_t *appl);

klangc_value_t *klangc_value_appl_get_arg(klangc_value_appl_t *appl, unsigned int i);
