#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <mutex>

using namespace std;
using namespace std::chrono;

struct Carta {
    int valor; // 0 para Joker, 1-13 para cartas normales
    int palo;  // 0: Corazones, 1: Trebol, 2: Espadas, 3: Diamantes, 4: Jokers
};

// Función para mezclar una parte del mazo
void mezclarParte(vector<Carta>& mazo, int inicio, int fin, mutex& mtx) {
    for (int i = inicio; i < fin; ++i) {
        lock_guard<mutex> lock(mtx);
        int aleatorio = inicio + rand() % (fin - inicio);
        swap(mazo[i], mazo[aleatorio]);
    }
}

// Mezclar el mazo en paralelo
void shuffleDeck(vector<Carta>& deck, int numThreads) {
    vector<thread> threads;
    mutex mtx;
    int chunkSize = deck.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end;
        if (i == numThreads - 1) {
            end = deck.size();
        }
        else {
            end = start + chunkSize;
        }
        threads.emplace_back(mezclarParte, ref(deck), start, end, ref(mtx));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int i = 0; i < deck.size(); ++i) {
        int randomIndex = rand() % deck.size();
        swap(deck[i], deck[randomIndex]);
    }

// Función para combinar dos partes ordenadas
void merge(vector<Carta>& mazo, int inicio, int medio, int fin) {
    vector<Carta> temp(fin - inicio + 1);
    int i = inicio, j = medio + 1, k = 0;

    while (i <= medio && j <= fin) {
        if (mazo[i].palo < mazo[j].palo ||
            (mazo[i].palo == mazo[j].palo && mazo[i].valor <= mazo[j].valor)) {
            temp[k++] = mazo[i++];
        }
        else {
            temp[k++] = mazo[j++];
        }
    }

    while (i <= medio) temp[k++] = mazo[i++];
    while (j <= fin) temp[k++] = mazo[j++];

    for (int i = 0; i < temp.size(); ++i) {
        mazo[inicio + i] = temp[i];
    }
}

void mergeSort(vector<Carta>& deck, int start, int end) {
    if (start < end) {
        int mid = start + (end - start) / 2;
        mergeSort(deck, start, mid);
        mergeSort(deck, mid + 1, end);
        merge(deck, start, mid, end);
    }
}


// merge sort
void parallelMergeSort(vector<Carta>& mazo, int inicio, int fin, int numHilos) {
    if (numHilos <= 1 || fin - inicio <= 1000) {
        mergeSort(mazo, inicio, fin); // Ordenar secuencialmente si hay pocos hilos o datos pequeños
    }
    else {
        int medio = inicio + (fin - inicio) / 2;
        thread hiloIzq(parallelMergeSort, ref(mazo), inicio, medio, numHilos / 2);
        thread hiloDer(parallelMergeSort, ref(mazo), medio + 1, fin, numHilos / 2);
        hiloIzq.join();
        hiloDer.join();
        merge(mazo, inicio, medio, fin);
    }
}

void crearMazo(vector<Carta>& mazo, int numBarajas) {
    for (int i = 0; i < numBarajas; ++i) {
        for (int palo = 0; palo < 4; ++palo) {
            for (int valor = 1; valor <= 13; ++valor) {
                mazo.push_back({ valor, palo });
            }
        }
        mazo.push_back({ 0, 4 }); 
        mazo.push_back({ 0, 4 });
    }
}

int main() {
    srand(time(NULL));
    vector<Carta> mazo;
    crearMazo(mazo, 4000000 / 54);
    auto inicioMezcla = high_resolution_clock::now();
    mezclarMazo(mazo, 4); 
    auto finMezcla = high_resolution_clock::now();
    auto tiempoMezcla = duration_cast<milliseconds>(finMezcla - inicioMezcla).count();
    cout << "Tarea 1: Mezcla completa en " << tiempoMezcla << " ms." << endl;

    auto inicioOrden = high_resolution_clock::now();
    parallelMergeSort(mazo, 0, mazo.size() - 1, 4); 
    auto finOrden = high_resolution_clock::now();
    auto tiempoOrden = duration_cast<milliseconds>(finOrden - inicioOrden).count();
    cout << "Tarea 2: Ordenacion completa en " << tiempoOrden << " ms." << endl;

    return 0;
}