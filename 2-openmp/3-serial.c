#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MEASUREMENTS 100
#define ELEMENTS 10000000

double input[ELEMENTS];
void generate_random(double *input, size_t size) {
    for (size_t i = 0; i < size; i++) {
        input[i] = rand() / (double)(RAND_MAX);
    }
}

// task 3.1
double serial_sum(double *x, size_t size) {
    double sum_val = 0.0;

    for (size_t i = 0; i < size; i++) {
        sum_val += x[i];
    }

    return sum_val;
}

// task 3.2
double omp_sum(double *x, size_t size) {
    double sum_val = 0.0;
#pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        sum_val += x[i];
    }

    return sum_val;
}

// task 3.3
// int num_threads[9] = {1, 2, 4, 8, 16, 20, 24, 28, 32}; // task 3.3
double omp_critical_sum(double *x, size_t size) {
    double sum_val = 0.0;
#pragma omp parallel for 
    for (size_t i = 0; i < size; i++) {
        #pragma omp critical
        {
            sum_val += x[i];
        }
    }

    return sum_val;
}

// task 3.4
// int num_threads[] = {1, 32, 64, 128};
double omp_local_sum(double *x, size_t size) {
    double local_sums[omp_get_max_threads()];
    double total_sum = 0.0;
    #pragma omp parallel
    {
        double local_sum = 0.0;
        int id = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < size; i++) {
            local_sum += x[i];
        }
        local_sums[id] = local_sum;
        #pragma omp critical
        {
            total_sum += local_sums[id];
        }
    }
    return total_sum;
}

// task 3.5
typedef struct {
    double sum;
    char pad[128];
} sum_t;
int num_threads[] = {1, 32, 64, 128};
double opt_local_sum(double *x, size_t size) {
    sum_t local_sums[omp_get_max_threads()];
    double total_sum = 0.0;
    #pragma omp parallel
    {
        int num_threads = omp_get_num_threads();
        double local_sum = 0.0;
        int id = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < size; i++) {
            local_sum += x[i];
        }
        local_sums[id].sum = local_sum;
        #pragma omp critical
        {
            total_sum += local_sums[id].sum;
        }
    }
    return total_sum;
}


int main() {
    generate_random(input, ELEMENTS);
    
    int len = sizeof(num_threads) / sizeof(int);
    for (int t_idx = 0; t_idx < len; t_idx++) {
        double exec_time[MEASUREMENTS];
        double total_time = 0.0;
        omp_set_num_threads(num_threads[t_idx]);
        
        double actual_sum = serial_sum(input, ELEMENTS);
        double sum;
        for (int i = 0; i < MEASUREMENTS; i++) {
            double start = omp_get_wtime();
            // sum = serial_sum(input, ELEMENTS);
            // sum = omp_sum(input, ELEMENTS);
            // sum = omp_critical_sum(input, ELEMENTS);
            sum = omp_local_sum(input, ELEMENTS);
            // sum = opt_local_sum(input, ELEMENTS);
            double stop = omp_get_wtime();
            exec_time[i] = stop - start;
            total_time += exec_time[i];
            if (fabs((actual_sum - sum)) > 0.000001) {
                printf("Wrong values on distributed sum: %f vs %f\n", actual_sum, sum);
            }
        }

        double avg = total_time / (double)MEASUREMENTS;
        double stddev = 0.0;
        for (int i = 0; i < MEASUREMENTS; i++) {
            stddev += pow(fabs(exec_time[i] - avg), 2);
        }
        stddev = sqrt(stddev / (double)MEASUREMENTS);

        printf("For %d threads\n", num_threads[t_idx]);
        
        printf("Avg: %f\n", avg);
        printf("Std dev: %f\n", stddev);
    }

    return 0;
}