#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    // Si no se indica otro archivo, se usa el texto de prueba del proyecto.
    string nombre_archivo = (argc >= 2) ? argv[1] : "texto.txt";
    ifstream archivo(nombre_archivo);

    // si no abre el archivo, se termina el programa
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << nombre_archivo << endl;
        return 1;
    }

    vector<string> lista;
    string linea;
    string palabra;

    // signos que quitamos del texto
    string signos = ".,;:!?\"'()[]{}-_";

    while (getline(archivo, linea)) {

        // cambiamos los signos por espacios
        for (size_t i = 0; i < linea.length(); i++) {
            if (signos.find(linea[i]) != string::npos) {
                linea[i] = ' ';
            }
        }

        // Se separa la linea en palabras
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

    // La medicion empieza cuando el texto ya esta cargado y tokenizado. Asi se
    // mide la misma fase que en la version paralela y se excluyen lectura e
    // impresion de resultados.
    const auto inicio_conteo = chrono::steady_clock::now();

    // Se guarda cuantas veces aparece cada palabra
    map<string, int> frecuencias;

    for (size_t i = 0; i < lista.size(); i++) {
        // Misma operacion de actualizacion usada dentro de cada hilo.
        frecuencias[lista[i]]++;
    }

    const auto fin_conteo = chrono::steady_clock::now();
    const double tiempo_ms =
        chrono::duration<double, milli>(fin_conteo - inicio_conteo).count();

    cout << fixed << setprecision(6)
         << "TIEMPO_PROCESAMIENTO_MS=" << tiempo_ms << '\n';

    // Se imprime el resultado final
    for (const auto& elemento : frecuencias) {
        cout << elemento.first << ": " << elemento.second << endl;
    }

    return 0;
}
