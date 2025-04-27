#include <iostream>
#include <math.h>
#include <mpi.h>
using namespace std;

int mainlab5(int argc, char *argcv[])
{
	const int r = 16, c = 8;
	int a[r][c], b[c][r];
	int i, j, np;

	MPI_Status st; //подключение  к MPI
	
	MPI_Datatype column, trans; //свои собственные дататипы для колонок и для объединения повёрнутых колонок в один буфер
	MPI_Init(&argc, &argcv); //используется в обменных операциях и объединяет все процессы (нужно чтобы узнать кол-во процессов и номер)
	MPI_Comm_rank(MPI_COMM_WORLD, &np); //номер процесса в np, каждый процесс знает свое имя

	if (np == 0)
	{
		for (i = 0; i < r; i++)
		{
			for (j = 0; j < c; j++)
			{
				a[i][j] = rand() % 10;
				cout << a[i][j] << " ";
			}
			cout << endl;
		}

		cout << endl;

		MPI_Type_vector(r, 1, c, MPI_INT, &column); //вытаскиваем столбец 1. количество блоков 2. длина блока 3. смещение (сколько надо пропустить чтобы попасть в блок) 4. базовый тип 5. новый тип) в общем сделали так что мы из каждой строки выбрали по одному элементу
		MPI_Type_create_hvector(c, 1, sizeof(int), column, &trans);
		MPI_Type_commit(&trans); //функция закрепления дататипа используем на последнем тк в нём делаем ссылку на column то есть MPI_Type_vector

		MPI_Send(a, 1, trans, 1, 28, MPI_COMM_WORLD);
	}
	else if (np == 1)
	{
		MPI_Recv(b, c*r, MPI_INT, 0, 28, MPI_COMM_WORLD, &st);
		for (i = 0; i < c; i++)
		{
			for (j = 0; j < r; j++)
			{
				cout << b[i][j] << " ";
			}
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}