#include "expr_env.h"
#include "hash.h"
#include "malloc.h"
#include "symbol.h"

struct klangc_expr_env {
  klangc_expr_env_t *kee_upper;
  klangc_hash_t *kee_entries;
};

klangc_expr_env_t *klangc_expr_env_new(klangc_expr_env_t *upper) {
  klangc_expr_env_t *env = klangc_malloc(sizeof(klangc_expr_env_t));
  env->kee_upper = upper;
  env->kee_entries = klangc_hash_new(16);
  return env;
}

klangc_expr_env_t *klangc_expr_env_get_upper(klangc_expr_env_t *env) {
  return env->kee_upper;
}

klangc_expr_ref_target_t *
klangc_expr_env_get_entry(klangc_expr_env_t *env, const klangc_symbol_t *sym) {
  klangc_expr_ref_target_t *target;
  for (klangc_expr_env_t *e = env; e != NULL; e = e->kee_upper) {
    if (klangc_hash_get(e->kee_entries, klangc_symbol_get_name(sym),
                        (void **)&target))
      return target;
  }
  return NULL;
}

int klangc_expr_env_put_entry(klangc_expr_env_t *env,
                              const klangc_symbol_t *sym,
                              klangc_expr_ref_target_t *target) {
  const klangc_str_t *name = klangc_symbol_get_name(sym);
  return klangc_hash_put(env->kee_entries, name, target, NULL);
}