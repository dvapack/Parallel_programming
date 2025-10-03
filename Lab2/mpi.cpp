#include "mpi.h"
#include "stdio.h"
#include <iostream>



int main(int argc, char* argv[])
{
    int num_arrays = 2;
    int array_size = 9000000;
    int Q = 21;
    int ProcRank, ProcNum;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    MPI_Status status;

    int* arr_1 = new int[array_size];
    int* arr_2 = new int[array_size];

    if (ProcRank == 0)
    {
        // 1) Вывод ВСЕХ параметров индивидуального варианта.
        std::cout << "type - int" << '\n';
        std::cout << "vectors - " << num_arrays << '\n';
        std::cout << "elements - " << array_size << '\n';
        std::cout << "Q - " << Q << '\n';
        std::cout << "procnum - " << ProcNum << '\n';
        std::cout << "starts - " << 20 << '\n';
        // 2) Создание массивов.
        for (int i = 0; i < array_size; ++i)
        {
            arr_1[i] = 1;
            arr_2[i] = 1;
        }
    }

    int k = array_size / ProcNum;
    int i1 = k * ProcRank;
    int i2 = k * (ProcRank + 1);

    if (ProcRank == ProcNum - 1) i2 = array_size;


    int sequential_sum = 0, partial_sum = 0, partial_sum_reduce = 0, pointpoint_sum = 0, collective_sum = 0;
    double time_sequantial = 0, time_pointpoint = 0, time_reduce = 0, time_bcast = 0;

    // 6) Цикл для выполнения пунктов 3-5 не менее 20 раз для получения средних значений всех указанных величин времени
    for (int counter = 0; counter < 20; counter++)
    {
        sequential_sum = 0;
        pointpoint_sum = 0;
        collective_sum = 0;
        partial_sum = 0;
        partial_sum_reduce = 0;
        MPI_Barrier(MPI_COMM_WORLD);

        // 4) Широковещательную рассылку и замер времени ее выполнения.
        double time_start = MPI_Wtime();
        MPI_Bcast(arr_1, array_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(arr_2, array_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        double time_bcast_stop = MPI_Wtime();
        time_bcast += time_bcast_stop - time_start;


        // 3) Последовательный алгоритм и замер его времени выполнения
        double time_sequantial_start = MPI_Wtime();
        if (ProcRank == 0)
        {
            for (int q = 0; q < Q; ++q)
            {
                for (int i = 0; i < array_size; ++i)
                {
                    sequential_sum += arr_1[i] + arr_2[i];
                }
            }
        }
        double time_sequantial_stop = MPI_Wtime();
        time_sequantial += time_sequantial_stop - time_sequantial_start;

        // 5) Две реализации параллельных алгоритмов поиска суммы и замер времени их выполнения
        MPI_Barrier(MPI_COMM_WORLD);
        double time_pointpoint_start = MPI_Wtime();
        for (int q = 0; q < Q; ++q)
        {
            for (int i = i1; i < i2; ++i)
            {
                partial_sum += arr_1[i] + arr_2[i];
            }
        }
        // point-to-point
        if (ProcRank == 0)
        {
            int process_sum = partial_sum;
            pointpoint_sum = process_sum;
            for (int i = 1; i < ProcNum; i++)
            {
                MPI_Recv(&process_sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                pointpoint_sum += process_sum;
            }
        }
        else
        {
            MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        double time_pointpoint_stop = MPI_Wtime();
        time_pointpoint += time_pointpoint_stop - time_pointpoint_start;

        // reduce
        double time_collective_start = MPI_Wtime();
        for (int q = 0; q < Q; ++q)
        {
            for (int i = i1; i < i2; ++i)
            {
                partial_sum_reduce += arr_1[i] + arr_2[i];
            }
        }
        MPI_Reduce(&partial_sum_reduce, &collective_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        double time_collective_stop = MPI_Wtime();
        time_reduce += time_collective_stop - time_collective_start;
    }


    if (ProcRank == 0)
    {
        double mean_sequential = time_sequantial / 20;
        double mean_pointpoint = time_pointpoint / 20;
        double mean_reduce = time_reduce / 20;
        // 8) Вывод полученных значений времени и ускорения.
        std::cout << "seq sum - " << sequential_sum / Q << '\n';
        std::cout << "point-to-point sum - " << pointpoint_sum / Q << '\n';
        std::cout << "reduce sum - " << collective_sum / Q << '\n';
        std::cout << "bcast time - " << time_bcast << " seconds" << '\n';
        std::cout << "mean seq time - " << mean_sequential << " seconds" << '\n';
        std::cout << "mean point-to-point time - " << mean_pointpoint << " seconds" << '\n';
        std::cout << "mean reduce time - " << mean_reduce << " seconds" << '\n';
        std::cout << "acceleration point-to-point bcast - " << mean_sequential / (mean_pointpoint + time_bcast) << '\n';
        std::cout << "acceleration point-to-point - " << mean_sequential / (mean_pointpoint) << '\n';
        std::cout << "acceleration reduce bcast - " << mean_sequential / (mean_pointpoint + time_bcast) << '\n';
        std::cout << "acceleration reduce - " << mean_sequential / (mean_pointpoint) << '\n';
    }
    MPI_Finalize();

    delete[] arr_1;
    delete[] arr_2;

    return 0;
}
