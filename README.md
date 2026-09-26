# CC3069 - Computación Paralela y Distribuida

Bienvenido al repositorio del curso: Computación Paralela y Distribuida.

## Integrantes

- Sebas Túnchez - 231359
- Daniel Chet - 231177
- Dulce Ambrosio - 231143

En esta iniciativa académica se aprenden las características y competencias necesarias para realizar computación paralela y distribuida de sistemas a mediana y gran escala. Se exploran y aplican herramientas de fuente abierta (Open Source) de los diferentes modelos de computación paralela y distribuida:

- Computación Paralela de memoria compartida (OpenMP)
- Computación Paralela de memoria distribuida (OpenMPI)
- Computación de Sistemas heterogéneos (CUDA, OpenACC).

En este repositorio se encuentran las actividades como Laboratorios, Hojas de Trabajo y Evaluaciones Cortas correspondientes al curso.

## Estructura del Repositorio

Este repositorio se maneja con base en *branches*. La rama `main` es la rama principal y contiene la referencia general del repositorio, mientras que las demás branches organizan las actividades del curso. Cada branch corresponde a una actividad y contiene su propio archivo `README.md` con documentación, respuestas a preguntas de ejercicios, enlaces a videos solicitados y otros recursos.

### Branches

- `main`: rama principal del repositorio
- `Parcial-1`: Evaluación Parcial 1
- `Evaluacion-Corta-2`: Paralelización del Algoritmo de Conteo de Frecuencia de Palabras
- `Hoja-de-Trabajo-1`: Análisis de *Speedup* y Eficiencia en la Paralelización del Conteo de Palabras
- `Hoja-de-Trabajo-2`: Introducción a Open MPI - reconocimiento de procesos, comunicación punto a punto, difusión (`MPI_Bcast`) y distribución de datos (`MPI_Scatter`)

## Hoja de Trabajo 02 - Introducción a Open MPI

### Objetivo

Ejecutar programas Open MPI de forma local e identificar el uso de funciones fundamentales para reconocer procesos, realizar comunicación punto a punto, difundir información y distribuir datos entre varios procesos. El enunciado completo está en [`cc3069-hoja-trabajo-02.md`](cc3069-hoja-trabajo-02.md).

### Estructura de este branch

```
├── original/                    # Código base entregado, sin modificar (referencia)
│   ├── HT2_inciso1.c
│   ├── HT2_inciso2.c
│   ├── HT2_inciso3.c
│   └── HT2_inciso4.c
├── modificado/                  # Código a entregar, con las modificaciones pedidas en cada ejercicio
│   ├── HT2_inciso2.c
│   ├── HT2_inciso3.c
│   └── HT2_inciso4.c
└── cc3069-hoja-trabajo-02.md     # Enunciado de la hoja de trabajo
```

`HT2_inciso1.c` solo existe en `original/` porque ese ejercicio no requiere modificación (únicamente verificar `MPI_COMM_WORLD` y el rank de cada proceso).

### Compilación y ejecución

```bash
mpicc modificado/HT2_inciso1.c -o HT2_inciso1
mpirun -np <procesos> ./HT2_inciso1
```

Se reemplaza `HT2_inciso1` por el inciso correspondiente y `<procesos>` por la cantidad de procesos indicada en cada ejercicio (2, 4 o 6 según el caso).

### Ejercicios

| Ejercicio | Descripción | Función(es) MPI clave | Estado |
|---|---|---|---|
| 1 | Reconocimiento de procesos y ranks | `MPI_Comm_rank`, `MPI_Comm_size` | Pendiente |
| 2 | Comunicación directa oficina central - sucursal | `MPI_Send`, `MPI_Recv` | Pendiente |
| 3 | Difusión de precio y descuento a todas las sucursales | `MPI_Bcast` | Pendiente |
| 4 | Distribución de pedidos y empleados por sucursal | `MPI_Scatter` | Pendiente |

### Preguntas de análisis

Las respuestas a las preguntas de análisis de los cuatro ejercicios se agregarán a este README (o a un documento aparte) conforme se resuelva cada ejercicio.
