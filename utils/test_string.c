#include "dstring.h"
#include <stdio.h>
int main() {
  struct Dstring str;
  CreateStr(&str, "Test");
  printf("La stringa è %s.\nE' lunga %d e occupa %d\n", str.string, str.size,
         str.alloc_size);
  ConcatStr(&str, "TestTestTestTest");

  printf("La stringa è %s.\nE' lunga %d e occupa %d\n", str.string, str.size,
         str.alloc_size);
  ConcatInt(&str, 22);
  printf("La stringa è %s.\nE' lunga %d e occupa %d\n", str.string, str.size,
         str.alloc_size);
}
