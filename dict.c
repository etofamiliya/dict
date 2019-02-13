#include <stdlib.h>
#include <string.h>
#include "dict.h"


// http://www.cse.yorku.ca/~oz/hash.html
static size_t djb2(char *str) {
	size_t hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}

inline static size_t hashget(dict_t *dict, char *key) {
  return dict->hashfunc(key) % dict->capacity;
}

inline static float loadfactor_get(dict_t *dict) {
  return (float) dict->elements / (float) dict->capacity;
}

static dictelem_t *dictelem_get(dict_t *dict, size_t hash, char *key) {
  dictelem_t *elem = dict->data[hash];
  while (elem && strcmp(elem->key, key)) {
    elem = elem->next;
  }  
  return elem;
}

static dictelem_t *dictelem_new(char *key, void *value, dictelem_t *next) {
	dictelem_t *elem = calloc(1, sizeof(dictelem_t));
	if (elem) {
    elem->keylen = strlen(key);
    elem->key = calloc(elem->keylen + 1, sizeof(char));
    if (elem->key) {
      memcpy(elem->key, key, elem->keylen);
      elem->value = value;
      elem->next = next;
      return elem;
    }
    free(elem);
	}
	return NULL;
}

static void dictelem_free(dictelem_t *elem) {
  if (elem) {
    free(elem->key);
    free(elem);
  }
}

static int dict_realloc(dict_t *dict, size_t capacity) {
	if (dict) {
		dictelem_t **oldata = dict->data;
		dictelem_t **data = calloc(capacity, sizeof(dictelem_t *));
		if (data) {
      size_t olcapacity = dict->capacity;
			dict->capacity = capacity;
			dict->elements = 0;
			dict->data = data;
      
      dictelem_t *elem, *temp;
			for (size_t i = 0; i < olcapacity; i++) {
				elem = oldata[i];
				while (elem) {
					temp = elem->next;
					dict_set(dict, elem->key, elem->value);
					dictelem_free(elem);
					elem = temp;
				}
			}
			free(oldata);
			return 1;
		}
	}
	return 0;
}

dict_t *dict_args(size_t (*hashfunc) (char *)) {
  dict_t *dict = calloc(1, sizeof(dict_t));
  if (dict) {
    if (dict_realloc(dict, 1)) {
      dict->hashfunc = hashfunc ? hashfunc : djb2;
      return dict;
    }
    free(dict);
  }
  return NULL;
}

void dict_set(dict_t *dict, char *key, void *value) {
  size_t hash = hashget(dict, key);
	dictelem_t *elem = dictelem_get(dict, hash, key);
  if (elem) {
    elem->value = value;
  } else {
    dict->data[hash] = dictelem_new(key, value, dict->data[hash]);
    dict->elements++;
    
    if (loadfactor_get(dict) > 0.75) {
      dict_realloc(dict, dict->capacity * 2);
    }
  }
}

void *dict_get(dict_t *dict, char *key) {
  size_t hash = hashget(dict, key);
	dictelem_t *elem = dictelem_get(dict, hash, key);
	return elem ? elem->value : NULL;
}

void *dict_remove(dict_t *dict, char *key) {
	size_t hash = hashget(dict, key);
	dictelem_t *elem = dict->data[hash];
	dictelem_t *prev = NULL;
	while (elem) {
		if (!strcmp(elem->key, key)) {
			if (prev)  prev->next = elem->next;
			else dict->data[hash] = elem->next;
      void *value = elem->value;
			dictelem_free(elem);
      dict->elements--;
      
      if (loadfactor_get(dict) < 0.25) {
        dict_realloc(dict, dict->capacity * 0.5);
      }
			return value;
		}
		prev = elem;
		elem = elem->next;
	}
  return NULL;
}

void dict_free(dict_t *dict) {
	if (dict) {	
    dictelem_t *elem, *temp;
    for (size_t i = 0; i < dict->capacity; i++) {
			elem = dict->data[i];
      while (elem) {
        temp = elem->next;
        dictelem_free(elem);
        elem = temp;
      }
    }
    free(dict->data);
    free(dict);
	}
}