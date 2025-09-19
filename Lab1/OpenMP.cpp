#include <math.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include "vector"

using namespace std;

int main(int argc, char* argv[])
{

    omp_set_num_threads(4);
    int nTheads, theadNum;
    double total_time = 0;
    int runs = 12;
    vector <double> time;
    for (int i = 0; i < runs; ++i)
    {
#pragma omp barrier
        double start_time = omp_get_wtime();

#pragma omp parallel private(nTheads, theadNum)
        {
            nTheads = omp_get_num_threads();
            theadNum = omp_get_thread_num();
            printf("OpenMP thread %d from %d threads \n", theadNum, nTheads);
        }

        double end_time = omp_get_wtime();
        double elapsed = end_time - start_time;
        time.push_back(elapsed);
        total_time += elapsed;
    }
    printf("-------------------------------\n");
    for (int i = 0; i < runs; ++i)
    {   
        printf("Run %d time: %f seconds\n", i, time[i]);
    }
    double avg_time = total_time / runs;
    printf("Average time over %d runs: %f seconds\n", runs, avg_time);
    printf("-------------------------------\n");
    return 0;
}
