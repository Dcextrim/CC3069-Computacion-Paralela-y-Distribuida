#include "../include/integracion.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <omp.h>

namespace {

[[nodiscard]] integracion::resultado integrar(
    const integracion::configuracion& opciones
) {
    const integracion::indice_t rectangulos{opciones.rectangulos};
    const double limite_inferior{opciones.limite_inferior};
    const double ancho{
        (opciones.limite_superior - limite_inferior)
        / static_cast<double>(rectangulos)
    };
    double suma{};
    int hilos_usados{};

    omp_set_dynamic(0);
    omp_set_num_threads(opciones.hilos);

    const double inicio{omp_get_wtime()};
    #pragma omp parallel default(none) \
        shared(rectangulos, limite_inferior, ancho, hilos_usados) reduction(+:suma)
    {
        #pragma omp single
        hilos_usados = omp_get_num_threads();

        #pragma omp for schedule(static)
        // OpenMP exige la forma canónica con "=" en la inicialización del ciclo.
        for (integracion::indice_t i = 0; i < rectangulos; ++i) {
            const double x{
                limite_inferior + (static_cast<double>(i) + 0.5) * ancho
            };
            suma += integracion::funcion(x);
        }
    }
    const double fin{omp_get_wtime()};

    return {.area = suma * ancho, .segundos = fin - inicio, .hilos_usados = hilos_usados};
}

}  // namespace

int main(const int argc, char* argv[]) {
    try {
        const auto opciones{
            integracion::leer_argumentos(argc, argv, true, omp_get_max_threads())
        };
        const auto medicion{integrar(opciones)};
        integracion::imprimir_resultado("paralelo", opciones, medicion);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
