#ifndef INCLUDE_HASHMAP__
#define INCLUDE_HASHMAP__
#define HashMap(K,V) HashMap
typedef struct HashMap HashMap;
HashMap *HashMap_new(int (*key_hash_func)(const void *), int (*key_equals_func)(const void *a,const void *b),
        void* (*key_copy_func)(const void *), void (*key_del_func)(void *),
        void* (*val_copy_func)(const void *), void (*val_del_func)(void *));
void HashMap_put(HashMap *map,const void *key,const void *val);
void *HashMap_get(HashMap *map,const void *key);
int HashMap_contains(HashMap *map,const void *key);
void HashMap_remove(HashMap *map,const void *key);
void **HashMap_keys(HashMap *map,int *count);
HashMap *HashMap_copy(HashMap *map);
int HashMap_size(HashMap *map);
void HashMap_destroy(HashMap *map);
#endif

