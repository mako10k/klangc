#include "inst.h"
#include "malloc.h"
#include "str.h"
#include "symbol.h"
#include "tuple.h"
#include "types.h"
#include "val.h"
#include "val_alge.h"
#include "val_data.h"
#include <assert.h>
#include <string.h>

typedef klangc_value_t *(*klangc_exec_func_t)(unsigned int argc,
                                              klangc_value_t **args);

struct klangc_inst_ent {
  klangc_inst_t *kient_inst;
  klangc_value_t *kient_val;
  klangc_exec_func_t kient_exec_func;
  unsigned int kient_argc;
  klangc_value_t **kient_args;
  klangc_inst_ent_t *kient_next;
};

struct klangc_inst {
  klangc_inst_ent_t *kinst_ents;
  klangc_inst_ent_t **kinst_pent_last;
};

typedef struct klangc_inst_func_spec {
  const char *name;
  const klangc_str_t *name_str;
  klangc_exec_func_t exec_func;
  unsigned int min_argc;
  unsigned int max_argc;
} klangc_inst_func_spec_t;

/*
 * instructions
 * () = ~inst println ~str
 * ~str = ~inst readln
 */

static klangc_value_t *klangc_inst_fopen(unsigned int argc,
                                         klangc_value_t **args);
static klangc_value_t *klangc_inst_fclose(unsigned int argc,
                                          klangc_value_t **args);
static klangc_value_t *klangc_inst_fread(unsigned int argc,
                                         klangc_value_t **args);
static klangc_value_t *klangc_inst_fwrite(unsigned int argc,
                                          klangc_value_t **args);

static klangc_inst_func_spec_t klangc_inst_func_specs[] = {
    {"fopen", NULL, klangc_inst_fopen, 2, 2},
    {"fclose", NULL, klangc_inst_fclose, 1, 1},
    {"fread", NULL, klangc_inst_fread, 2, 2},
    {"fwrite", NULL, klangc_inst_fwrite, 2, 2},
    {NULL, NULL, NULL, 0, 0}};

/**
 * evaluate instruction until the specified end of the instruction list
 * @param inst instruction
 * @param ent_end end of the instruction list
 * @return 0 if successful, -1 if failed
 */
static int klangc_inst_eval_ent(klangc_inst_t *inst,
                                klangc_inst_ent_t *ent_end) {
  assert(inst != NULL);
  assert(inst->kinst_ents != NULL);
  assert(inst->kinst_pent_last != NULL);
  assert(*inst->kinst_pent_last == NULL);
  assert(ent_end != NULL);
  assert(ent_end->kient_inst == inst);
  assert(ent_end->kient_val == NULL);
  assert(ent_end->kient_exec_func != NULL);
  klangc_inst_ent_t *ent = inst->kinst_ents;
  while (ent != NULL) {
    assert(ent->kient_inst == inst);
    assert(ent->kient_val == NULL);
    assert(ent->kient_exec_func != NULL);
    ent->kient_val = ent->kient_exec_func(ent->kient_argc, ent->kient_args);
    assert(ent->kient_val != NULL);
    ent->kient_inst = NULL;
    inst->kinst_ents = ent->kient_next;
    if (inst->kinst_ents == NULL)
      inst->kinst_pent_last = &inst->kinst_ents;
    if (ent == ent_end)
      return 0;
    ent = ent->kient_next;
  }
  return -1;
}

static klangc_value_t *klangc_inst_fopen(unsigned int argc,
                                         klangc_value_t **args) {
  assert(argc == 2);
  assert(args != NULL);
  assert(args[0] != NULL);
  assert(args[1] != NULL);
  klangc_value_t *name = args[0];
  klangc_value_t *mode = args[1];
  klangc_value_eval(name);
  if (klangc_value_get_type(name) != KLANGC_VTYPE_STRING)
    return klangc_value_new_error("fopen: name is not a string");
  klangc_value_eval(mode);
  if (klangc_value_get_type(mode) != KLANGC_VTYPE_STRING)
    return klangc_value_new_error("fopen: mode is not a string");

  const char *name_str = klangc_str_get_cstr(klangc_value_get_string(name));
  const char *flags_str = klangc_str_get_cstr(klangc_value_get_string(mode));
  FILE *fp = fopen(name_str, flags_str);
  if (fp == NULL)
    return klangc_value_new_error("fopen: failed to open file");
  klangc_value_data_t *vdata_fp = klangc_value_data_new(fp);
  return klangc_value_new_data(vdata_fp);
}

static klangc_value_t *klangc_inst_fclose(unsigned int argc,
                                          klangc_value_t **args) {
  assert(argc == 2);
  assert(args != NULL);
  assert(args[0] != NULL);
  klangc_value_t *val_fp = args[0];
  klangc_value_eval(val_fp);
  if (klangc_value_get_type(val_fp) != KLANGC_VTYPE_DATA)
    return klangc_value_new_error("fcopse: fp is not a data");
  klangc_value_data_t *vdata_fp = klangc_value_get_data(val_fp);
  FILE *fp = klangc_value_data_get_data(vdata_fp);
  fclose(fp);
  if (fp == NULL)
    return klangc_value_new_error("fclose: failed to close file");
  return klangc_value_unit();
}

