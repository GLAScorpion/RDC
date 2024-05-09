#include "dstring.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void CreateStr(struct Dstring *string, const char *new_str) {
  string->size = strlen(new_str);
  if (string->size > 0) {
    string->alloc_size = 2 << (int)ceil(log2(string->size));
  } else {
    string->alloc_size = 2;
  }
  string->string = malloc(string->alloc_size);
  strcpy(string->string, new_str);
}

void CreateStrFromInt(struct Dstring *string, size_t num) {
  string->size = 1;
  if (num > 0) {
    string->size += log10(num);
  }
  string->alloc_size = 2 << (int)ceil(log2(string->size));
  string->string = malloc(string->alloc_size);
  sprintf(string->string, "%lu", num);
}

void ConcatStr(struct Dstring *string, const char *other) {
  size_t other_len = strlen(other);
  if (other_len == 0) {
    return;
  }
  size_t tmp_new = other_len + string->size;
  if (tmp_new + 1 > string->alloc_size) {
    size_t new_size = 2 << (size_t)ceil(log2(tmp_new + 1));
    char *tmp_ptr = realloc(string->string, new_size);

    if (tmp_ptr == NULL) {
      return;
    }
    string->string = tmp_ptr;
    string->alloc_size = new_size;
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
    size_t new_size = 2 << (size_t)ceil(log2(tmp_new + 1));
    char *tmp_ptr = realloc(string->string, new_size);
    if (tmp_ptr == NULL) {
      return;
    }
    string->string = tmp_ptr;
    string->alloc_size = new_size;
  }
  sprintf((string->string + string->size), "%lu", num);
  string->size = tmp_new;
}

void AppendChar(struct Dstring *string, char *c) {
  size_t old = string->size;
  size_t tmp_new = 1 + string->size;
  if (tmp_new + 1 > string->alloc_size) {
    size_t new_size = 2 << (size_t)ceil(log2(tmp_new + 1));
    char *tmp_ptr = realloc(string->string, new_size);
    if (tmp_ptr == NULL) {
      return;
    }
    string->string = tmp_ptr;
    string->alloc_size = new_size;
  }
  string->string[old] = *c;
  string->size = tmp_new;
}

void DestroyStr(struct Dstring *str) {
  free(str->string);
  str->string = NULL;
  str->size = 0;
  str->alloc_size = 0;
}

void CopyStr(struct Dstring *string, const char *other) {
  DestroyStr(string);
  CreateStr(string, other);
}

void CopyIntToStr(struct Dstring *string, int num) {
  DestroyStr(string);
  CreateStrFromInt(string, num);
}
