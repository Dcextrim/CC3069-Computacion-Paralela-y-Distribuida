/*----------------------------------------------------------------------
 * Universidad del Valle de Guatemala
 * Curso:     CC3169 - Computacion Paralela y Distribuida
 * Ejercicio: Hoja de Trabajo 02 - Introduccion a Open MPI
 *            Inciso 4
 * Descripcion: simulacion de la distribucion de pedidos desde la
 *              Oficina Central hacia las sucursales.
 *
 *              Cada proceso MPI representa una ubicacion diferente:
 *                  rank 0 -> Oficina central
 *                  rank 1 -> Sucursal 1
 *                  rank 2 -> Sucursal 2
 *                  rank 3 -> Sucursal 3
 *
 *              La Oficina Central posee una lista de pedidos y
 *              distribuye una parte a cada proceso utilizando
 *              MPI_Scatter().
 *
 * Modificacion: la Oficina Central distribuye dos datos por ubicacion
 *              (cantidad de pedidos y cantidad de empleados disponibles)
 *              en lugar de uno solo. El arreglo de origen se organiza
 *              de forma intercalada [pedidos_0, empleados_0, pedidos_1,
 *              empleados_1, ...] y sendcount/recvcount se ajustan a 2
 *              para que cada proceso reciba ambos valores en un solo
 *              bloque contiguo.
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <mpi.h>

#define DATOS_POR_PROCESO 2

int main(int argc, char *argv[]) {

    int rank;
    int size;
    int datos[4 * DATOS_POR_PROCESO];
    int datos_recibidos[DATOS_POR_PROCESO];

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

    // La Oficina Central define, para cada ubicacion, la cantidad de
    // pedidos y la cantidad de empleados disponibles para procesarlos
    if (rank == 0) {

        // datos[i*2]    pedidos de la ubicacion i
        // datos[i*2+1]  empleados disponibles en la ubicacion i
        datos[0] = 120; datos[1] = 5;  // Oficina Central
        datos[2] = 95;  datos[3] = 4;  // Sucursal 1
        datos[4] = 140; datos[5] = 6;  // Sucursal 2
        datos[6] = 110; datos[7] = 3;  // Sucursal 3

        printf("Oficina Central: distribuyendo pedidos y empleados...\n");
    }

    // Distribuir dos valores contiguos del arreglo a cada proceso
    MPI_Scatter(
        datos,
        DATOS_POR_PROCESO,
        MPI_INT,
        datos_recibidos,
        DATOS_POR_PROCESO,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    // Cada proceso muestra los valores que recibio
    if (rank == 0) {
        printf("Oficina Central: %d pedidos asignados, %d empleados disponibles.\n",
               datos_recibidos[0], datos_recibidos[1]);
    } else {
        printf("Sucursal %d: %d pedidos asignados, %d empleados disponibles.\n",
               rank, datos_recibidos[0], datos_recibidos[1]);
    }

    // Finaliza correctamente el entorno MPI
    MPI_Finalize();

    return 0;
}