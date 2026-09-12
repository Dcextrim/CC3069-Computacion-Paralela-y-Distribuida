#ifndef INTEGRALMENTE_PARALELOS_INTEGRACION_HPP
#define INTEGRALMENTE_PARALELOS_INTEGRACION_HPP

#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace integracion {

using indice_t = std::int64_t;

constexpr indice_t rectangulos_predeterminados{1'000'000'000};
constexpr double limite_inferior_predeterminado{0.0};
constexpr double limite_superior_predeterminado{10.0};

struct configuracion {
    indice_t rectangulos{rectangulos_predeterminados};
    double limite_inferior{limite_inferior_predeterminado};
    double limite_superior{limite_superior_predeterminado};
    int hilos{1};
    bool salida_csv{false};
};

struct resultado {
    double area{};
    double segundos{};
    int hilos_usados{1};
};

// La función es positiva en el dominio permitido y combina varias operaciones
// trascendentales para representar una carga de trabajo numérica no trivial.
[[nodiscard]] inline double funcion(const double x) noexcept {
    const double seno{std::sin(x)};
    const double coseno{std::cos(2.0 * x)};
    const double componente_oscilatoria{seno * seno + coseno * coseno};
    const double componente_suave{std::sqrt(x + 1.0) + std::log1p(x)};

    return std::exp(-x / 10.0) * (componente_oscilatoria + componente_suave)
        + 1.0 / (1.0 + x * x);
}

[[nodiscard]] inline indice_t convertir_entero(
    const std::string_view texto,
    const std::string_view nombre
) {
    indice_t valor{};
    const char* const inicio{texto.data()};
    const char* const fin{texto.data() + texto.size()};
    const auto [posicion, error]{std::from_chars(inicio, fin, valor)};

    if (error != std::errc{} || posicion != fin) {
        throw std::invalid_argument{
            "Valor inválido para " + std::string{nombre} + ": " + std::string{texto}
        };
    }
    return valor;
}

[[nodiscard]] inline int convertir_hilos(
    const std::string_view texto,
    const std::string_view nombre
) {
    const indice_t valor{convertir_entero(texto, nombre)};
    if (valor > std::numeric_limits<int>::max()) {
        throw std::invalid_argument{"La cantidad de hilos es demasiado grande"};
    }
    return static_cast<int>(valor);
}

[[nodiscard]] inline double convertir_real(
    const std::string_view texto,
    const std::string_view nombre
) {
    double valor{};
    const char* const inicio{texto.data()};
    const char* const fin{texto.data() + texto.size()};
    const auto [posicion, error]{
        std::from_chars(inicio, fin, valor, std::chars_format::general)
    };

    if (error != std::errc{} || posicion != fin || !std::isfinite(valor)) {
        throw std::invalid_argument{
            "Valor inválido para " + std::string{nombre} + ": " + std::string{texto}
        };
    }
    return valor;
}

inline void mostrar_ayuda(const std::string_view programa, const bool permite_hilos) {
    std::cout
        << "Uso: " << programa << " [opciones]\n\n"
        << "Opciones:\n"
        << "  --rectangles N  Cantidad de rectángulos (predeterminado: 1000000000)\n"
        << "  --a VALOR       Límite inferior, debe ser >= 0 (predeterminado: 0)\n"
        << "  --b VALOR       Límite superior (predeterminado: 10)\n";
    if (permite_hilos) {
        std::cout << "  --threads N     Cantidad de hilos OpenMP (predeterminado: máximo disponible)\n";
    }
    std::cout
        << "  --csv            Imprime una fila CSV fácil de procesar\n"
        << "  --help           Muestra esta ayuda\n";
}

[[nodiscard]] inline configuracion leer_argumentos(
    const int argc,
    char* argv[],
    const bool permite_hilos,
    const int hilos_predeterminados
) {
    configuracion opciones{};
    opciones.hilos = permite_hilos ? hilos_predeterminados : 1;

    for (int i{1}; i < argc; ++i) {
        const std::string_view argumento{argv[i]};
        if (argumento == "--help") {
            mostrar_ayuda(argv[0], permite_hilos);
            std::exit(EXIT_SUCCESS);
        }
        if (argumento == "--csv") {
            opciones.salida_csv = true;
            continue;
        }

        if (i + 1 >= argc) {
            throw std::invalid_argument{"Falta el valor de " + std::string{argumento}};
        }
        const std::string_view valor{argv[++i]};

        if (argumento == "--rectangles") {
            opciones.rectangulos = convertir_entero(valor, argumento);
        } else if (argumento == "--a") {
            opciones.limite_inferior = convertir_real(valor, argumento);
        } else if (argumento == "--b") {
            opciones.limite_superior = convertir_real(valor, argumento);
        } else if (argumento == "--threads" && permite_hilos) {
            opciones.hilos = convertir_hilos(valor, argumento);
        } else {
            throw std::invalid_argument{"Opción desconocida: " + std::string{argumento}};
        }
    }

    if (opciones.rectangulos <= 0) {
        throw std::invalid_argument{"La cantidad de rectángulos debe ser positiva"};
    }
    if (opciones.limite_inferior < 0.0) {
        throw std::invalid_argument{"El límite inferior debe ser mayor o igual a cero"};
    }
    if (opciones.limite_superior <= opciones.limite_inferior) {
        throw std::invalid_argument{"El límite superior debe ser mayor que el inferior"};
    }
    if (opciones.hilos <= 0) {
        throw std::invalid_argument{"La cantidad de hilos debe ser positiva"};
    }

    return opciones;
}

inline void imprimir_resultado(
    const std::string_view modo,
    const configuracion& opciones,
    const resultado& medicion
) {
    std::cout << std::setprecision(17);
    if (opciones.salida_csv) {
        std::cout
            << "modo,rectangulos,a,b,hilos,area,segundos\n"
            << modo << ','
            << opciones.rectangulos << ','
            << opciones.limite_inferior << ','
            << opciones.limite_superior << ','
            << medicion.hilos_usados << ','
            << medicion.area << ','
            << medicion.segundos << '\n';
        return;
    }

    std::cout
        << "Modo: " << modo << '\n'
        << "Rectángulos: " << opciones.rectangulos << '\n'
        << "Intervalo: [" << opciones.limite_inferior << ", "
        << opciones.limite_superior << "]\n"
        << "Hilos: " << medicion.hilos_usados << '\n'
        << "Área aproximada: " << medicion.area << '\n'
        << "Tiempo (s): " << medicion.segundos << '\n';
}

}  // namespace integracion

#endif  // INTEGRALMENTE_PARALELOS_INTEGRACION_HPP
