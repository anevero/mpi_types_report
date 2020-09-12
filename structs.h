#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <stdalign.h>

typedef struct Struct {
  int first_number;
  char first_char;
  int second_number;
  char second_char;
} Struct;

typedef struct __attribute__((__packed__)) PackedStruct {
  int first_number;
  char first_char;
  int second_number;
  char second_char;
} PackedStruct;

typedef struct ManuallyAlignedStruct {
  alignas(8) int first_number;
  alignas(8) char first_char;
  alignas(8) int second_number;
  alignas(8) char second_char;
} ManuallyAlignedStruct;

#endif  // STRUCTS_H_
