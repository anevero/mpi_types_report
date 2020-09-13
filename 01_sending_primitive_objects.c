#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int current_process_rank;
  int processes_number;
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_number);

  MPI_Barrier(MPI_COMM_WORLD);

  if (current_process_rank == 0) {
    for (int i = 0; i < processes_number; ++i) {
      MPI_Send(&i, 1, MPI_INT, i, 42, MPI_COMM_WORLD);
    }
    printf("Process #%d: completed send operations\n", 0);
  }

  int number;
  MPI_Recv(&number, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("Process #%d: received %d\n", current_process_rank, number);

  MPI_Finalize();

  return 0;
}
