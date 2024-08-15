#pragma once

#include "klangc.h"

klangc_value_data_t *klangc_value_data_new(void *data);

void *klangc_value_data_get_data(klangc_value_data_t *data_val);