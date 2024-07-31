#ifndef __KLANGC_HASH_H__
#define __KLANGC_HASH_H__

#include "klangc.h"

#include "klangc_types.h"

typedef struct klangc_hash_entry klangc_hash_entry_t;

klangc_hash_t *klangc_hash_new(int capacity);
int klangc_hash_get(klangc_hash_t *hash, const char *key, void **value);
int klangc_hash_put(klangc_hash_t *hash, const char *key, void *value,
                    void **old_value);
int klangc_hash_remove(klangc_hash_t *hash, const char *key, void **value);
void klangc_hash_walk(klangc_hash_t *hash,
                      void (*callback)(const char *, void *, void *),
                      void *data);

#endif // __KLANGC_HASH_H__