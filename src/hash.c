#include "hash.h"
#include "malloc.h"
#include <assert.h>
#include <string.h>

struct klangc_hash_entry {
  char *khe_key;
  void *khe_value;
  klangc_hash_entry_t *khe_next;
};

struct klangc_hash {
  klangc_hash_entry_t **kh_entries;
  int kh_size;
  int kh_capacity;
};

/**
 * Creates a new hash table with the specified capacity.
 *
 * @param capacity The initial capacity of the hash table.
 * @return A pointer to the newly created hash table.
 */

klangc_hash_t *klangc_hash_new(int capacity) {
  assert(capacity > 0);
  klangc_hash_t *hash = klangc_malloc(sizeof(klangc_hash_t));
  hash->kh_capacity = capacity;
  hash->kh_size = 0;
  hash->kh_entries = klangc_malloc(capacity * sizeof(klangc_hash_entry_t *));
  for (int i = 0; i < capacity; i++) {
    hash->kh_entries[i] = NULL;
  }
  return hash;
}

/**
 * Calculates the hash value for the given key.
 *
 * @param key The key for which the hash value needs to be calculated.
 * @return The calculated hash value.
 */
static int klangc_calc_hash(const char *key) {
  assert(key != NULL);
  int hash = 0;
  for (int i = 0; key[i] != '\0'; i++)
    hash = hash * 31 + key[i];
  return hash;
}

/**
 * Retrieves the value associated with the specified key in the given hash
 * table.
 *
 * @param hash The hash table to search in.
 * @param key The key to search for.
 * @param value A pointer to store the retrieved value.
 * @return 1 if the key was found and the value was retrieved successfully, 0
 * otherwise.
 */
int klangc_hash_get(klangc_hash_t *hash, const char *key, void **value) {
  assert(hash != NULL);
  assert(key != NULL);
  int hash_value = klangc_calc_hash(key) % hash->kh_capacity;
  klangc_hash_entry_t *entry = hash->kh_entries[hash_value];
  while (entry != NULL) {
    if (strcmp(entry->khe_key, key) == 0) {
      if (value != NULL)
        *value = entry->khe_value;
      return 1;
    }
    entry = entry->khe_next;
  }
  return 0;
}

/**
 * Inserts a new entry into the hash table.
 *
 * @param pentry    A pointer to the pointer to the hash entry.
 * @param capacity  The capacity of the hash table.
 * @param size      A pointer to the size of the hash table.
 * @param key       The key of the entry to be inserted.
 * @param value     The value of the entry to be inserted.
 * @param old_value A pointer to store the old value if the key already exists.
 * @return 1 if the entry was successfully inserted, 0 otherwise.
 */
static int klangc_hash_put_raw(klangc_hash_entry_t **pentry, int capacity,
                               int *size, char *key, void *value,
                               void **old_value) {
  assert(pentry != NULL);
  assert(size != NULL);
  assert(capacity >= *size);
  assert(key != NULL);
  while (*pentry != NULL) {
    if (strcmp((*pentry)->khe_key, key) == 0) {
      if (old_value != NULL)
        *old_value = (*pentry)->khe_value;
      (*pentry)->khe_value = value;
      return 1;
    }
    pentry = &(*pentry)->khe_next;
  }
  *pentry = klangc_malloc(sizeof(klangc_hash_entry_t));
  (*pentry)->khe_key = key;
  (*pentry)->khe_value = value;
  (*pentry)->khe_next = NULL;
  (*size)++;
  return 0;
}

/**
 * Walks through the elements of a hash table and performs an operation on each
 * element.
 *
 * @param hash The hash table to walk through.
 * @param callback The callback function to be called on each element.
 * @param data Additional data to be passed to the callback function.
 */
void klangc_hash_walk(klangc_hash_t *hash,
                      void (*callback)(const char *, void *, void *),
                      void *data) {
  assert(hash != NULL);
  assert(callback != NULL);
  for (int i = 0; i < hash->kh_capacity; i++) {
    klangc_hash_entry_t *entry = hash->kh_entries[i];
    while (entry != NULL) {
      callback(entry->khe_key, entry->khe_value, data);
      entry = entry->khe_next;
    }
  }
}

/**
 * Rehashes the given hash table with a new capacity.
 *
 * @param hash The hash table to be rehashed.
 * @param new_capacity The new capacity of the hash table.
 */
static void klangc_hash_rehash(klangc_hash_t *hash, int new_capacity) {
  assert(hash != NULL);
  assert(new_capacity >= hash->kh_size);
  klangc_hash_entry_t **new_entries =
      klangc_malloc(new_capacity * sizeof(klangc_hash_entry_t *));
  for (int i = 0; i < new_capacity; i++)
    new_entries[i] = NULL;
  for (int i = 0; i < hash->kh_capacity; i++) {
    klangc_hash_entry_t *entry = hash->kh_entries[i];
    while (entry != NULL) {
      int hash_value = klangc_calc_hash(entry->khe_key) % new_capacity;
      klangc_hash_put_raw(&new_entries[hash_value], new_capacity,
                          &hash->kh_size, entry->khe_key, entry->khe_value,
                          NULL);
      entry = entry->khe_next;
    }
  }
  klangc_free(hash->kh_entries);
  hash->kh_entries = new_entries;
  hash->kh_capacity = new_capacity;
}

/**
 * Inserts a key-value pair into the specified hash table.
 *
 * @param hash The hash table to insert the key-value pair into.
 * @param key The key to insert.
 * @param value The value to insert.
 * @param old_value A pointer to store the old value if the key already exists.
 * @return 1 if the key-value pair was successfully inserted, 0 otherwise.
 */
int klangc_hash_put(klangc_hash_t *hash, const char *key, void *value,
                    void **old_value) {
  assert(hash != NULL);
  assert(key != NULL);
  if (hash->kh_size >= hash->kh_capacity * 0.75)
    klangc_hash_rehash(hash, hash->kh_capacity * 2);
  return klangc_hash_put_raw(
      &hash->kh_entries[klangc_calc_hash(key) % hash->kh_capacity],
      hash->kh_capacity, &hash->kh_size, klangc_strdup(key), value, old_value);
}

/**
 * Removes an entry from the specified hash table based on the given key.
 *
 * @param hash The hash table from which to remove the entry.
 * @param key The key of the entry to be removed.
 * @param value A pointer to store the value associated with the removed entry.
 * @return 1 if the entry was successfully removed, 0 otherwise.
 */
int klangc_hash_remove(klangc_hash_t *hash, const char *key, void **value) {
  assert(hash != NULL);
  assert(key != NULL);
  int hash_value = klangc_calc_hash(key) % hash->kh_capacity;
  klangc_hash_entry_t **pentry = &hash->kh_entries[hash_value];
  while (*pentry != NULL) {
    if (strcmp((*pentry)->khe_key, key) == 0) {
      if (value != NULL)
        *value = (*pentry)->khe_value;
      klangc_hash_entry_t *entry = *pentry;
      *pentry = entry->khe_next;
      klangc_free(entry->khe_key);
      klangc_free(entry);
      hash->kh_size--;
      return 1;
    }
    pentry = &(*pentry)->khe_next;
  }
  return 0;
}