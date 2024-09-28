#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);

    // rango del proceso
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Obtiene el número total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Variable para el mensaje que será enviado
    char message[50];

    if (rank != 0) {
        // Procesos que no son el 0 envían un mensaje
        sprintf(message, "Hello from process %d to process 0", rank);
        printf("Process %d sending message to process 0\n", rank);
        MPI_Send(message, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        // Proceso 0 recibe mensajes de todos los demás
        for (int i = 1; i < size; i++) {
            MPI_Recv(message, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process 0 received: %s\n", message);
        }
    }

    MPI_Finalize();
    return 0;
}
