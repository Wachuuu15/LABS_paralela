#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inicializa el entorno MPI
    MPI_Init(&argc, &argv);

    // Obtiene el rango (ID) del proceso
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtiene el número total de procesos
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Imprime el mensaje desde cada proceso
    printf("Hello World, I’m process %d of %d!\n", rank, size);

    // Finaliza el entorno MPI
    MPI_Finalize();

    return 0;
}
