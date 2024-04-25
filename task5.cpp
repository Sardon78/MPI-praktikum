#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// Функция для вывода матрицы
void printMatrix(const vector<vector<double> >& matrix) {
    for (const auto& row : matrix) {
        for (double element : row) {
            cout << element << " ";
        }
        cout << endl;
    }
}

// Функция для решения системы линейных уравнений методом Гаусса
vector<double> gaussElimination(const vector<vector<double> >& A, const vector<double>& b) {
    int n = b.size();
    vector<vector<double> > AB(n, vector<double>(n + 1));

    // Слияние матрицы A и вектора b
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            AB[i][j] = A[i][j];
        }
        AB[i][n] = b[i];
    }

    // Прямой ход метода Гаусса
    for (int i = 0; i < n - 1; ++i) {
        for (int k = i + 1; k < n; ++k) {
            double factor = AB[k][i] / AB[i][i];
            for (int j = i; j < n + 1; ++j) {
                AB[k][j] -= factor * AB[i][j];
            }
        }
    }

    // Обратный ход метода Гаусса
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = AB[i][n] / AB[i][i];
        for (int k = i - 1; k >= 0; --k) {
            AB[k][n] -= AB[k][i] * x[i];
        }
    }

    return x;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int equation_sizes[] = {10, 100, 1000}; // Размеры системы уравнений для тестирования
    int iterations = 3; // Количество итераций для каждого размера

    for (int size_idx = 0; size_idx < 3; ++size_idx) {
        int equation_size = equation_sizes[size_idx];

        // Создание матрицы A и вектора b
        vector<vector<double> > A(equation_size, vector<double>(equation_size, 0));
        vector<double> b(equation_size, 1);
        if (rank == 0) {
            for (int i = 0; i < equation_size; ++i) {
                A[i][i] = 2; // Заполняем диагональные элементы матрицы A
            }
        }

        // Распределение матрицы A и вектора b между потоками
        MPI_Bcast(b.data(), equation_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for (int i = 0; i < equation_size; ++i) {
            MPI_Bcast(A[i].data(), equation_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        // Замер времени
        double start_time = MPI_Wtime();

        // Решение системы уравнений методом Гаусса
        vector<double> x = gaussElimination(A, b);

        // Замер времени окончания и вывод результатов
        if (rank == 0) {
            double end_time = MPI_Wtime();
            double duration = end_time - start_time;
            cout << "Equation size: " << equation_size << ", Time taken: " << duration << " seconds" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}

/*
Equation size: 10, Time taken: 2.9e-05 seconds
Equation size: 100, Time taken: 0.0066 seconds
Equation size: 1000, Time taken: 4.07133 seconds
*/