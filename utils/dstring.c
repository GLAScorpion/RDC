#include "dstring.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void CreateStr(struct Dstring *string, const char *new_str) {
  string->size = strlen(new_str);
  string->alloc_size = 16 + 1.5 * string->size;
  string->string = malloc(string->alloc_size);
  strcat(string->string, new_str);
}

void ConcatStr(struct Dstring *string, const char *other) {
  size_t other_len = strlen(other);
  size_t tmp_new = other_len + string->size;
  if (tmp_new + 1 > string->alloc_size) {
    char *tmp_ptr = realloc(string->string, 1.5 * tmp_new);
    if (tmp_ptr == NULL) {
      return;
    }
    string->string = tmp_ptr;
    string->alloc_size = 1.5 * tmp_new;
  }
  strcat(string->string, other);
  string->size = tmp_new;
}

void ConcatInt(struct Dstring *string, size_t num) {
  size_t add_size = 1;
  if (num > 0) {
    add_size += log10(num);
  }
  size_t tmp_new = add_size + string->size;
  if (tmp_new + 1 > string->alloc_size) {
    char *tmp_ptr = realloc(string->string, 1.5 * tmp_new);
    if (tmp_ptr == NULL) {
      return;
    }
    string->string = tmp_ptr;
    string->alloc_size = 1.5 * tmp_new;
  }
  sprintf((string->string + string->size), "%d", num);
  string->size = tmp_new;
}

void DestroyStr(struct Dstring *string) { free(string->string); }
