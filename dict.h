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
  size_t (*hashfunc) (char *key);
  dictelem_t **data;
  size_t capacity;
  size_t elements;
} dict_t;

void dict_set(dict_t *dict, char *key, void *value);
dict_t *dict_args(size_t (*hashfunc) (char *));
void *dict_remove(dict_t *dict, char *key);
void *dict_get(dict_t *dict, char *key);
#define dict_new() dict_args(0)
void dict_free(dict_t *dict);

#endif