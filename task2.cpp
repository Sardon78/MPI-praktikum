#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int array_sizes[] = {10, 1000, 10000000}; // Размеры массивов для тестирования
    int iterations = 3; // Количество итераций для каждого размера массива

    for (int size_idx = 0; size_idx < 3; ++size_idx) {
        int array_size = array_sizes[size_idx];

        // Создание массива
        vector<int> numbers(array_size, 1);

        // Замер времени
        double start_time = MPI_Wtime();

        // Распределение массива между потоками
        vector<int> local_numbers(array_size / size);
        MPI_Scatter(numbers.data(), array_size / size, MPI_INT, local_numbers.data(), array_size / size, MPI_INT, 0, MPI_COMM_WORLD);

        // Вычисление локальной суммы
        int local_sum = 0;
        for (int num : local_numbers) {
            local_sum += num;
        }

        // Сбор всех локальных сумм на потоке 0
        int total_sum;
        MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        // Замер времени окончания и вывод результатов
        if (rank == 0) {
            double end_time = MPI_Wtime();
            double duration = end_time - start_time;
            cout << "Array size: " << array_size << ", Time taken: " << duration << " seconds" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}

/*
Array size: 10, Time taken: 0.001112 seconds
Array size: 1000, Time taken: 3.6e-05 seconds
Array size: 10000000, Time taken: 0.082518 seconds
*/