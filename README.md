# Hoja de Trabajo 02 - Introducción a Open MPI

## Integrantes
- Daniel Chet - 231177
- Dulce Ambrosio - 231143

## Objetivo

Ejecutar programas Open MPI de forma local e identificar el uso de funciones fundamentales para reconocer procesos, realizar comunicación punto a punto, difundir información y distribuir datos entre varios procesos.

## Estructura de este branch

```
├── original/                    # Código base entregado, sin modificar (referencia)
│   ├── HT2_inciso1.c
│   ├── HT2_inciso2.c
│   ├── HT2_inciso3.c
│   └── HT2_inciso4.c
└── modificado/                  # Código con las modificaciones pedidas
    ├── HT2_inciso2.c
    ├── HT2_inciso3.c
    └── HT2_inciso4.c
```

`HT2_inciso1.c` solo existe en `original/` porque ese ejercicio no requiere modificación (únicamente verificar `MPI_COMM_WORLD` y el rank de cada proceso).

## Compilación y ejecución

```bash
mpicc original/HT2_inciso1.c -o HT2_inciso1
mpirun -np <procesos> ./HT2_inciso1
```

Para los incisos 2, 3 y 4 se compila el archivo correspondiente dentro de `modificado/`, por ejemplo:

```bash
mpicc modificado/HT2_inciso2.c -o HT2_inciso2
mpirun -np <procesos> ./HT2_inciso2
```

Se reemplaza `HT2_incisoN` por el ejercicio correspondiente y `<procesos>` por la cantidad de procesos indicada en cada ejercicio (2, 4 o 6 según el caso).

## Ejercicios

| Ejercicio | Descripción | Función(es) MPI clave | Estado |
|---|---|---|---|
| 1 | Reconocimiento de procesos y ranks | `MPI_Comm_rank`, `MPI_Comm_size` | Código completo (sin modificar, según enunciado) |
| 2 | Comunicación directa oficina central - sucursal | `MPI_Send`, `MPI_Recv` | Código completo (ventas + pedidos con tags 100/200) |
| 3 | Difusión de precio y descuento a todas las sucursales | `MPI_Bcast` | Código completo (precio + descuento en llamadas independientes) |
| 4 | Distribución de pedidos y empleados por sucursal | `MPI_Scatter` | Código completo (sendcount/recvcount = 2, datos intercalados) |

Los cuatro programas fueron compilados y ejecutados (vía WSL/Open MPI) con la cantidad de procesos indicada en cada ejercicio, confirmando que la salida coincide con lo solicitado en el enunciado.
