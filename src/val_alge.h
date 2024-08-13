#pragma once

#include "klangc.h"

klangc_symbol_t *klangc_value_alge_get_constr(klangc_value_alge_t *valge);

int klangc_value_alge_get_argc(klangc_value_alge_t *valge);

klangc_value_t *klangc_value_alge_get_arg(klangc_value_alge_t *valge, int index);