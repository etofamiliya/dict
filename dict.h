#ifndef _dict_h_
#define _dict_h_

#include <stddef.h>

typedef struct dictelem_t dictelem_t;
struct dictelem_t {
  dictelem_t *next;
  size_t keylen;
  void *value;  
  char *key;
};

typedef struct {
  dictelem_t **data;
  size_t capacity;
  size_t elements;
} dict_storage_t;

typedef struct {
	dict_storage_t storages[2];
	size_t iterator;
	int resizing;
} dict_t;


void *dict_get(dict_t *dict, char *key);
void  dict_set(dict_t *dict, char *key, void *value);
void *dict_remove(dict_t *dict, char *key);

dict_t *dict_new();
void dict_free(dict_t *dict, void (freefunc)(void *));

#endif