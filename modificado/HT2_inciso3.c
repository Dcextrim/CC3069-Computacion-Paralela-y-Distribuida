/*----------------------------------------------------------------------
 * Universidad del Valle de Guatemala
 * Curso:     CC3169 - Computacion Paralela y Distribuida
 * Ejercicio: Hoja de Trabajo 02 - Introduccion a Open MPI
 *            Inciso 3
 * Descripcion: simulacion de la comunicacion de un cambio de precio
 *              desde la Oficina Central hacia todas las sucursales.
 *
 *              Cada proceso MPI representa una ubicacion diferente:
 *                  rank 0 -> Oficina central
 *                  rank 1 -> Sucursal 1
 *                  rank 2 -> Sucursal 2
 *                  ...
 *
 *              La Oficina Central define el nuevo precio y lo comunica
 *              a todas las sucursales utilizando MPI_Bcast().
 *
 * Modificacion: la Oficina Central tambien define un porcentaje de
 *              descuento y lo distribuye a todas las sucursales
 *              mediante una llamada independiente a MPI_Bcast().
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {

    int rank;
    int size;
    float precio;
    float descuento;

    // Inicializa el entorno MPI
    MPI_Init(&argc, &argv);

    // Obtener el identificador del proceso actual
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtener el numero total de procesos que participan
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // La Oficina Central define el nuevo precio y el descuento del dia
    if (rank == 0) {
        precio = 25.50;
        descuento = 10.0;

        printf("Oficina Central: nuevo precio = Q%.2f\n", precio);
        printf("Oficina Central: descuento = %.2f %%\n", descuento);
    }

    // La Oficina Central envia el precio a todos los procesos
    MPI_Bcast(&precio, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Llamada independiente para distribuir el descuento a todos los procesos
    MPI_Bcast(&descuento, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Cada sucursal muestra el precio y el descuento recibidos
    if (rank != 0) {
        printf("Sucursal %d: precio recibido = Q%.2f\n", rank, precio);
        printf("Sucursal %d: descuento recibido = %.2f %%\n", rank, descuento);
    }

    // Finaliza correctamente el entorno MPI
    MPI_Finalize();

    return 0;
}