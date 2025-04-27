#include <iostream>
#include <conio.h>
#include <omp.h>

#define N 10 // больше 10 нельзя а то не будет положительно определённой

void gauss(double m[N][N + 1])
{
	int i, j, k;
	for (i = 0; i < N; i++) // идём вперёд
	{
		int maxRow = i;
		for (k = i + 1; k < N; k++) // ищем максимум в столбце i 
		{
			if (std::abs(m[k][i]) > std::abs(m[maxRow][i]))
			{
				maxRow = k;
			}
		}

		for (k = i; k <= N; k++) // приводим к нормализованному виду
		{
			double t = m[maxRow][k];
			m[maxRow][k] = m[i][k];
			m[i][k] = t;
		}
		
		#pragma omp parallel // делаем пул потоков дальше отключим барьер ожидания
		{
			#pragma omp for nowait // максимальная оптимизация приоритет скорости (/02)
			for (int k = i + 1; k < N; k++) // приводим к верхнему треугольному виду
			{
				double f = m[k][i] / m[i][i];

				#pragma omp simd // пусть компилятор строит длинные векторные инструкции (/arch:AVX2)
				for (int j = i; j <= N; j++)
				{
					m[k][j] -= f * m[i][j];
				}
			}
		}
	}

	double x[N];
	for (i = N - 1; i >= 0; i--) // идём назад
	{
		x[i] = m[i][N];
		for (j = i + 1; j < N; j++)
		{
			x[i] -= m[i][j] * x[j];
		}
		x[i] /= m[i][i];
	}

	/*
	printf("\n");
	for (i = 0; i < N; i++)
	{
		printf("x[%d] = %lf\n", i+1, x[i]);
	}
	*/
}

int main()
{
	double matrix[N][N + 1];

	//srand(time(NULL));

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N + 1; j++)
		{
			matrix[i][j] = (double)(rand() % 21 - 10);
			printf("%7.2lf ", matrix[i][j]);
		}
		printf("\n");
	}

	int num_threads;
	std::cout << "Enter number of threads (recommended = " << omp_get_max_threads() << "): ";
	std::cin >> num_threads;

	omp_set_num_threads(num_threads);

	double start_time = omp_get_wtime();
	gauss(matrix);
	double end_time = omp_get_wtime();

	printf("\nTime elapsed: %lf seconds\n", end_time - start_time);

	return 0;
}
//g++ -O3 -fopenmp -march=native lab3.cpp -o lab3