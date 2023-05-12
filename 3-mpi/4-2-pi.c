
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

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

    if (rank == 0) {
        int all_count = count; // instead of sending to itself
        for (int i = 1; i < comm_size; i++) {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Received count %d from %d\n", temp, i);
            all_count += temp;
        }

        double pi = 4.0 * ((double)all_count / (double)FLIPS);
        
        end_time = MPI_Wtime();
        printf("time(%d): %3fs\n", comm_size, end_time - start_time);
        printf("pi ≈ %f\n", pi);
    } else {
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}