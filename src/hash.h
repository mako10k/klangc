#pragma once

#include "klangc.h"

// *******************************
// Hash table.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new hash table.
 * @param capacity Initial capacity.
 * @return New hash table.
 */
klangc_hash_t *klangc_hash_new(int capacity);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the number of entries in a hash table.
 * @param hash Hash table.
 * @param key Key.
 * @param value Pointer to the variable to store the value.
 * @return Number of entries.
 */
int klangc_hash_get(klangc_hash_t *hash, const klangc_str_t *key, void **value);

/**
 * Put a key-value pair into a hash table.
 * @param hash Hash table.
 * @param key Key.
 * @param value Value.
 * @param old_value Pointer to the variable to store the old value.
 * @return 1 if the key already exists, 0 otherwise.
 */
int klangc_hash_put(klangc_hash_t *hash, const klangc_str_t *key, void *value,
                    void **old_value);

/**
 * Remove a key-value pair from a hash table.
 * @param hash Hash table.
 * @param key Key.
 * @param value Pointer to the variable to store the value.
 * @return 1 if the key exists, 0 otherwise.
 */
int klangc_hash_remove(klangc_hash_t *hash, const klangc_str_t *key,
                       void **value);

/**
 * Iterate over the entries of a hash table.
 * @param hash Hash table.
 * @param callback Callback function.
 * @param data Data to be passed to the callback function.
 */
void klangc_hash_foreach(klangc_hash_t *hash,
                         void (*callback)(const klangc_str_t *, void *, void *),
                         void *data);
