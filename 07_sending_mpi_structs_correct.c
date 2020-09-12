#include <mpi.h>
#include <stdio.h>

#include "structs.h"

#define CreateType(SomeStruct, type_variable) {                               \
  MPI_Datatype __temp_type;                                                   \
  int __struct_block_lengths[4] = {1, 1, 1, 1};                               \
  MPI_Aint __struct_offsets[4] = {offsetof(SomeStruct, first_number),         \
                                  offsetof(SomeStruct, first_char),           \
                                  offsetof(SomeStruct, second_number),        \
                                  offsetof(SomeStruct, second_char)};         \
  MPI_Datatype __struct_types[4] = {MPI_INT, MPI_CHAR, MPI_INT, MPI_CHAR};    \
  MPI_Type_create_struct(4,                                                   \
                         __struct_block_lengths,                              \
                         __struct_offsets,                                    \
                         __struct_types,                                      \
                         &__temp_type);                                       \
  MPI_Type_create_resized(                                                    \
      __temp_type, 0, sizeof(SomeStruct), &type_variable);                    \
  MPI_Type_commit(&type_variable);                                            \
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int current_process_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

  MPI_Datatype struct_mpi_type;
  if (current_process_rank == 0) {
    CreateType(Struct, struct_mpi_type);
  } else {
    CreateType(ManuallyAlignedStruct, struct_mpi_type);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (current_process_rank == 0) {
    Struct items[2] = {{100500, 'a', 100500, 'a'},
                       {100500, 'a', 100500, 'a'}};
    MPI_Send(items, 2, struct_mpi_type, 1, 42, MPI_COMM_WORLD);
    printf("Process #%d: completed send operations\n", 0);
  } else {
    ManuallyAlignedStruct items[2];
    MPI_Recv(items, 2, struct_mpi_type,
             0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process #%d: received two structs:\n"
           "first_number = %d, first_char = %c, "
           "second_number = %d, second_char = %c\n"
           "first_number = %d, first_char = %c, "
           "second_number = %d, second_char = %c\n",
           current_process_rank,
           items[0].first_number, items[0].first_char,
           items[0].second_number, items[0].second_char,
           items[1].first_number, items[1].first_char,
           items[1].second_number, items[1].second_char);
  }

  MPI_Type_free(&struct_mpi_type);
  MPI_Finalize();

  return 0;
}
