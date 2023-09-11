/// Simple hash table implemented in C.

#ifndef HT_H
#define HT_H

#include <stdbool.h>
#include <stddef.h>

//=== HASH TABLE ===============================================================

/// Hash table structure: create with ht_create, free with ht_destroy.
typedef struct hash_tbl hash_tbl;

/// Create hash table and return pointer to it, or NULL if out of memory.
hash_tbl* ht_create(void);

/// Free memory allocated for hash table, including allocated keys.
void ht_destroy(hash_tbl* tbl);

/// Get item with given key (NUL-terminated) from hash table. Return
/// value (which was set with ht_set), or NULL if key not found.
void* ht_get(hash_tbl* tbl, const char* key);

/// Set item with given key (NUL-terminated) to value (which must not
/// be NULL). If not already present in table, key is copied to newly
/// allocated memory (keys are freed automatically when ht_destroy is
/// called). Return address of copied key, or NULL if out of memory.
const char* ht_set(hash_tbl* tbl, const char* key, void* val);

/// Return number of items in hash table.
size_t ht_length(hash_tbl* tbl);

//=== ITERATOR =================================================================

/// Hash table iterator: create with ht_iterator, iterate with ht_next.
typedef struct {
    /// current key
    const char* key;

    /// current value
    void* val;

    /// reference to hash table being iterated
    /// Don't use this field directly.
    hash_tbl* _tbl_;

    /// current index into ht._entries
    /// Don't use this field directly.
    size_t _idx_;
} hash_tbl_iter;

/// Return new hash table iterator (for use with ht_next).
hash_tbl_iter ht_iterator(hash_tbl* tbl);

/// Move iterator to next item in hash table, update iterator's key
/// and value to current item, and return true. If there are no more
/// items, return false. Don't call ht_set during iteration.
bool ht_next(hash_tbl_iter* iter);

#endif
