/// Simple hash table implemented in C.

#include "ht.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/// must not be zero
#define INITIAL_CAPACITY 16
#define FNV_OFFSET       14695981039346656037UL
#define FNV_PRIME        1099511628211UL

//=== HASH TABLE ===============================================================

/// Hash table entry (slot may be filled or empty).
typedef struct {
    // key is NULL if this slot is empty
    const char* key;

    void* val;
} ht_entry;

/// Hash table structure: create with ht_create, free with ht_destroy.
struct hash_tbl {
    // hash slots
    ht_entry* entries;

    // size of _entries array
    size_t capacity;

    /// number of items in hash table
    size_t len;
};

hash_tbl* ht_create(void) {
    // Allocate space for hash table struct.
    hash_tbl* tbl = malloc(sizeof(hash_tbl));
    if (tbl == NULL) {
        return NULL;
    }
    tbl->len = 0;
    tbl->capacity = INITIAL_CAPACITY;

    // Allocate (zero'd) space for entry buckets.
    tbl->entries = calloc(tbl->capacity, sizeof(ht_entry));
    if (tbl->entries == NULL) {
        // error, free tbl before we return!
        free(tbl);
        return NULL;
    }
    return tbl;
}

void ht_destroy(hash_tbl* tbl) {
    // First free allocated keys.
    for (size_t i = 0; i < tbl->capacity; i++) {
        free((void*)tbl->entries[i].key);
    }
    // Then free entries array and table itself.
    free(tbl->entries);
    free(tbl);
}

/// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
/// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* ht_get(hash_tbl* tbl, const char* key) {
    /// AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t idx = (size_t)(hash & (uint64_t)(tbl->capacity - 1));

    // Loop till we find an empty entry.
    while (tbl->entries[idx].key != NULL) {
        if (strcmp(key, tbl->entries[idx].key) == 0) {
            // Found key, return value.
            return tbl->entries[idx].val;
        }
        // Key wasn't in this slot, move to next (linear probing).
        idx++;
        if (idx >= tbl->capacity) {
            // At end of entries array, wrap around.
            idx = 0;
        }
    }
    return NULL;
}

/// Internal function to set an entry (without expanding table).
static const char* ht_set_entry(
    ht_entry* entries,
    size_t capacity,
    const char* key,
    void* val,
    size_t* plength
) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t idx = (size_t)(hash & (uint64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[idx].key != NULL) {
        if (strcmp(key, entries[idx].key) == 0) {
            // Found key (it already exists), update value.
            entries[idx].val = val;
            return entries[idx].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        idx++;
        if (idx >= capacity) {
            // At end of entries array, wrap around.
            idx = 0;
        }
    }
    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[idx].key = (char*)key;
    entries[idx].val = val;
    return key;
}

/// Expand hash table to twice its current size. Return true on success,
/// false if out of memory.
static bool ht_expand(hash_tbl* tbl) {
    // Allocate new entries array.
    size_t new_capacity = tbl->capacity * 2;
    if (new_capacity < tbl->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }
    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < tbl->capacity; i++) {
        ht_entry entry = tbl->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key, entry.val, NULL);
        }
    }
    // Free old entries array and update this table's details.
    free(tbl->entries);
    tbl->entries = new_entries;
    tbl->capacity = new_capacity;
    return true;
}

const char* ht_set(hash_tbl* tbl, const char* key, void* val) {
    assert(val != NULL);
    if (val == NULL) {
        return NULL;
    }
    // If length will exceed half of current capacity, expand it.
    if (tbl->len >= tbl->capacity / 2) {
        if (!ht_expand(tbl)) {
            return NULL;
        }
    }
    // Set entry and update length.
    return ht_set_entry(tbl->entries, tbl->capacity, key, val, &tbl->len);
}

size_t ht_length(hash_tbl* tbl) {
    return tbl->len;
}

//=== ITERATOR =================================================================

hash_tbl_iter ht_iterator(hash_tbl* tbl) {
    hash_tbl_iter iter;
    iter._tbl_ = tbl;
    iter._idx_ = 0;
    return iter;
}

bool ht_next(hash_tbl_iter* iter) {
    // Loop till we've hit end of entries array.
    hash_tbl* tbl = iter->_tbl_;
    while (iter->_idx_ < tbl->capacity) {
        size_t i = iter->_idx_;
        iter->_idx_++;
        if (tbl->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            ht_entry entry = tbl->entries[i];
            iter->key = entry.key;
            iter->val = entry.val;
            return true;
        }
    }
    return false;
}
