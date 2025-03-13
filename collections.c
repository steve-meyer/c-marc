#include "collections.h"


int next_prime(int x);
int is_prime(const int x);

static HashTableItem HT_DELETED_ITEM = {NULL, NULL};

static HashTableItem* HT_new_item(const char *key, const char *value) {
    HashTableItem *item = malloc(sizeof(HashTableItem));

    item->key = strdup(key);
    item->value = strdup(value);

    return item;
}


static void HT_delete_item(HashTableItem *item) {
    free(item->key);
    free(item->value);
    free(item);
}


static HashTable* HT_new_sized(const int base_size) {
    HashTable *hash_table = malloc(sizeof(HashTable));
    hash_table->base_size = base_size;

    hash_table->size = next_prime(hash_table->base_size);
    hash_table->count = 0;
    hash_table->items = calloc((size_t)hash_table->size, sizeof(HashTableItem*));

    return hash_table;
}


static void HT_resize(HashTable *hash_table, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE)
        return;

    HashTable *new_hash_table = HT_new_sized(base_size);

    for (int i = 0; i < hash_table->size; i++) {
        HashTableItem *item = hash_table->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            HT_insert(new_hash_table, item->key, item->value);
        }
    }

    hash_table->base_size = new_hash_table->base_size;
    hash_table->count = new_hash_table->count;

    // Last step will be to delete the new hash table, so swap the values.
    const int tmp_size = hash_table->size;
    hash_table->size = new_hash_table->size;
    new_hash_table->size = tmp_size;

    HashTableItem **tmp_items = hash_table->items;
    hash_table->items = new_hash_table->items;
    new_hash_table->items = tmp_items;

    HT_delete_hash_table(new_hash_table);
}


static void HT_resize_up(HashTable *hash_table) {
    const int new_size = hash_table->size * 2;
    HT_resize(hash_table, new_size);
}


static void HT_resize_down(HashTable *hash_table) {
    const int new_size = hash_table->size / 2;
    HT_resize(hash_table, new_size);
}


HashTable* HT_new() {
    return HT_new_sized(HT_INITIAL_BASE_SIZE);
}


void HT_delete_hash_table(HashTable *hash_table) {
    for (int i = 0; i < hash_table->size; i++) {
        HashTableItem *item = hash_table->items[i];
        if (item != NULL) {
            HT_delete_item(item);
        }
    }

    free(hash_table->items);
    free(hash_table);
}


/**
 * str: the string to hash
 * a: a prime number larger than the size of the alphabet
 * bucket_size: the number of buckets currently in the hash table, will be modulo'd to get in range
 */
static int HT_hash(const char *str, const int a_prime, const int bucket_size) {
    long hash = 0;
    const int len = strlen(str);

    for (int i = 0; i < len; i++)
        hash += (long) pow(a_prime, len - (i + 1)) * str[i];

    hash = hash % bucket_size;

    return (int) hash;
}


static int HT_get_hash(const char *str, const int num_buckets, const int attempt) {
    const int hash_a = HT_hash(str, HT_PRIME_1, num_buckets);
          int hash_b = HT_hash(str, HT_PRIME_2, num_buckets);

    if (hash_b % num_buckets == 0)
        hash_b = 1;

    return (hash_a + (attempt * hash_b)) % num_buckets;
}


void HT_insert(HashTable *hash_table, const char *key, const char *value) {
    const int load = hash_table->count * 100 / hash_table->size;
    if (load > 70)
        HT_resize_up(hash_table);

    HashTableItem *item = HT_new_item(key, value);

    int index = HT_get_hash(item->key, hash_table->size, 0);
    HashTableItem *current = hash_table->items[index];

    int i = 1;
    while (current != NULL)
    {
        if (current != &HT_DELETED_ITEM) {
            if (strcmp(current->key, key) == 0) {
                HT_delete_item(current);
                hash_table->items[index] = item;
                return;
            }
        }
        index = HT_get_hash(item->key, hash_table->size, i);
        current = hash_table->items[index];
        i++;
    }

    hash_table->items[index] = item;
    hash_table->count++;
}


char* HT_find(HashTable *hash_table, const char *key) {
    int index = HT_get_hash(key, hash_table->size, 0);
    HashTableItem *item = hash_table->items[index];

    int i = 1;
    while (item != NULL)
    {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }

        index = HT_get_hash(key, hash_table->size, i);
        item = hash_table->items[index];
        i++;
    }

    return NULL;
}


void HT_delete(HashTable *hash_table, const char *key) {
    const int load = hash_table->count * 100 / hash_table->size;
    if (load < 30)
        HT_resize_down(hash_table);

    int index = HT_get_hash(key, hash_table->size, 0);
    HashTableItem *item = hash_table->items[index];

    int i = 1;
    while (item != NULL)
    {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                HT_delete_item(item);
                hash_table->items[index] = &HT_DELETED_ITEM;
                hash_table->count--;
                return;
            }
        }

        index = HT_get_hash(key, hash_table->size, i);
        item = hash_table->items[index];
        i++;
    }
}


Node* List_add(struct Node *head, void *data, size_t data_size) {
    Node *new = malloc(sizeof(Node));

    new->data = malloc(data_size);
    new->next = head;

    memcpy(new->data, data, data_size);

    return new;
}


/**
 * Return whether x is prime.
 *
 * Returns:
 *   1  - prime
 *   0  - not prime
 *  -1  - undefined (i.e., x < 2)
 */
int is_prime(const int x) {
    if (x < 2) return -1;
    if (x < 4) return 1;
    if ((x % 2) == 0) return 0;

    for (int i = 3; i <= floor(sqrt((double) x)); i += 1)
        if ((x % i) == 0)
            return 0;

    return 1;
}


/**
 * Return the next prime number after x, or x if x is prime
 */
int next_prime(int x) {
    while(is_prime(x) != 1)
        x++;

    return x;
}
