// Universidad Del Valle de Guatemala
// Departamento de computación
// Computación paralela y distribuida
// Diana Lucía Fernández Villatoro - 21747
// Jennifer Michelle Toxcón Ordoñez - 21276

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Check_for_error(int local_ok, char fname[], char message[], MPI_Comm comm);
void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Allocate_vectors(double** local_x_pp, double** local_y_pp, double** local_z_pp, int local_n, MPI_Comm comm);
void Generate_random_vector(double local_a[], int local_n, int n, int my_rank, MPI_Comm comm);
void Print_vector_partial(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm);
void Parallel_vector_sum(double local_x[], double local_y[], double local_z[], int local_n);
double Parallel_dot_product(double local_x[], double local_y[], int local_n);
void Scalar_vector_mult(double local_a[], double scalar, int local_n);

/*-------------------------------------------------------------------*/
int main(void) {
    int n = 10, local_n;
    int comm_sz, my_rank;
    double *local_x, *local_y, *local_z;
    MPI_Comm comm;
    double tstart, tend, total_time;
    double dot_product, global_dot_product;
    double scalar = 2.5; 

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    // Distribuir n entre todos los procesos
    local_n = n / comm_sz;

    tstart = MPI_Wtime();
    Allocate_vectors(&local_x, &local_y, &local_z, local_n, comm);

    // Generar los vectores aleatorios
    Generate_random_vector(local_x, local_n, n, my_rank, comm);
    Generate_random_vector(local_y, local_n, n, my_rank, comm);

    if (my_rank == 0){
        printf("\n------------------------------ Vectores originales ------------------------------\n");
    }

    Print_vector_partial(local_x, local_n, n, "Vector X:", my_rank, comm);
    Print_vector_partial(local_y, local_n, n, "Vector Y:", my_rank, comm);

    // Calcular el producto punto
    dot_product = Parallel_dot_product(local_x, local_y, local_n);

    // Reducir el producto punto a un solo valor en el proceso 0
    MPI_Reduce(&dot_product, &global_dot_product, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

    // Multiplicar ambos vectores por el escalar
    Scalar_vector_mult(local_x, scalar, local_n);
    Scalar_vector_mult(local_y, scalar, local_n);

    if (my_rank == 0){
        printf("\n------------------------------ Multiplicación por un escalar ------------------------------\n");
    }

    Print_vector_partial(local_x, local_n, n, "Vector X:", my_rank, comm);
    Print_vector_partial(local_y, local_n, n, "Vector Y:", my_rank, comm);

    tend = MPI_Wtime();
    // Calcular el tiempo total transcurrido
    total_time = (tend - tstart) * 1000; // En milisegundos

    // Solo el proceso 0 imprime el tiempo total y el resultado del producto punto
    if (my_rank == 0) {
        printf("-------------------------------------------------------------------------------------------------\n");
        printf("Producto punto de los vectores originales: %f\n", global_dot_product);

        printf("\n-------------------------------------------------------------------------------------------------\n");
        printf("\nTook %f ms to run\n", total_time);
    }

    free(local_x);
    free(local_y);
    free(local_z);

    MPI_Finalize();
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Allocate_vectors
 * Purpose:   Allocate storage for x, y, and z
 */
void Allocate_vectors(double** local_x_pp, double** local_y_pp, double** local_z_pp, int local_n, MPI_Comm comm) {
    int local_ok = 1;
    char* fname = "Allocate_vectors";
    *local_x_pp = malloc(local_n * sizeof(double));
    *local_y_pp = malloc(local_n * sizeof(double));
    *local_z_pp = malloc(local_n * sizeof(double));
    if (*local_x_pp == NULL || *local_y_pp == NULL || *local_z_pp == NULL) local_ok = 0;
    Check_for_error(local_ok, fname, "Can't allocate local vector(s)", comm);
}

/*-------------------------------------------------------------------
 * Function:  Generate_random_vector
 * Purpose:   Generate a random vector with values between 0 and 1
 */
void Generate_random_vector(double local_a[], int local_n, int n, int my_rank, MPI_Comm comm) {
    double* a = NULL;
    int i;
    if (my_rank == 0) {
        a = malloc(n * sizeof(double));
        for (i = 0; i < n; i++) {
            a[i] = (double)rand() / RAND_MAX;
        }
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);
        free(a);
    } else {
        MPI_Scatter(NULL, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);
    }
}

/*-------------------------------------------------------------------
 * Function:  Print_vector_partial
 * Purpose:   Print the first and last 10 elements of a vector
 */
void Print_vector_partial(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm) {
    double* b = NULL;
    int i;
    if (my_rank == 0) {
        b = malloc(n * sizeof(double));
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, comm);

        printf("%s\n", title);
        // Si n es menor o igual a 10, imprimimos "elementos" y los elementos reales
        if (n <= 10) {
            for (i = 0; i < n; i++)
                printf("%f ", b[i]);
            printf("\n\n");
        } else {
            // Si n es mayor a 10, solo se imprimen los primeros y últimos 10 elementos
            printf("First 10 elements: ");
            for (i = 0; i < 10; i++)
                printf("%f ", b[i]);
            printf("\nLast 10 elements: ");
            for (i = n - 10; i < n; i++)
                printf("%f ", b[i]);
            printf("\n");
        }

        free(b);
    } else {
        MPI_Gather(local_b, local_n, MPI_DOUBLE, NULL, local_n, MPI_DOUBLE, 0, comm);
    }
}

/*-------------------------------------------------------------------
 * Function: Parallel_dot_product
 * Purpose:  Calculate the dot product of two vectors distributed across processes
 */
double Parallel_dot_product(double local_x[], double local_y[], int local_n) {
    double local_dot_product = 0.0;
    int local_i;
    for (local_i = 0; local_i < local_n; local_i++) {
        local_dot_product += local_x[local_i] * local_y[local_i];
    }
    return local_dot_product;
}

/*-------------------------------------------------------------------
 * Function: Scalar_vector_mult
 * Purpose:  Multiply a vector by a scalar
 */
void Scalar_vector_mult(double local_a[], double scalar, int local_n) {
    int local_i;
    for (local_i = 0; local_i < local_n; local_i++) {
        local_a[local_i] *= scalar;
    }
}

/*-------------------------------------------------------------------
 * Function: Check_for_error
 * Purpose: Check whether any process has found an error. If so,
 *          print message and terminate all processes.
 */
void Check_for_error(int local_ok, char fname[], char message[], MPI_Comm comm) {
    int ok;
    MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
    if (ok == 0) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if (my_rank == 0) {
            fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, message);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
}
