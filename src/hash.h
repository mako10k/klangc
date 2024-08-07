#pragma once

#include "klangc.h"

typedef struct klangc_hash_entry klangc_hash_entry_t;

klangc_hash_t *klangc_hash_new(int capacity);

int klangc_hash_get(klangc_hash_t *hash, const char *key, void **value);

int klangc_hash_put(klangc_hash_t *hash, const char *key, void *value,
                    void **old_value);

int klangc_hash_remove(klangc_hash_t *hash, const char *key, void **value);

void klangc_hash_foreach(klangc_hash_t *hash,
                         void (*callback)(const char *, void *, void *),
                         void *data);