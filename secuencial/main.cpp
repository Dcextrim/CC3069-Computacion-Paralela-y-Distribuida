#include "../include/integracion.hpp"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>

namespace {

[[nodiscard]] integracion::resultado integrar(
    const integracion::configuracion& opciones
) {
    const double ancho{
        (opciones.limite_superior - opciones.limite_inferior)
        / static_cast<double>(opciones.rectangulos)
    };
    double suma{};

    const auto inicio{std::chrono::steady_clock::now()};
    for (integracion::indice_t i{0}; i < opciones.rectangulos; ++i) {
        const double x{
            opciones.limite_inferior + (static_cast<double>(i) + 0.5) * ancho
        };
        suma += integracion::funcion(x);
    }
    const auto fin{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> duracion{fin - inicio};

    return {.area = suma * ancho, .segundos = duracion.count(), .hilos_usados = 1};
}

}  // namespace

int main(const int argc, char* argv[]) {
    try {
        const auto opciones{integracion::leer_argumentos(argc, argv, false, 1)};
        const auto medicion{integrar(opciones)};
        integracion::imprimir_resultado("secuencial", opciones, medicion);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
