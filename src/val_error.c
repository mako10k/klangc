#include "val_error.h"
#include "malloc.h"

struct klangc_value_error {
  const char *message;
};

klangc_value_error_t *klangc_value_error_new(const char *message) {
  klangc_value_error_t *error = klangc_malloc(sizeof(klangc_value_error_t));
  error->message = message;
  return error;
}