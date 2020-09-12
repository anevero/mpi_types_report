#include <mpi.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  MPI_Datatype int_array_16;
  MPI_Type_contiguous(16, MPI_INT, &int_array_16);
  MPI_Type_commit(&int_array_16);
  MPI_Type_free(&int_array_16);

//      Full array          What we want
//                             to send
//  +-----+-----+-----+  +-----+-----+-----+
//  |  0  |  1  |  2  |  |  -  |  1  |  -  |
//  +-----+-----+-----+  +-----+-----+-----+
//  |  3  |  4  |  5  |  |  -  |  4  |  -  |
//  +-----+-----+-----+  +-----+-----+-----+
//  |  6  |  7  |  8  |  |  -  |  7  |  -  |
//  +-----+-----+-----+  +-----+-----+-----+

  MPI_Datatype column_vector;
  MPI_Type_vector(3, 1, 3, MPI_INT, &column_vector);
  MPI_Type_commit(&column_vector);
  MPI_Type_free(&column_vector);

//      Full array          What we want
//                             to send
//  +-----+-----+-----+  +-----+-----+-----+
//  |  0  |  1  |  2  |  |  0  |  -  |  -  |
//  +-----+-----+-----+  +-----+-----+-----+
//  |  3  |  4  |  5  |  |  3  |  4  |  -  |
//  +-----+-----+-----+  +-----+-----+-----+
//  |  6  |  7  |  8  |  |  6  |  7  |  8  |
//  +-----+-----+-----+  +-----+-----+-----+

  MPI_Datatype indexed_vector;
  int lengths[3] = {1, 2, 3};
  int displacements[3] = {0, 3, 6};
  MPI_Type_indexed(3, lengths, displacements, MPI_INT, &indexed_vector);
  MPI_Type_commit(&indexed_vector);

  MPI_Finalize();

  return 0;
}
