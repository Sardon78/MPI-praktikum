#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;

// Пример пользовательской функции f(x, y)
double function_xy(double x, double y) {
    return sin(x) * cos(y);
}

// Пример вычисления производной по x (df(x, y)/dx)
double derivative_x(double x, double y) {
    double h = 1e-6; // Шаг для численного дифференцирования
    return (function_xy(x + h, y) - function_xy(x, y)) / h;
}

// Пример вычисления производной по y (df(x, y)/dy)
double derivative_y(double x, double y) {
    double h = 1e-6; // Шаг для численного дифференцирования
    return (function_xy(x, y + h) - function_xy(x, y)) / h;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int grid_size = 10; // Размер сетки
    int iterations = 3; // Количество итераций для каждого размера сетки

    for (int i = 0; i < iterations; ++i) {
        // Создание и заполнение двумерного массива для пользовательской функции f(x, y)
        vector<vector<double> > grid(grid_size, vector<double>(grid_size));
        for (int i = 0; i < grid_size; ++i) {
            for (int j = 0; j < grid_size; ++j) {
                grid[i][j] = function_xy(i, j);
            }
        }

        // Замер времени
        double start_time = MPI_Wtime();

        // Вычисление производной по x (df(x, y)/dx) для каждого элемента массива
        vector<vector<double> > derivative_x_grid(grid_size, vector<double>(grid_size));
        for (int i = 0; i < grid_size; ++i) {
            for (int j = 0; j < grid_size; ++j) {
                derivative_x_grid[i][j] = derivative_x(i, j);
            }
        }

        // Замер времени окончания и вывод результатов
        if (rank == 0) {
            double end_time = MPI_Wtime();
            double duration = end_time - start_time;
            cout << "Grid size: " << grid_size << "x" << grid_size << ", Time taken: " << duration << " seconds" << endl;
        }

        // Увеличение размера сетки для следующей итерации
        grid_size *= 10;
    }

    MPI_Finalize();
    return 0;
}

/*
Grid size: 10x10, Time taken: 1.7e-05 seconds
Grid size: 100x100, Time taken: 0.001001 seconds
Grid size: 1000x1000, Time taken: 0.095903 seconds
*/