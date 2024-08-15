#include "val_data.h"
#include "malloc.h"

struct klangc_value_data {
  void *kvd_data;
};

klangc_value_data_t *klangc_value_data_new(void *data) {
  klangc_value_data_t *data_val = klangc_malloc(sizeof(klangc_value_data_t));
  data_val->kvd_data = data;
  return data_val;
}

void *klangc_value_data_get_data(klangc_value_data_t *data_val) {
  return data_val->kvd_data;
}