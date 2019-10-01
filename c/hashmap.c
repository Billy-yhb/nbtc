#include "hashmap.h"
#include <stdlib.h>
typedef int (*HASHFUNC)(const void *);
typedef int (*EQUALSFUNC)(const void *,const void *);
typedef void* (*COPYFUNC)(const void *);
typedef void (*DELFUNC)(void *);
typedef struct Node{
    void *key;
    void *val;
    struct Node *next;
}Node;
struct HashMap{
    HASHFUNC key_hash;
    EQUALSFUNC key_equals;
    COPYFUNC key_copy;
    DELFUNC key_del;

    COPYFUNC val_copy;
    DELFUNC val_del;

    Node *arr[258];
    unsigned int count;
};
HashMap *HashMap_new(HASHFUNC key_hash_func, EQUALSFUNC key_equals_func,
        COPYFUNC key_copy_func, DELFUNC key_del_func,
        COPYFUNC val_copy_func, DELFUNC val_del_func){
    HashMap *map=malloc(sizeof(HashMap));
    map->key_hash=key_hash_func;
    map->key_equals=key_equals_func;
    map->key_copy=key_copy_func;
    map->key_del=key_del_func;
    map->val_copy=val_copy_func;
    map->val_del=val_del_func;
    for(int i=0;i<256;i++){
        map->arr[i]=(void *)0;
    }
    map->count=0;
    return map;
}
void HashMap_put(HashMap *map,const void *key,const void *val){
    int hash=map->key_hash(key)%256;
    Node *lt=map->arr[hash];
    if(!lt){
        lt=map->arr[hash]=malloc(sizeof(Node));
        lt->key=map->key_copy(key);
        lt->val=map->val_copy(val);
        lt->next=NULL;
        map->count++;
    }else{
        if(map->key_equals(key,lt->key)){
            map->val_del(lt->val);
            lt->val=map->val_copy(val);
        }else{
            while(lt->next&&(!map->key_equals(lt->key,key))){
                lt=lt->next;
            }
            if(map->key_equals(lt->key,key)){
                map->val_del(lt->val);
                lt->val=map->val_copy(val);
            }else{
                lt->next=malloc(sizeof(Node));
                lt=lt->next;
                lt->key=map->key_copy(key);
                lt->val=map->val_copy(val);
                lt->next=NULL;
                map->count++;
            }
        }
    }
}
void *HashMap_getNonCopy(HashMap *map,const void *key){
    int hash=map->key_hash(key)%256;
    Node *lt=map->arr[hash];
    while(lt&&(!map->key_equals(key,lt->key))){
        lt=lt->next;
    }
    if(lt){
        return lt->val;
    }else{
        return NULL;
    }
}
void *HashMap_get(HashMap *map,const void *key){
    void *rval=HashMap_getNonCopy(map,key);
    return rval?map->val_copy(rval):NULL;
}
int HashMap_contains(HashMap *map,const void *key){
    int hash=map->key_hash(key)%256;
    Node *lt=map->arr[hash];
    while(lt&&(!map->key_equals(key,lt->key))){
        lt=lt->next;
    }
    if(lt){
        return 1;
    }else{
        return 0;
    }
}
void HashMap_remove(HashMap *map,const void *key){
    int hash=map->key_hash(key)%256;
    Node *lt=map->arr[hash];
    if(!lt){
        return;
    }
    if(map->key_equals(key,lt->key)){
        map->arr[hash]=lt->next;
        map->key_del(lt->key);
        map->val_del(lt->val);
        free(lt);
        map->count--;
        return;
    }else if(!lt->next){
        return;
    }
    while(lt->next&&(!map->key_equals(lt->next->key,key))){
        lt=lt->next;
    }
    if(!map->key_equals(lt->next->key,key)){
        return;
    }
    Node *k=lt->next;
    lt->next=k->next;
    map->key_del(k->key);
    map->val_del(k->val);
    free(k);
    map->count--;
}
void **HashMap_keys(HashMap *map,int *count){
    void **rval=malloc(sizeof(void*)*map->count);
    *count=map->count;
    int j=0;
    for(int i=0;i<256;i++){
        Node *lt=map->arr[i];
        while(lt){
            rval[j]=map->key_copy(lt->key);
            j++;
            lt=lt->next;
        }
    }
    return rval;
}
HashMap *HashMap_copy(HashMap *map){
    HashMap *rval=HashMap_new(map->key_hash,map->key_equals,map->key_copy,map->key_del,map->val_copy,map->val_del);
    for(int i=0;i<256;i++){
        Node *lt=map->arr[i];
        while(lt){
            HashMap_put(rval,lt->key,lt->val);
            lt=lt->next;
        }
    }
    return rval;
}
int HashMap_size(HashMap *map){
    return map->count;
}
void HashMap_destroy(HashMap *map){
    for(int i=0;i<256;i++){
        Node *lt=map->arr[i];
        while(lt){
            map->key_del(lt->key);
            map->val_del(lt->val);
            Node *k=lt;
            lt=lt->next;
            free(k);
        }
    }
    free(map);
}
HashMapLoc HashMap_iterator(HashMap *map){
    int i;
    for(i=0;i<256;i++){
        if(map->arr[i]){
            break;
        }
    }
    HashMapLoc rval={map,i,i<256,i<256?map->arr[i]:NULL};
    return rval;
}
HashMapEntry HashMap_next(HashMapLoc *iterator){
    Node *node=iterator->node;
    HashMapEntry rval={node->key,node->val};
    if(node->next){
        iterator->node=node->next;
    }else{
        int i;
        for(i=iterator->hash+1;i<256;i++){
            if(((HashMap *)(iterator->map))->arr[i]){
                break;
            }
        }
        iterator->hash=i;
        iterator->hasNext=i<256;
        iterator->node=iterator->hasNext?((HashMap *)(iterator->map))->arr[i]:NULL;
    }
    return rval;
}
