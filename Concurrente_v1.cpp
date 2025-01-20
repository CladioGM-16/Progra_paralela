#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <time.h>
using namespace std;
struct Cartas {
    int valor; // 0 para Joker, 1-13 para cartas normales
    int palo;  // 0: Corazones, 1: TrEbol, 2: Espadas, 3: Diamantes, 4: Joker
};

void mezclarSubBaraja(vector<Cartas>& baraja, int inicio, int fin) {
    for (int i = inicio; i < fin; ++i) {
        int indiceAleatorio = inicio + rand() % (fin - inicio);
        Cartas temp = baraja[i];
        baraja[i] = baraja[indiceAleatorio];
        baraja[indiceAleatorio] = temp;
    }
}

void mezclarBaraja(vector<Cartas>& baraja, int numHilos) {
    vector<thread> hilos;
    int tamanoSegmento = baraja.size() / numHilos;
    for (int t = 0; t < numHilos; ++t) {
        int inicio = t * tamanoSegmento;
        int fin = (t == numHilos - 1) ? baraja.size() : inicio + tamanoSegmento;
        hilos.emplace_back(mezclarSubBaraja, ref(baraja), inicio, fin);
    }
    for (auto& hilo : hilos) hilo.join();
    for (int i = 0; i < baraja.size(); ++i) 
        swap(baraja[i], baraja[rand() % baraja.size()]);
}

// ordenamiento burbuja
void ordenarBurbuja(vector<Cartas>& baraja) {
    for (size_t i = 0; i < baraja.size() - 1; ++i)
        for (size_t j = 0; j < baraja.size() - i - 1; ++j)
            if (baraja[j].palo > baraja[j + 1].palo ||
                (baraja[j].palo == baraja[j + 1].palo && baraja[j].valor > baraja[j + 1].valor))
                swap(baraja[j], baraja[j + 1]);
}


void crearBaraja(vector<Cartas>& baraja, int numMazos) {
    for (int m = 0; m < numMazos; ++m)
        for (int palo = 0; palo < 4; ++palo)
            for (int valor = 1; valor <= 13; ++valor)
                baraja.push_back({ valor, palo });
    baraja.push_back({ 0, 4 });
    baraja.push_back({ 0, 4 });
}


void imprimirBaraja(const vector<Cartas>& baraja) {
    const char* palos[] = { "Corazones", "Trebol", "espada", "diamante", "Jokeres" };
    const char* valores[] = { "Joker", "As", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

    for (const auto& carta : baraja) {
        if (carta.palo == 4) {
            cout << valores[0] << " de " << palos[carta.palo] << endl;
        }
        else {
            cout << valores[carta.valor] << " de " << palos[carta.palo] << endl;
        }
    }
}
int main() {
    srand(time(0));
    vector<Cartas> baraja;
    crearBaraja(baraja, 4000 / 54);

    mezclarBaraja(baraja, 4);
    cout << "\nBaraja Mezclada:" << endl;
    imprimirBaraja(baraja);

    ordenarBurbuja(baraja);
    cout << "\nBaraja Ordenada:" << endl;
    imprimirBaraja(baraja);

    return 0;
}
