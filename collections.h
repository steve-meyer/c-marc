// Collections: simple implementations for hash table and linked list data structures.
// Hash table implementation based on: https://github.com/benhoyt/ht

#ifndef __COLLECTIONS__
#define __COLLECTIONS__

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define INITIAL_CAPACITY 30 // must not be zero


typedef struct {
    const char *key; // key is NULL if this slot is empty
    void *value;
} HashTableEntry;


// Hash table structure: create with ht_create, free with ht_destroy.
typedef struct {
    HashTableEntry *entries;  // hash slots
    size_t capacity;          // size of _entries array
    size_t length;            // number of items in hash table
} HashTable;


// Hash table iterator: create with ht_iterator, iterate with ht_next.
typedef struct {
    const char *key; // current key
    void *value;     // current value

    // Don't use these fields directly.
    HashTable *_table; // reference to hash table being iterated
    size_t _index;     // current index into ht._entries
} HashTableIterator;


typedef struct Node {
    void *data;
    struct Node *next;
} Node;


// Create linked list node and return a pointer to it.
Node* Node_create(void *data);


// Free Node from memory.
void Node_destroy(Node *node);


// Create hash table and return pointer to it, or NULL if out of memory.
HashTable* HT_create(void);


// Free memory allocated for hash table, including allocated keys.
void HT_destroy(HashTable *table);


// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with ht_set), or NULL if key not found.
void* HT_get(HashTable *table, const char *key);


// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when ht_destroy is
// called). Return address of copied key, or NULL if out of memory.
const char* HT_set(HashTable *table, const char *key, void *value);


// Return number of items in hash table.
size_t HT_length(HashTable *table);


// Return new hash table iterator (for use with ht_next).
HashTableIterator HT_iterator(HashTable *table);


// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
bool HT_next(HashTableIterator *it);

#endif // _HT_H
