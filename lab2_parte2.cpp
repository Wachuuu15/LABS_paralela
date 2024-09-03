#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <omp.h>
#include <chrono>
#include <sys/stat.h> 

using namespace std;
using namespace chrono; 

void quickSortParallel(int* array, int left, int right, int depth = 0) {
    if (left < right) {
        int pivot = array[(left + right) / 2];
        int i = left, j = right;
        while (i <= j) {
            while (array[i] < pivot) i++;
            while (array[j] > pivot) j--;
            if (i <= j) {
                swap(array[i], array[j]);
                i++;
                j--;
            }
        }

        if (depth < 4) {
            #pragma omp task
            quickSortParallel(array, left, j, depth + 1);
            #pragma omp task
            quickSortParallel(array, i, right, depth + 1);
        } else {
            quickSortParallel(array, left, j, depth + 1);
            quickSortParallel(array, i, right, depth + 1);
        }
    }
}

void quickSort(int* array, int len) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        quickSortParallel(array, 0, len - 1);
    }
}

int main() {
    srand(time(0));

    int N;
    cout << "Ingrese la cantidad de números aleatorios a generar: ";
    cin >> N;

    const char* folderName = "PARALELIZADO";
    mkdir(folderName, 0777); 

    string outputFile1 = string(folderName) + "/numeros.csv";
    string outputFile2 = string(folderName) + "/numeros_ordenados.csv";

    ofstream outFile(outputFile1);
    if (!outFile) {
        cerr << "No se pudo crear el archivo " << outputFile1 << endl;
        return 1;
    }

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        int num = rand() % 100;  // Notar que rand() no es thread-safe
        #pragma omp critical
        {
            outFile << num;
            if (i < N - 1) {
                outFile << ",";
            }
        }
    }
    outFile.close();

    ifstream inFile(outputFile1);
    if (!inFile) {
        cerr << "No se pudo leer el archivo " << outputFile1 << endl;
        return 1;
    }

    int *Array = new int[N];
    string line;
    getline(inFile, line);
    inFile.close();

    stringstream ss(line);
    string temp;
    int index = 0;
    while (getline(ss, temp, ',')) {
        Array[index++] = stoi(temp);
    }

    auto start = high_resolution_clock::now();

    quickSort(Array, N);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    ofstream sortedFile(outputFile2);
    if (!sortedFile) {
        cerr << "No se pudo crear el archivo " << outputFile2 << endl;
        delete[] Array; // Liberar memoria
        return 1;
    }

    for (int i = 0; i < N; i++) {
        sortedFile << Array[i];
        if (i < N - 1) {
            sortedFile << ",";
        }
    }
    sortedFile.close();

    delete[] Array;

    cout << "Los números aleatorios se han generado, clasificado y guardado en numeros_ordenados.csv" << endl;
    cout << "Tiempo de clasificación: " << duration.count() << " microsegundos." << endl;

    return 0;
}
