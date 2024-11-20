#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    
    // Inicializar el entorno MPI
    MPI_Init(&argc, &argv);
    
    // Obtener el rango (ID) del proceso y el tamaño total de los procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Imprimir un mensaje desde cada nodo
    printf("Hola desde el proceso %d de %d\n", rank, size);
    
    // Finalizar el entorno MPI
    MPI_Finalize();
    
    return 0;
}
