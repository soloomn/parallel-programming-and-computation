#include <iostream>
#include <math.h>
#include <mpi.h>
#include <cstdlib>
#include <time.h>
using namespace std;

static int compare(const void *x1, const void *x2) // вложенная функция для прямой сортировки
{
	return( *(int*)x1 - *(int*)x2);
}

static int compare_desc(const void* x1, const void* x2) // вложенная функция для обратной сортировки
{
	return (*(int*)x2 - *(int*)x1);
}

void set(int *m, int n) // функция заполнения массива
{
	for (int i = 0; i < n; i++)
	{
		m[i] = rand() % 100;
	}
}

void out(int *m, int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << m[i] << endl;
	}
	cout << endl;
}

void sortv(int *m, int n)
{
	qsort(m, n, sizeof(int), compare);
}

void sortu(int *m, int n)
{
	qsort(m, n, sizeof(int), compare_desc);
}

void sendb(int *m, int n, int p, MPI_Comm c)
{
	MPI_Bcast(m, n, MPI_INT, p, c);
}

int mainlab6(int argc, char* argcv[]) //указатель на командную строчку, и количество параметров в коммандной строке
{
	int n; // число элементов в массиве
	int proc, rank, *m;  // служебные объекты и указатель для прединициализации массива
	const int g1_size = 3, g2_size = 4; // статически определим размеры процессорных групп

	double start_time, end_time; // служебные переменные для подсчёта времени
	
	MPI_Group g, g1, g2;
	MPI_Comm c1 = MPI_COMM_NULL, c2 = MPI_COMM_NULL;
	
	int r1[g1_size] = { 2, 3, 4 };
	int r2[g2_size] = { 6, 7, 8, 9 };

	MPI_Init(&argc, &argcv); //используется в обменных операциях и объединяет все процессы (нужно чтобы узнать кол-во процессов и номер)
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	srand(time(NULL) + rank);
	MPI_Comm_group(MPI_COMM_WORLD, &g);
	MPI_Group_incl(g, 3, r1, &g1); // 1. дескриптор базовой группы 2. количество процессов 3. номера процессов 4. дескриптор выходной группы 
	MPI_Group_incl(g, 4, r2, &g2); // 1. дескриптор базовой группы 2. количество процессов 3. номера процессов 4. дескриптор выходной группы 
	MPI_Comm_create(MPI_COMM_WORLD, g1, &c1); // 1. базовый коммуникатор 2. идентификатор группы 3. полученный коммуникатор
	MPI_Comm_create(MPI_COMM_WORLD, g2, &c2); // 1. базовый коммуникатор 2. идентификатор группы 3. полученный коммуникатор

	if (c1 != MPI_COMM_NULL)
	{
		n = 15;
		m = new int[n];
		MPI_Comm_rank(c1, &proc);
		if (proc == 0)
		{
			set(m, n);
			out(m, n);
		}
		sendb(m, n, 0, c1);
		if (proc == 2)
		{
			sortv(m, n);
			out(m, n);
		}
	}
	if (c2 != MPI_COMM_NULL)
	{
		const double des_sleep_time = 0.7; // предполагаемое время сна - то за сколько другой группе на обратиться к функциям и сделать вывод на экран
		start_time = MPI_Wtime(); // начинаем считать время для действий аналогичных соседней группе
		
		n = 10;
		m = new int[n];
		MPI_Comm_rank(c2, &proc);

		end_time = MPI_Wtime(); // закончили считать время
		
		double el_time = end_time - start_time; // посчитали время обращения к памяти и создания массива

		MPI_Barrier(c2); // синхронизируем все процессы перед ожиданием
		while(MPI_Wtime() - start_time < des_sleep_time + el_time)
		{ 
			// ждём
		}
		// подождали и едем дальше:
		if (proc == 0)
		{
			cout << "----------------------" << endl;
			set(m, n);
			out(m, n);
		}
		sendb(m, n, 0, c2);
		if (proc == 3)
		{
			sortu(m, n);
			out(m, n);
		}
	}

	if (MPI_Group_free(&g) != MPI_SUCCESS) // освобождаем выделенные группам ресурсы, делаем вручную потому что MPI_Finalize() сделает не всё
	{
		cerr << "Error freeing group g" << endl;
	}
	if (MPI_Group_free(&g1) != MPI_SUCCESS) 
	{
		cerr << "Error freeing group g1" << endl;
	}
	if (MPI_Group_free(&g2) != MPI_SUCCESS)
	{
		cerr << "Error freeing group g2" << endl;
	}

	MPI_Finalize(); //освобождает память из под служебных объектов
	return 0;
}
