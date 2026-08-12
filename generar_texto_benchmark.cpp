#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Uso: generar_texto_benchmark <entrada> <salida> <repeticiones>\n";
        return 1;
    }

    ifstream entrada(argv[1], ios::binary);
    if (!entrada) {
        cerr << "No se pudo abrir la entrada.\n";
        return 1;
    }

    ostringstream contenido;
    contenido << entrada.rdbuf();
    string bloque = contenido.str();
    if (bloque.empty()) {
        cerr << "El archivo de entrada esta vacio.\n";
        return 1;
    }
    if (bloque.back() != '\n') {
        bloque.push_back('\n');
    }

    char* fin = nullptr;
    const unsigned long long repeticiones = strtoull(argv[3], &fin, 10);
    if (fin == argv[3] || *fin != '\0' || repeticiones == 0) {
        cerr << "Las repeticiones deben ser un entero positivo.\n";
        return 1;
    }

    ofstream salida(argv[2], ios::binary);
    if (!salida) {
        cerr << "No se pudo crear la salida.\n";
        return 1;
    }

    for (unsigned long long i = 0; i < repeticiones; ++i) {
        salida.write(bloque.data(), static_cast<streamsize>(bloque.size()));
    }

    cout << "Archivo generado con " << repeticiones << " repeticiones.\n";
    return 0;
}
