#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// Генерация случайной матрицы размером rows x cols
vector<vector<double> > generateMatrix(int rows, int cols) {
    vector<vector<double> > matrix(rows, vector<double>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = rand() % 10; // Заполняем элемент случайным числом от 0 до 9
        }
    }
    return matrix;
}

// Умножение двух матриц
vector<vector<double> > matrixMultiplication(const vector<vector<double> >& A,
                                             const vector<vector<double> >& B) {
    int m = A.size();
    int n = A[0].size();
    int p = B[0].size();
    vector<vector<double> > result(m, vector<double>(p, 0));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < p; ++j) {
            for (int k = 0; k < n; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int matrix_sizes[] = {10, 100, 1000}; // Размеры матриц для тестирования
    int iterations = 3; // Количество итераций для каждого размера матрицы

    for (int size_idx = 0; size_idx < 3; ++size_idx) {
        int matrix_size = matrix_sizes[size_idx];

        // Создание двух матриц A и B
        vector<vector<double> > A(matrix_size, vector<double>(matrix_size));
        vector<vector<double> > B(matrix_size, vector<double>(matrix_size));
        if (rank == 0) {
            A = generateMatrix(matrix_size, matrix_size);
            B = generateMatrix(matrix_size, matrix_size);
        }

        // Распределение матриц A и B между потоками
        MPI_Bcast(B.data(), matrix_size * matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for (int i = 0; i < matrix_size; ++i) {
            MPI_Bcast(A[i].data(), matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        // Замер времени
        double start_time = MPI_Wtime();

        // Умножение матриц
        vector<vector<double> > C = matrixMultiplication(A, B);

        // Замер времени окончания и вывод результатов
        if (rank == 0) {
            double end_time = MPI_Wtime();
            double duration = end_time - start_time;
            cout << "Matrix size: " << matrix_size << "x" << matrix_size << ", Time taken: " << duration << " seconds" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
