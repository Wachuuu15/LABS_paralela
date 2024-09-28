#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);

    // Obtiene el rango del proceso
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Obtiene el número total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Variable para el mensaje que será enviado y la confirmación de respuesta
    char message[50];
    char ack[50];

    if (rank != 0) {
        // Procesos que no son el 0 envían un mensaje
        sprintf(message, "Hello from process %d of %d to process 0", rank, size);
        printf("Process %d: Sending message to process 0\n", rank);
        MPI_Send(message, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        // Espera la confirmación del proceso 0
        MPI_Recv(ack, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d: Received acknowledgment from process 0: %s\n", rank, ack);
    } else {
        // Proceso 0 recibe mensajes y envía confirmación
        for (int i = 1; i < size; i++) {
            MPI_Recv(message, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process 0: Received: %s\n", message);

            // Enviar confirmación al proceso que envió el mensaje
            printf(ack, "Acknowledgment from process 0 to process %d", i);
            MPI_Send(ack, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
