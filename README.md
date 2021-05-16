### Basic usage:

```c
#include <stdlib.h>
#include <stdio.h>
#include "dict.h"

int main(int argn, char **argv) {
  
  char *query;
  char solo[] = "Alexey Berezin";
  char noone[] = "Vladimir Minenko";
  
  dict_t *vp = dict_new();
  dict_set(vp, "Solo", solo);
  dict_set(vp, "Noone", noone);

  query = dict_get(vp, "Solo");
  printf("%s\n", query); // Alexey Berezin
  query = dict_get(vp, "Noone");
  printf("%s\n", query); // Vladimir Minenko
  
  dict_free(vp, 0);
  
  return 0;  
}

```