static klangc_value_t *klangc_inst_fread(unsigned int argc,
                                         klangc_value_t **args) {
  assert(argc == 2);
  assert(args != NULL);
  assert(args[0] != NULL);
  assert(args[1] != NULL);
  klangc_value_t *val_fp = args[0];
  klangc_value_t *val_size = args[1];
  klangc_value_eval(val_fp);
  if (klangc_value_get_type(val_fp) != KLANGC_VTYPE_DATA)
    return klangc_value_new_error("fread: fp is not a data");
  klangc_value_data_t *vdata_fp = klangc_value_get_data(val_fp);
  FILE *fp = klangc_value_data_get_data(vdata_fp);
  klangc_value_eval(val_size);
  if (klangc_value_get_type(val_size) != KLANGC_VTYPE_INT)
    return klangc_value_new_error("fread: size is not an int");
  int size = klangc_value_get_int(val_size);
  char *buf = klangc_malloc(size);
  size_t n = fread(buf, 1, size, fp);
  if (n != size)
    return klangc_value_new_error("fread: failed to read file");
  return klangc_value_new_str(klangc_str_new(buf, n));
}

static klangc_value_t *klangc_inst_fwrite(unsigned int argc,
                                          klangc_value_t **args) {
  assert(argc == 2);
  assert(args != NULL);
  assert(args[0] != NULL);
  assert(args[1] != NULL);
  klangc_value_t *val_fp = args[0];
  klangc_value_t *val_str = args[1];
  klangc_value_eval(val_fp);
  if (klangc_value_get_type(val_fp) != KLANGC_VTYPE_DATA)
    return klangc_value_new_error("fwrite: fp is not a data");
  klangc_value_data_t *vdata_fp = klangc_value_get_data(val_fp);
  FILE *fp = klangc_value_data_get_data(vdata_fp);
  klangc_value_eval(val_str);
  if (klangc_value_get_type(val_str) != KLANGC_VTYPE_STRING)
    return klangc_value_new_error("fwrite: str is not a string");
  const char *str = klangc_str_get_cstr(klangc_value_get_string(val_str));
  size_t n = strlen(str);
  size_t m = fwrite(str, 1, n, fp);
  if (m != n)
    return klangc_value_new_error("fwrite: failed to write file");
  return klangc_value_unit();
}

static klangc_value_ioref_t *klangc_inst_ent_add(klangc_inst_t *inst,
                                                 klangc_exec_func_t exec_func,
                                                 unsigned int argc,
                                                 klangc_value_t **args) {
  assert(inst != NULL);
  assert(inst->kinst_pent_last != NULL);
  assert(exec_func != NULL);
  klangc_inst_ent_t *ent = klangc_malloc(sizeof(klangc_inst_ent_t));
  ent->kient_inst = inst;
  ent->kient_val = NULL;
  ent->kient_exec_func = exec_func;
  ent->kient_argc = argc;
  ent->kient_args = klangc_malloc(sizeof(klangc_value_t *) * argc);
  for (unsigned int i = 0; i < argc; i++)
    ent->kient_args[i] = args[i];
  ent->kient_next = NULL;
  *inst->kinst_pent_last = ent;
  inst->kinst_pent_last = &ent->kient_next;
  return klangc_value_ioref_new(ent);
}

int klang_inst_addinst(klangc_inst_t *inst, klangc_value_t *val,
                       klangc_value_t **pval) {
  klangc_value_eval(val);
  klangc_value_type_t vtype = klangc_value_get_type(val);
  if (vtype != KLANGC_VTYPE_ALGE)
    return 0;
  klangc_value_alge_t *valge = klangc_value_get_alge(val);
  const klangc_symbol_t *constr = klangc_value_alge_get_constr(valge);
  const klangc_str_t *constr_str = klangc_symbol_get_name(constr);
  unsigned int argc = klangc_value_alge_get_argc(valge);

  klangc_inst_func_spec_t *specs = klangc_inst_func_specs;
  while (specs->name != NULL) {
    if (specs->name_str == NULL)
      specs->name_str = klangc_str_new(specs->name, strlen(specs->name));
    if (constr_str == specs->name_str) {
      if (argc < specs->min_argc || argc > specs->max_argc)
        return 0;
      klangc_value_t *args[argc];
      for (unsigned int i = 0; i < argc; i++)
        args[i] = klangc_value_alge_get_arg(valge, i);
      klangc_value_ioref_t *ioref =
          klangc_inst_ent_add(inst, specs->exec_func, argc, args);
      *pval = klangc_value_new_ioref(ioref);
      return 0;
    }
    specs++;
  }
  return 0;
}