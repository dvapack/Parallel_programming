#include "mpi.h"
#include "stdio.h"
#include "vector"

using namespace std;

int main(int argc, char* argv[])
{
    int rank, ranksize, i;
    double start_time, end_time;
    double total_time = 0.0;
    int runs = 12;
    vector <double> time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ranksize);
    for (i = 0; i < runs; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        start_time = MPI_Wtime();

        printf("Hello world from process %d of total %d\n", rank, ranksize);

        MPI_Barrier(MPI_COMM_WORLD);
        end_time = MPI_Wtime();

        double elapsed = end_time - start_time;
        if (rank == 0) {
            time.push_back(elapsed);
            total_time += elapsed;
        }
    }
    MPI_Finalize();
    if (rank == 0) {
        printf("-------------------------------\n");
        for (i = 0; i < runs; i++) {
            printf("Run %d time: %f seconds\n", i + 1, time[i]);
        }
        double avg_time = total_time / runs;
        printf("Average time over %d runs: %f seconds\n", runs, avg_time);
        printf("-------------------------------");
    }
    return 0;
}
