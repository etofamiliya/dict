### Basic usage:

```c
#include "dict.h"

dict_t *vp = dict_new();
char *solo[] = "Alexey Berezin";
char *noone[] = "Vladimir Minenko";
dict_set(vp, "Solo", solo);
dict_set(vp, "Noone", noone);
...
char *query = dict_get(dict, "Solo");
dict_free(vp, 0); // 0 for values placed in static variables


...
usertype *somename = usertype_new();
dict_free(dict, usertype_free)

```
