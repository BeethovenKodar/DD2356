
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define FLIPS 100000000

int main(int argc, char *argv[]) {
    int count = 0, rank, comm_size, provided;
    double x, y, z;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    printf("I'm %d\n", rank);

    double seed = time(NULL) + 12345678 + rank * 10000
    srand(seed);
    printf("Proc %d seed: %d\n", rank, seed);

    for (int i = 0; i < FLIPS/comm_size; i++) {
        // Generate random (X,Y) points
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        // Check if point is in unit circle
        if (z <= 1.0)
            count++;
    }

    printf("Proc %d counted %d\n", rank, count);

    int all_count = 0;
    MPI_Reduce(&count,
            &all_count,
            1,
            MPI_DOUBLE,
            MPI_SUM,
            0,
            MPI_COMM_WORLD);

    // wait for all processes to finish
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        if (all_count < 0) {
            printf("Overflow?\n");
            MPI_Finalize();
            return 1;
        }

        // Estimate Pi and display the result
        double pi = comm_size * 4.0 * ((double)all_count / (double)FLIPS);
        printf("Pi is approximated to: %f\n%d processes\n", pi, comm_size);
    }

    MPI_Finalize();
}