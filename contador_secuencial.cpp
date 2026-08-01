#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main() {
    string nombre_archivo = "data/texto.txt";
    ifstream archivo(nombre_archivo);

    // si no abre el archivo, se termina el programa
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo" << endl;
        return 1;
    }

    vector<string> lista;
    string linea;
    string palabra;

    // signos que quitamos del texto
    string signos = ".,;:!?\"'()[]{}-_";

    while (getline(archivo, linea)) {

        // cambiamos los signos por espacios
        for (int i = 0; i < linea.length(); i++) {
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

    // Se guarda cuantas veces aparece cada palabra
    map<string, int> frecuencias;

    for (int i = 0; i < lista.size(); i++) {
        palabra = lista[i];

        if (frecuencias.find(palabra) != frecuencias.end()) {
            frecuencias[palabra] = frecuencias[palabra] + 1;
        } else {
            frecuencias[palabra] = 1;
        }
    }

    // Se imprime el resultado final
    for (auto elemento : frecuencias) {
        cout << elemento.first << ": " << elemento.second << endl;
    }

    return 0;
}
