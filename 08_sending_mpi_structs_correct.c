#include <mpi.h>
#include <stdio.h>

#include "structs.h"

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int current_process_rank;
  int processes_number;
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_number);

  MPI_Datatype temp_type;
  MPI_Datatype struct_mpi_type;

  int struct_block_lengths[4] = {1, 1, 1, 1};
  MPI_Aint struct_offsets[4] = {offsetof(Struct, first_number),
                                offsetof(Struct, first_char),
                                offsetof(Struct, second_number),
                                offsetof(Struct, second_char)};
  MPI_Datatype struct_types[4] = {MPI_INT, MPI_CHAR, MPI_INT, MPI_CHAR};

  MPI_Type_create_struct(4,
                         struct_block_lengths,
                         struct_offsets,
                         struct_types,
                         &temp_type);
  MPI_Type_create_resized(temp_type, 0, sizeof(Struct), &struct_mpi_type);
  MPI_Type_commit(&struct_mpi_type);

  MPI_Barrier(MPI_COMM_WORLD);

  if (current_process_rank == 0) {
    for (int i = 0; i < processes_number; ++i) {
      Struct item = {
          .first_number = i,
          .first_char = 'a',
          .second_number = i,
          .second_char = 'a'
      };
      MPI_Send(&item, 1, struct_mpi_type,
               i, 42, MPI_COMM_WORLD);
    }
    printf("Process #%d: completed send operations\n", 0);
  }

  Struct item;
  MPI_Recv(&item, 1, struct_mpi_type,
           0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("Process #%d: received struct, "
         "first_number = %d, first_char = %c, "
         "second_number = %d, second_char = %c\n",
         current_process_rank,
         item.first_number, item.first_char,
         item.second_number, item.second_char);

  MPI_Type_free(&struct_mpi_type);
  MPI_Finalize();

  return 0;
}
