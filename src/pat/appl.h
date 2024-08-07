#pragma once

#include "../klangc.h"

klangc_pat_appl_t *klangc_pat_appl_new(klangc_pat_t *constr, klangc_pat_t *arg);

klangc_pat_t *klangc_pat_appl_get_constr(klangc_pat_appl_t *appl);

klangc_pat_t *klangc_pat_appl_get_arg(klangc_pat_appl_t *appl);
