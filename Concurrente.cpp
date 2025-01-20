#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono> 
using namespace std;
using namespace std::chrono;

struct Cartas {
    int valor; // 0 para Joker, 1-13 para cartas normales
    int palo;  // 0: Corazones, 1: Trebol, 2: Espada, 3: diamante, 4: jokers
};

void mezclarSubMazo(vector<Cartas>& baraja, int inicio, int fin) {
    for (int i = inicio; i < fin; ++i) {
        int Alt = inicio + rand() % (fin - inicio);
        Cartas temp = baraja[i];
        baraja[i] = baraja[Alt];
        baraja[Alt] = temp;
    }
}

void mezclarBaraja(vector<Cartas>& baraja, int numHilos) {
    vector<thread> hilos;
    int tamPorcion = baraja.size() / numHilos;

    for (int t = 0; t < numHilos; ++t) {
        int inicio = t * tamPorcion;
        int fin;
        if (t == numHilos - 1) {
            fin = baraja.size();
        } else {
            fin = inicio + tamPorcion;
        }
        hilos.emplace_back(mezclarSubMazo, ref(baraja), inicio, fin);
    }

    for (auto& hilo : hilos) {
        hilo.join(); // Esperar a que todos los hilos terminen
    }
}



void merge(vector<Cartas>& deck, int left, int mid, int right) {
    vector<Cartas> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (deck[i].palo < deck[j].palo ||
            (deck[i].palo == deck[j].palo && deck[i].palo <= deck[j].palo)) {
            temp[k++] = deck[i++];
        }
        else {
            temp[k++] = deck[j++];
        }
    }

    while (i <= mid) temp[k++] = deck[i++];
    while (j <= right) temp[k++] = deck[j++];

    for (int p = 0; p < temp.size(); ++p) {
        deck[left + p] = temp[p];
    }
}

// Merge sort for a sub-deck
void sortSubDeck(vector<Cartas>& deck, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        sortSubDeck(deck, left, mid);
        sortSubDeck(deck, mid + 1, right);
        merge(deck, left, mid, right);
    }
}



void ordenarBaraja(vector<Cartas>& baraja, int numHilos) {
    vector<thread> hilos;
    int tamPorcion = baraja.size() / numHilos;

    for (int t = 0; t < numHilos; ++t) {
        int inicio = t * tamPorcion;
        int fin = (t == numHilos - 1) ? baraja.size() - 1 : inicio + tamPorcion - 1;
        hilos.emplace_back(sortSubDeck, ref(baraja), inicio, fin);
    }

    for (auto& hilo : hilos) {
        hilo.join(); // Esperar a que todos los hilos terminen
    }

    // Fusionar las partes ordenadas por los hilos
    for (int i = 0; i < numHilos - 1; ++i) {
        int izquierda = i * tamPorcion;
        int derecha = (i + 1 == numHilos) ? baraja.size() - 1 : (i + 1) * tamPorcion - 1;
        merge(baraja, 0, izquierda + tamPorcion - 1, derecha);
    }
}


void crearBaraja(vector<Cartas>& baraja, int numBarajas) {
    for (int d = 0; d < numBarajas; ++d) {
        for (int palo = 0; palo < 4; ++palo) {
            for (int valor = 1; valor <= 13; ++valor) {
                baraja.push_back({ valor, palo });
            }
        }
        baraja.push_back({ 0, 4 }); 
        baraja.push_back({ 0, 4 });
    }
}

int main() {
    srand(time(NULL));
    vector<Cartas> baraja;
    crearBaraja(baraja, 4000000 / 54);
    auto inicioMezcla = high_resolution_clock::now();
    mezclarBaraja(baraja, 4); 
    auto finMezcla = high_resolution_clock::now();
    auto duracionMezcla = duration_cast<milliseconds>(finMezcla - inicioMezcla).count();
    cout << "\nTarea 1: Mezcla completa en " << duracionMezcla << " ms." << endl;
    auto inicioOrden = high_resolution_clock::now();
    ordenarBaraja(baraja, 4);
    auto finOrden = high_resolution_clock::now();
    auto duracionOrden = duration_cast<milliseconds>(finOrden - inicioOrden).count();
    cout << "\nTarea 2: Ordenación completa en " << duracionOrden << " ms." << endl;

    return 0;
}
