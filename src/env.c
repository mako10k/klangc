#include "env.h"
#include "hash.h"
#include <assert.h>

struct klangc_env {
  klangc_env_t *ke_parent;
  klangc_hash_t *ke_hash;
};

void klangc_env_put(klangc_env_t *env, const char *name, klangc_value_t *val) {
  assert(env != NULL);
  assert(name != NULL);
  assert(val != NULL);

  klangc_hash_put(env->ke_hash, name, val, NULL);
}