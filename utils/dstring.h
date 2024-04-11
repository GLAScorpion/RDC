#include <string.h>

struct Dstring {
  char *string;
  size_t alloc_size;
  size_t size;
};

void CreateStr(struct Dstring *string, const char *new_str);

void ConcatStr(struct Dstring *string, const char *other);

void ConcatInt(struct Dstring *string, size_t num);

void DestroyStr(struct Dstring *string);
