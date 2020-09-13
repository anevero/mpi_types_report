#include <mpi.h>

void ContiguousExample() {
  MPI_Datatype int_array_16;
  MPI_Type_contiguous(16, MPI_INT, &int_array_16);
  MPI_Type_commit(&int_array_16);
  MPI_Type_free(&int_array_16);
}

void VectorsExample() {
  //  Full array:
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
  //  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
  //  What do we want to send:
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
  //  |  -  |  1  |  -  |  -  |  4  |  -  |  -  |  7  |  -  |
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+

  MPI_Datatype vector;
  MPI_Type_vector(3, 1, 3, MPI_INT, &vector);
  MPI_Type_commit(&vector);
  MPI_Type_free(&vector);

  MPI_Datatype hvector;
  MPI_Type_vector(3, 1, 3 * sizeof(int), MPI_INT, &hvector);
  MPI_Type_commit(&hvector);
  MPI_Type_free(&hvector);
}

void IndexedExample() {
  //  Full array:
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
  //  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
  //  What do we want to send:
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
  //  |  0  |  -  |  -  |  3  |  4  |  -  |  6  |  7  |  8  |
  //  +-----+-----+-----+-----+-----+-----+-----+-----+-----+

  MPI_Datatype indexed;
  int indexed_blocklengths[3] = {1, 2, 3};
  int indexed_displacements[3] = {0, 3, 6};
  MPI_Type_indexed(3, indexed_blocklengths, indexed_displacements,
                   MPI_INT, &indexed);
  MPI_Type_commit(&indexed);
  MPI_Type_free(&indexed);

  MPI_Datatype hindexed;
  int hindexed_blocklengths[3] = {1, 2, 3};
  MPI_Aint hindexed_displacements[3] = {0, 3 * sizeof(int), 6 * sizeof(int)};
  MPI_Type_create_hindexed(3, hindexed_blocklengths, hindexed_displacements,
                           MPI_INT, &hindexed);
  MPI_Type_commit(&hindexed);
  MPI_Type_free(&hindexed);
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  ContiguousExample();
  VectorsExample();
  IndexedExample();

  MPI_Finalize();

  return 0;
}
