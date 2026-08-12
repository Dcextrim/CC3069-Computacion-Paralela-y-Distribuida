#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

// Cada hilo ejecuta esta funcion sobre un bloque distinto de la lista.
// y el hilo escribe solamente en su propio diccionario local.
void contar_bloque(const vector<string>& palabras,
                   size_t inicio,
                   size_t fin,
                   map<string, int>& frecuencia_local) {
    for (size_t i = inicio; i < fin; ++i) {
        frecuencia_local[palabras[i]]++;
    }
}

int main(int argc, char* argv[]) {
    // contador_paralelo [archivo] [numero_de_hilos]
    string nombre_archivo = (argc >= 2) ? argv[1] : "texto.txt";
    int cantidad_hilos = 2;

    if (argc >= 3) {
        try {
            cantidad_hilos = stoi(argv[2]);
        } catch (...) {
            cerr << "El numero de hilos debe ser un entero mayor o igual que 2" << endl;
            return 1;
        }

        if (cantidad_hilos < 2) {
            cerr << "El numero de hilos debe ser mayor o igual que 2" << endl;
            return 1;
        }
    }

    ifstream archivo(nombre_archivo);

    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << nombre_archivo << endl;
        return 1;
    }

    vector<string> lista;
    string linea;
    string palabra;
    const string signos = ".,;:!?\"'()[]{}-_";

    // La lectura y separacion del archivo se hacen una sola vez.
    while (getline(archivo, linea)) {
        for (size_t i = 0; i < linea.length(); ++i) {
            if (signos.find(linea[i]) != string::npos) {
                linea[i] = ' ';
            }
        }

        stringstream separar(linea);
        while (separar >> palabra) {
            lista.push_back(palabra);
        }
    }

    archivo.close();

    if (lista.empty()) {
        cout << "No hay texto legible para procesar" << endl;
        return 0;
    }

    // La medicion del tiempo empieza cuando el texto ya esta cargado y 
    // tokenizado y se incluye el overhead real de crear/unir hilos y combinar los resultados parciales.
    const auto inicio_conteo = chrono::steady_clock::now();

    // Se crean antes los diccionarios locales. Cada hilo recibe uno
    // diferente, de modo que no es necesario usar mutex durante el conteo.
    vector<map<string, int>> frecuencias_locales(cantidad_hilos);
    vector<thread> hilos;
    hilos.reserve(cantidad_hilos);

    const size_t total_palabras = lista.size();

    for (int id = 0; id < cantidad_hilos; ++id) {
        // Aqui se reparte el residuo y forma bloques contiguos.
        size_t inicio = static_cast<size_t>(id) * total_palabras / cantidad_hilos;
        size_t fin = static_cast<size_t>(id + 1) * total_palabras / cantidad_hilos;

        hilos.emplace_back(contar_bloque,
                           cref(lista),
                           inicio,
                           fin,
                           ref(frecuencias_locales[id]));
    }

    // join funciona como barrera ya que la combinacion 
    //no empieza hasta que todos los hilos hayan terminado su conteo parcial.
    for (thread& hilo : hilos) {
        hilo.join();
    }

    map<string, int> frecuencia_global;

    // El hilo principal combina los N resultados locales una sola vez.
    for (const auto& frecuencia_local : frecuencias_locales) {
        for (const auto& elemento : frecuencia_local) {
            frecuencia_global[elemento.first] += elemento.second;
        }
    }

    const auto fin_conteo = chrono::steady_clock::now();
    const double tiempo_ms =
        chrono::duration<double, milli>(fin_conteo - inicio_conteo).count();

    cout << fixed << setprecision(6)
         << "TIEMPO_PROCESAMIENTO_MS=" << tiempo_ms << '\n';

    // map mantiene las palabras ordenadas, igual que en la version secuencial.
    for (const auto& elemento : frecuencia_global) {
        cout << elemento.first << ": " << elemento.second << endl;
    }

    return 0;
}
