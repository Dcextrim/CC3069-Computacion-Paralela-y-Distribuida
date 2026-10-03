/*----------------------------------------------------------------------
 * Universidad del Valle de Guatemala
 * Curso:     CC3169 - Computacion Paralela y Distribuida
 * Ejercicio: Ejercicio30Septiembre - Introduccion a Open MPI
 * Descripcion: simulacion de la recoleccion de temperaturas
 *              registradas en diferentes sucursales.
 *
 *              Cada proceso MPI representa una ubicacion diferente:
 *                  rank 0 -> Oficina central
 *                  rank 1 -> Sucursal 1
 *                  rank 2 -> Sucursal 2
 *                  rank 3 -> Sucursal 3
 *
 *              MODIFICADO: cada proceso registra DOS mediciones de
 *              temperatura y la Oficina Central recopila los 8 valores
 *              con una sola llamada a MPI_Gather() (sendcount y
 *              recvcount = 2).
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {

    int rank;
    int size;
    float temperatura[2];     // dos mediciones locales por proceso
    float temperaturas[8];    // 4 procesos x 2 mediciones (se llena en rank 0)

    // Inicializa el entorno MPI
    MPI_Init(&argc, &argv);

    // Obtener el identificador del proceso actual
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtener el numero total de procesos que participan
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Este ejercicio requiere exactamente 4 procesos
    if (size != 4) {

        if (rank == 0) {
            printf("Este programa requiere exactamente 4 procesos.\n");
        }

        MPI_Finalize();
        return 0;
    }

    // Cada proceso registra dos temperaturas locales
    if (rank == 0) {
        temperatura[0] = 24.5;
        temperatura[1] = 25.0;
    } else if (rank == 1) {
        temperatura[0] = 26.1;
        temperatura[1] = 26.4;
    } else if (rank == 2) {
        temperatura[0] = 23.8;
        temperatura[1] = 24.1;
    } else {
        temperatura[0] = 27.0;
        temperatura[1] = 27.3;
    }

    printf("Proceso %d: temperaturas registradas = %.1f C, %.1f C\n",
           rank, temperatura[0], temperatura[1]);

    // Reunir las temperaturas de todos los procesos en rank 0.
    // sendcount = 2: cada proceso envia 2 valores.
    // recvcount = 2: el root recibe 2 valores POR CADA proceso
    // (no el total), por lo que recvbuf necesita espacio para 8 floats.
    MPI_Gather(
        temperatura,
        2,
        MPI_FLOAT,
        temperaturas,
        2,
        MPI_FLOAT,
        0,
        MPI_COMM_WORLD
    );

    // La Oficina Central muestra todas las temperaturas recibidas
    if (rank == 0) {

        printf("\nOficina Central: temperaturas recibidas\n");

        // Los datos del proceso i quedan en las posiciones 2*i y 2*i + 1
        for (int i = 0; i < size; i++) {
            printf("Proceso %d: %.1f C, %.1f C\n",
                   i, temperaturas[2 * i], temperaturas[2 * i + 1]);
        }
    }

    // Finaliza correctamente el entorno MPI
    MPI_Finalize();

    return 0;
}

