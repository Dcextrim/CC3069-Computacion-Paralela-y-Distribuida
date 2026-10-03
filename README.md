# CC3069 - Computación Paralela y Distribuida

## Integrantes
- Daniel Chet - 231177
- Dulce Ambrosio - 231143

## Descripción

Este repositorio contiene ejercicios de introducción a Open MPI desarrollados para la clase de Computación Paralela y Distribuida. Los programas muestran el uso de primitivas de comunicación colectiva para sincronizar y compartir datos entre varios procesos.

## Objetivo

- Comprender el modelado de procesos con MPI.
- Ejecutar programas distribuidos localmente con `mpicc` y `mpirun`.
- Aplicar funciones MPI como `MPI_Gather` y `MPI_Reduce` en escenarios de simulación.
- Analizar la salida de cada proceso y validar el comportamiento colectivo del sistema.

## Estructura del repositorio

```text
.
├── README.md
├── original/
│   ├── Ejercicio1a.c
│   └── Ejercicio1b.c
├── modificado/
│   ├── Ejercicio1a.c
│   └── Ejercicio1b.c
├── doc/
│   ├── Ejercicio1a.c
│   ├── Ejercicio1b.c
│   └── Ejercicio 30 Septiembre - Paralela.pdf
└── .gitignore
```

- `original/`: versión base de referencia.
- `modificado/`: versión con cambios realizados para el ejercicio asignado.
- `doc/`: documentación y archivos relacionados con el trabajo.

## Ejercicios incluidos

| Ejercicio | Descripción | Función MPI principal |
|---|---|---|
| 1a | Recolección de temperaturas registradas por cada sucursal. | `MPI_Gather` |
| 1b | Cálculo del consumo total, máximo y mínimo entre distintas sucursales. | `MPI_Reduce` |

## Compilación y ejecución

### Ejemplo general

```bash
mpicc modificado/Ejercicio1a.c -o ejercicio1a
mpirun -np 4 ./ejercicio1a
```

```bash
mpicc modificado/Ejercicio1b.c -o ejercicio1b
mpirun -np 4 ./ejercicio1b
```

> El número de procesos puede variar según la lógica del ejercicio, pero en estos ejemplos se utiliza 4 procesos para representar 4 sucursales/ubicaciones.

## Requisitos

- Open MPI instalado en el sistema.
- Entorno compatible con WSL, Linux o macOS.
- Compilador `gcc`/`mpicc` disponible en el PATH.

## Notas

Los ejercicios fueron desarrollados y probados con ejecución local usando Open MPI, verificando que la salida de cada proceso coincide con la lógica pedida en el enunciado del trabajo.
