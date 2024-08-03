#ifndef __KLANGC_BIND_H__
#define __KLANGC_BIND_H__

#include "klangc_input.h"
#include "klangc_types.h"

klangc_bind_t *klangc_bind_new(klangc_pattern_t *pat, klangc_expr_t *expr,
                               klangc_ipos_t ipos);

klangc_pattern_t *klangc_bind_get_pat(klangc_bind_t *bind);
klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind);

#endif