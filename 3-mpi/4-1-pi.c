
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define FLIPS 1E8 // 10^9 on dardel

int main(int argc, char *argv[]) {
    int count = 0, rank, comm_size, provided;
    double x, y, z, start_time, end_time;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    srand(time(NULL) + rank * 12345678);

    if (rank == 0)
        start_time = MPI_Wtime();

    for (int i = 0; i < FLIPS/comm_size; i++) {
        x = (double)rand() / (double)RAND_MAX;
        y = (double)rand() / (double)RAND_MAX;
        
        // check if "hypotenuse" is inside unit circle
        if (sqrt((x*x) + (y*y)) <= 1.0)
            count++;
    }

    int all_count = 0;
    MPI_Reduce(&count, &all_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi = 4.0 * ((double)all_count / (double)FLIPS);

        end_time = MPI_Wtime();
        printf("time(%d): %3fs\n", comm_size, end_time - start_time);
        printf("pi ≈ %f\n", pi);
    }

    MPI_Finalize();
}