#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "dict.h"

/*http://www.cse.yorku.ca/~oz/hash.html*/
static unsigned long djb2(char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}


static dictelem_t *next(dict_t *dict) {
  if (dict->resizing) {
    dict_storage_t *storage = dict->storages;

    for (size_t z = 0; z < 64 && dict->iterator < storage->capacity; z++) {
      dictelem_t *elem = storage->data[dict->iterator];
      if (elem) {
        storage->data[dict->iterator] = elem->next;
        storage->elements--;
        return elem;
      }
      dict->iterator++;
    }
  }
  return NULL;
}


inline static size_t hashget(dict_t *dict, dict_storage_t *storage, char *key) {
  return djb2(key) % storage->capacity;
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

inline static void dictelem_free(dictelem_t *elem) {
  if (elem) {
    free(elem->key);
    free(elem);
  }
}


static dictelem_t *hashed_get(dict_storage_t *storage, char *key, size_t hash) {
  dictelem_t *elem = storage->data[hash];
  while (elem && strncmp(elem->key, key, elem->keylen)) {
    elem = elem->next;
  }
  return elem;
}


inline static dictelem_t *storage_get(
  dict_t *dict, dict_storage_t *storage, char *key) {

  return hashed_get(storage, key, hashget(dict, storage, key));
}


static dictelem_t *storage_set(
  dict_t *dict, dict_storage_t *storage, char *key, void *value) {

  size_t hash = hashget(dict, storage, key);
  dictelem_t *elem = hashed_get(storage, key, hash);
  if (elem) {
    elem->value = value;
  } else {
    storage->data[hash] = dictelem_new(key, value, storage->data[hash]);
    elem = storage->data[hash];
    storage->elements++;
  }
  return elem;
}


static dictelem_t *storage_remove(
  dict_t *dict, dict_storage_t *storage, char *key) {

  size_t hash = hashget(dict, storage, key);
  dictelem_t *elem = storage->data[hash];
  dictelem_t *prev = NULL;

  while(elem) {
    if (strncmp(elem->key, key, elem->keylen) == 0) {
      if (prev) {
        prev->next = elem->next;
      } else {
        storage->data[hash] = elem->next;
      }
      storage->elements--;
      break;
    }
    prev = elem;
    elem = elem->next;
  }
  return elem;
}


static dictelem_t *dict_rehash(
  dict_t *dict, dict_storage_t *storage, dictelem_t *elem) {

  if (elem) {
    dictelem_t *result = storage_set(dict, storage, elem->key, elem->value);
    dictelem_free(elem);
    return result;
  }
  return NULL;
}


static void dict_resize(dict_t *dict) {
  dict_storage_t *prime = dict->storages;
  if (dict->resizing) {
    if (prime->elements == 0) {
      free(prime->data);
      memcpy(dict->storages, dict->storages + 1, sizeof(dict_storage_t));
      memset(dict->storages + 1, 0, sizeof(dict_storage_t));
      dict->resizing = 0;
      dict->iterator = 0;
    }
  } else {
    float loadfactor = (float) prime->elements / (float) prime->capacity;
    int lowload = loadfactor < 0.3;
    int highload = loadfactor > 0.7;
    if (lowload || highload) {
      float multiplier = lowload ? 0.5 : 2.0;
      size_t new_capacity = prime->capacity * multiplier;

      if (new_capacity > 16) {
        dictelem_t **new_data = calloc(new_capacity, sizeof(dictelem_t *));
        if (new_data) {
          dict_storage_t store = {.capacity = new_capacity, .data = new_data};
          memcpy(dict->storages + 1, &store, sizeof(dict_storage_t));
          dict->resizing = 1;
          dict->iterator = 0;
        }
      }
    }
  }
}


void *dict_get(dict_t *dict, char *key) {
  dict_storage_t *storage = dict->storages + dict->resizing;
  if (dict->resizing) {
    dictelem_t *elem = storage_remove(dict, dict->storages, key);
    if (elem) {
      elem = dict_rehash(dict, storage, elem);
      return elem ? elem->value : NULL;
    }
  }
  dictelem_t *elem = storage_get(dict, storage, key);
  return elem ? elem->value : NULL;
}


void dict_set(dict_t *dict, char *key, void *value) {
  dict_storage_t *storage = dict->storages + dict->resizing;
  storage_set(dict, storage, key, value);

  if (dict->resizing) {
    dict_rehash(dict, storage, next(dict));
  }
  dict_resize(dict);
}


void *dict_remove(dict_t *dict, char *key) {
  void *result = NULL;

  for (int s = 0; s < (dict->resizing ? 2 : 1); s++) {
    dict_storage_t *storage = dict->storages + s;
    dictelem_t *elem = storage_remove(dict, storage, key);
    if (elem) {
      result = elem->value;
      dictelem_free(elem);
    }
  }
  dict_resize(dict);
  return result;
}


dict_t *dict_new() {
  dict_t *dict = calloc(1, sizeof(dict_t));
  if (dict) {
    dict_storage_t *prime = dict->storages;
    prime->data = calloc(16, sizeof(dictelem_t *));
    if (prime->data) {
      prime->capacity = 16;
      return dict;
    }
    free(dict);
  }
  return NULL;
}


void dict_free(dict_t *dict, void (freefunc)(void *)) {
  for (int s = 0; s < 2; s++) {
    dict_storage_t *storage = dict->storages + s;
    dictelem_t **data = storage->data;
    for (size_t z = 0; z < storage->capacity; z++) {
      if (data[z]) {
        dictelem_t *next, *elem = data[z];
        while(elem) {
          next = elem->next;
          if (freefunc) freefunc(elem->value);
          dictelem_free(elem);
          elem = next;
        }
      }
    }
    free(storage->data);
  }
  free(dict);
}
