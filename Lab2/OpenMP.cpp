#include <iostream>
#include <omp.h>
#include <stdlib.h>


int main() {
    int num_arrays = 2;
    int array_size = 90000;
    int Q = 21;

    // 1) Вывод ВСЕХ параметров индивидуального варианта.
    std::cout << "type - int" << '\n';
    std::cout << "vectors - " << num_arrays << '\n';
    std::cout << "elements - " << array_size << '\n';
    std::cout << "Q - " << Q << '\n';
    std::cout << "threads - " << 10 << '\n';
    std::cout << "starts - " << 20 << '\n';
    omp_set_num_threads(10);

    // 2) Создание массивов.
    int* arr_1 = new int[array_size];
    int* arr_2 = new int[array_size];

    //заполнение массива
    for (int i = 0; i < array_size; ++i)
    {
        arr_1[i] = 1;
        arr_2[i] = 1;
    }

    int sequential_sum = 0, reduction_sum = 0, critical_sum = 0, atomic_sum = 0;
    double sequential_time = 0, reduction_time = 0, critical_time = 0, atomic_time = 0, paral_time = 0;
    // 6) Цикл для выполнения пунктов 3-5 не менее 20 раз для получения средних значений всех указанных величин времени
    for (int counter = 0; counter < 20; counter++) 
    {
        sequential_sum = 0;
        reduction_sum = 0;
        critical_sum = 0;
        atomic_sum = 0;

        // 3) Последовательный алгоритм и замер его времени выполнения
        double time_sequantial_start = omp_get_wtime();
        for (int i = 0; i < array_size; ++i)
        {
            for (int q = 0; q < Q; ++q)
            {
                sequential_sum += arr_1[i] + arr_2[i];
            }
        }
        double time_sequantial_stop = omp_get_wtime();
        sequential_time += time_sequantial_stop - time_sequantial_start;


        // 4) Параллельную область и замер времени ее инициализации
        paral_time = 0;
        double start_paral_time = omp_get_wtime();
        #pragma omp parallel shared(arr_1, arr_2)
        {
            #pragma omp barrier
            double end_paral_time = omp_get_wtime();
            paral_time += end_paral_time - start_paral_time;

            #pragma omp barrier
            // 5) Три реализации параллельных алгоритмов поиска суммы и замер времени их выполнения
            double start_reduction_time = omp_get_wtime();
            #pragma omp for reduction(+: reduction_sum)
            for (int i = 0; i < array_size; ++i)
            {
                for (int q = 0; q < Q; ++q)
                {
                    reduction_sum += arr_1[i] + arr_2[i];
                }
            }

            #pragma omp barrier
            double end_reduction_time = omp_get_wtime();
            reduction_time += end_reduction_time - start_reduction_time;

            #pragma omp barrier
            double start_critical_time = omp_get_wtime();
            #pragma omp for
            for (int i = 0; i < array_size; ++i) 
            {
                for (int q = 0; q < Q; ++q) 
                {
                    #pragma omp critical
                    critical_sum += arr_1[i] + arr_2[i];
                }
            }
            #pragma omp barrier
            double end_critical_time = omp_get_wtime();
            critical_time += end_critical_time - start_critical_time;

            #pragma omp barrier
            double start_atomic_time = omp_get_wtime();
            #pragma omp for
            for (int i = 0; i < array_size; ++i) 
            {
                for (int q = 0; q < Q; ++q) 
                {
                    #pragma omp atomic
                    atomic_sum += arr_1[i] + arr_2[i];
                }
            }
            #pragma omp barrier
            double end_atomic_time = omp_get_wtime();
            atomic_time += end_atomic_time - start_atomic_time;
        }
    }

    double mean_sequential = sequential_time / 20;
    double mean_atomic = atomic_time / 20;
    double mean_critical = critical_time / 20;
    double mean_reduction = reduction_time / 20;
    double mean_paral = paral_time / 20;
    // 8) Вывод полученных значений времени и ускорения.
    std::cout << "seq sum - " << sequential_sum / Q << '\n';
    std::cout << "atomic sum - " << atomic_sum / Q << '\n';
    std::cout << "critical sum - " << critical_sum / Q << '\n';
    std::cout << "reduction sum - " << reduction_sum / Q << '\n';
    std::cout << "parallel time - " << mean_paral << " seconds" << '\n';
    std::cout << "mean seq time - " << mean_sequential << " seconds" << '\n';
    std::cout << "mean atomic time - " << mean_atomic << " seconds" << '\n';
    std::cout << "mean critical time - " << mean_critical << " seconds" << '\n';
    std::cout << "mean reduction time - " << mean_reduction << " seconds" << '\n';
    std::cout << "acceleration atomic paral - " << mean_sequential / (mean_atomic + mean_paral) << '\n';
    std::cout << "acceleration atomic - " << mean_sequential / (mean_atomic) << '\n';
    std::cout << "acceleration critical paral - " << mean_sequential / (mean_critical + mean_paral) << '\n';
    std::cout << "acceleration critical - " << mean_sequential / (mean_critical) << '\n';
    std::cout << "acceleration reduction paral - " << mean_sequential / (mean_reduction + mean_paral) << '\n';
    std::cout << "acceleration reduction - " << mean_sequential / (mean_reduction) << '\n';

    delete[] arr_1;
    delete[] arr_2;

    return 0;
}