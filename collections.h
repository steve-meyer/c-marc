#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define HT_INITIAL_BASE_SIZE 53
#define HT_PRIME_1 151
#define HT_PRIME_2 163


typedef struct {
  void *data;
  struct Node *next;
} Node;


typedef struct {
  char *key;
  char *value;
} HashTableItem;


typedef struct {
  int base_size;
  int size;
  int count;
  HashTableItem **items;
} HashTable;


HashTable* HT_new();
void HT_delete_hash_table(HashTable *hash_table);
void HT_insert(HashTable *hash_table, const char *key, const char *value);
char* HT_find(HashTable *hash_table, const char *key);
void HT_delete(HashTable *hash_table, const char *key);


Node* List_add(struct Node *head, void *data, size_t data_size);

#endif
