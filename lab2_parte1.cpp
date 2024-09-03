#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <ctime>   
#include <string>
#include <sstream>
#include <chrono>
#include <sys/stat.h> 
#include <vector>      // Necesario para usar std::vector
#include <numeric>     // Necesario para usar std::accumulate

using namespace std;
using namespace chrono; 

// Función para comparar dos números
int compare (const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    srand(time(0));

    int N;
    cout << "Ingrese la cantidad de números aleatorios a generar: ";
    cin >> N;

    // generación de números aleatorios y escritura en un archivo
    const char* folderName = "SECUENCIAL";
    mkdir(folderName, 0777); 

    string outputFile1 = string(folderName) + "/numeros.csv";
    string outputFile2 = string(folderName) + "/numeros_ordenados.csv";

    ofstream outFile(outputFile1);
    if (!outFile) {
        cerr << "No se pudo crear el archivo " << outputFile1 << endl;
        return 1;
    }

    for (int i = 0; i < N; i++) {
        int num = rand() % 100;
        outFile << num;
        if (i < N - 1) {
            outFile << ",";
        }
    }
    outFile.close();

    ifstream inFile(outputFile1);
    if (!inFile) {
        cerr << "No se pudo leer el archivo " << outputFile1 << endl;
        return 1;
    }

    int *Array = new int[N]; // reserva de memoria
    string line;
    getline(inFile, line);
    inFile.close();

    stringstream ss(line);
    string temp;
    int index = 0;
    while (getline(ss, temp, ',')) {
        Array[index++] = stoi(temp); // se almacena en el arreglo
    }

    // Medir el tiempo de clasificación
    auto start = high_resolution_clock::now();

    // Classifed
    qsort(Array, N, sizeof(int), compare); 

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

    // Liberar la memoria del Heap
    delete[] Array;

    cout << "Los números aleatorios se han generado, clasificado y guardado en numeros_ordenados.csv" << endl;
    cout << "Tiempo de clasificación: " << duration.count() << " microsegundos." << endl;

    return 0;
}
