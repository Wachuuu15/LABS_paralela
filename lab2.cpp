#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <ctime>   
#include <string>
#include <sstream>

using namespace std;

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
    ofstream outFile("numeros.csv");
    if (!outFile) {
        cerr << "No se pudo crear el archivo numeros.csv" << endl;
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

    // lectura de números desde el archivo y almacenamiento en memoria
    ifstream inFile("numeros.csv");
    if (!inFile) {
        cerr << "No se pudo leer el archivo numeros.csv" << endl;
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

    // Classifed
    qsort(Array, N, sizeof(int), compare); 

    
    ofstream sortedFile("numeros_ordenados.csv");
    if (!sortedFile) {
        cerr << "No se pudo crear el archivo numeros_ordenados.csv" << endl;
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

    return 0;
}
