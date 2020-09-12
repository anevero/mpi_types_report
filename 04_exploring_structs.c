#include <stddef.h>
#include <stdio.h>

#include "structs.h"

#define PrintStructSizeAndAlligment(SomeStruct) {                             \
  printf("sizeof("#SomeStruct") = %lu\n", sizeof(SomeStruct));                \
  SomeStruct __item;                                                          \
  printf("first_number: offset = %lu, size = %lu\n",                          \
         offsetof(SomeStruct, first_number), sizeof(__item.first_number));    \
  printf("first_char: offset = %lu, size = %lu\n",                            \
         offsetof(SomeStruct, first_char), sizeof(__item.first_char));        \
  printf("second_number: offset = %lu, size = %lu\n",                         \
         offsetof(SomeStruct, second_number), sizeof(__item.second_number));  \
  printf("second_char: offset = %lu, size = %lu\n",                           \
         offsetof(SomeStruct, second_char), sizeof(__item.second_char));      \
}

int main() {
  PrintStructSizeAndAlligment(Struct);
  printf("\n");
  PrintStructSizeAndAlligment(PackedStruct);
  printf("\n");
  PrintStructSizeAndAlligment(ManuallyAlignedStruct);

  return 0;
}
