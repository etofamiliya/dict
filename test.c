/*
  etofamiliya, 2019
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "dict.h"

int main() {
  
  char sokolov[] = "Sokolov";
  char smirnov[] = "Smirnov";
  char ivanov[] = "Ivanon";
  char petrov[] = "Petrov";
  char popov[] = "Popov";
  
  char alexey[] = "Alexey";
  char sergey[] = "Sergey";
  char anton[] = "Anton";
  char pavel[] = "Pavel";
  char ivan[] = "Ivan";
  
  char *query;
  dict_t *names = dict_new();
  
  // dict_set
  dict_set(names, sokolov, alexey);
  dict_set(names, smirnov, sergey);
  dict_set(names, ivanov, anton);
  dict_set(names, petrov, pavel);
  dict_set(names, popov, ivan);
  
  query = dict_get(names, sokolov);
  assert(strcmp(query, alexey) == 0);
  query = dict_get(names, smirnov);
  assert(strcmp(query, sergey) == 0);
  query = dict_get(names, ivanov);
  assert(strcmp(query, anton) == 0);
  query = dict_get(names, petrov);
  assert(strcmp(query, pavel) == 0);
  query = dict_get(names, popov);
  assert(strcmp(query, ivan) == 0);
  
  //dict_set.replacing
  dict_set(names, sokolov, ivan);
  query = dict_get(names, sokolov);
  assert(strcmp(query, ivan) == 0);
  
  //dict_remove
  query = dict_remove(names, sokolov);
  assert(strcmp(query, ivan) == 0);
  
  dict_remove(names, smirnov);
  dict_remove(names, ivanov);
  dict_remove(names, petrov);
  dict_remove(names, popov);
  assert(names->elements == 0);
  assert(names->capacity > 0);
  
  dict_free(names);
  return 0;
}