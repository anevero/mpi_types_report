#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int current_process_rank;
  int processes_number;
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_number);

  MPI_Barrier(MPI_COMM_WORLD);

  const int array_size = 10;

  if (current_process_rank == 0) {
    for (int i = 0; i < processes_number; ++i) {
      int* array = malloc(array_size * sizeof(int));
      if (array == NULL) {
        MPI_Abort(MPI_COMM_WORLD, 1);
      }
      for (int j = 0; j < array_size; ++j) {
        array[j] = i;
      }
      MPI_Send(array, array_size, MPI_INT, i, 42, MPI_COMM_WORLD);
      free(array);
    }
    printf("Process #%d: completed send operations\n", 0);
  }

  int* array = malloc(array_size * sizeof(int));
  if (array == NULL) {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Recv(array, array_size, MPI_INT,
           0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("Process #%d: received array, beginning with %d, ending with %d\n",
         current_process_rank, array[0], array[9]);
  free(array);

  MPI_Finalize();

  return 0;
}
