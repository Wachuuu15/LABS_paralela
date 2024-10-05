#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Read_n(int* n_p);
void Allocate_vectors(double** x_pp, double** y_pp, double** z_pp, int n);
void Generate_random_vector(double a[], int n);
void Print_vector_partial(double b[], int n, char title[]);
void Vector_sum(double x[], double y[], double z[], int n);

/*---------------------------------------------------------------------*/
int main(void) {
   int n = 70000000;  // Tamaño del vector
   double *x, *y, *z;
   clock_t start, end;
   double cpu_time_used;

   // Medir tiempo de inicio
   start = clock();

   Allocate_vectors(&x, &y, &z, n);

   // Generar los vectores de forma aleatoria
   Generate_random_vector(x, n);
   Generate_random_vector(y, n);

   // Calcular la suma de los vectores
   Vector_sum(x, y, z, n);
   
   // Medir tiempo de fin
   end = clock();

   // Imprimir los primeros y últimos 10 elementos de cada vector
   Print_vector_partial(x, n, "Vector x:");
   Print_vector_partial(y, n, "Vector y:");
   Print_vector_partial(z, n, "Suma de los vectores z:");

   
   // Calcular tiempo transcurrido
   cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000; // En milisegundos
   printf("Took %f ms to run\n", cpu_time_used);

   // Liberar la memoria
   free(x);
   free(y);
   free(z);

   return 0;
}

/*---------------------------------------------------------------------*/
void Allocate_vectors(double** x_pp, double** y_pp, double** z_pp, int n) {
   *x_pp = malloc(n * sizeof(double));
   *y_pp = malloc(n * sizeof(double));
   *z_pp = malloc(n * sizeof(double));
   if (*x_pp == NULL || *y_pp == NULL || *z_pp == NULL) {
      fprintf(stderr, "Can't allocate vectors\n");
      exit(-1);
   }
}

void Generate_random_vector(double a[], int n) {
   int i;
   srand(time(NULL));
   for (i = 0; i < n; i++) {
      a[i] = (double)rand() / RAND_MAX;
   }
}

void Print_vector_partial(double b[], int n, char title[]) {
   int i;
   printf("%s\n", title);
   printf("First 10 elements: ");
   for (i = 0; i < 10; i++)
      printf("%f ", b[i]);
   printf("\nLast 10 elements: ");
   for (i = n - 10; i < n; i++)
      printf("%f ", b[i]);
   printf("\n");
}

void Vector_sum(double x[], double y[], double z[], int n) {
   int i;
   for (i = 0; i < n; i++)
      z[i] = x[i] + y[i];
}
