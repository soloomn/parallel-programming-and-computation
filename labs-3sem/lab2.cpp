#include <iostream>
#include <math.h>
#include <mpi.h>
#define n 100 // константа n 100 элементов
using namespace std;

// найти точку наиболее отдалённую/прилежащую к началу координат

static double dln(double x, double y) // расстояние от любой точки до начала отсчёта
{
	return sqrt(x * x + y * y);
}

int main(int argc, char* argcv[]) // указатель на командную строчку, и количество параметров в коммандной строке
{
	double x[n], y[n], * lx, * ly, min, xmin, ymin, lmin, a, b; // вещественные числа массива
	int i, np, size, * len; // целое с длинной, так как len - переменная величина(поэтому пишем указатель *)

	xmin = ymin = 0;

	MPI_Status st; // подключение  к MPI

	MPI_Init(&argc, &argcv); // используется в обменных операциях и объединяет все процессы (нужно чтобы узнать кол-во процессов и номер)
	MPI_Comm_size(MPI_COMM_WORLD, &size); // 1.имя коммуникатора; 2.переложить кол-во из 1 пункта (при запуске)
	MPI_Comm_rank(MPI_COMM_WORLD, &np); // номер процесса в np, каждый процесс знает свое имя

	len = new int[size]; // заполнение длины

	for (i = 0; i < size; i++) // заполнение массива от 0 до size
	{
		len[i] = n / size; // находим ячейки
	}

	len[size - 1] += n % size; // находим остаток от деления, size -1 (потому что индексация нацинается с 0)

	lx = new double[len[np]]; // выделяем память под локальный массив (один код - для всех), из одного массива создаем свой маленький.
	ly = new double[len[np]];

	if (np == 0) // код для одного процесса, через условный оператор
	{
		cout << "initializied lab2; " << "mpi were started " << "\n";
		
		for (i = 0; i < n; i++)
		{
			x[i] = rand() % 200 + (double)rand() / RAND_MAX; // статистические преобразования с псевдорандомом: первое слагаемое даёт целую часть а второе - определяет мантиссу
			y[i] = rand() % 100 + (double)rand() / RAND_MAX;

			cout << x[i] << "\t" << y[i] << endl; // вывели на проверку
		}

		for (i = 1; i < size; i++) // нельзя отправлять 0 индексу
		{
			MPI_Send(&x[i * len[0]], len[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD); // распределили начиная с первого процесса, соответственно &x - адрес буфер начала отправки умноженный на некоторое смещение
			MPI_Send(&y[i * len[0]], len[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD);
		}

		/*функция отправки(1. то что мы отправляем первому процессу(буфер данных, велчиина смещения), 2.количество процессов (кому отправляем и сколько) 3. куда отправляем (номер процесса получателя), тэг безопасности (маркируется, гарантирует взаимную однозначность передачи данных) 4. коммуникатов*/

		min = dln(x[0], y[0]);

		for (i = 1; i < len[0]; i++)
		{
			if (min > dln(x[i], y[i]))
			{
				min = dln(x[i], y[i]);
				xmin = x[i];
				ymin = y[i];
			}
		} // нашли на нулевом процессе точку самую близку к началу

		for (i = 1; i < size; i++)
		{

			MPI_Recv(&a, 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, &st);
			MPI_Recv(&b, 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, &st);

			if (min > dln(a, b))
			{
				min = dln(a, b);
				xmin = a;
				ymin = b;
			}
		}

		cout << "min tochka " << xmin << "  " << ymin << "  " << "rasstoyanie = " << min;
	}

	else
	{
		MPI_Recv(lx, len[np], MPI_DOUBLE, 0, np, MPI_COMM_WORLD, &st); // принимаем сообщение, выполняют все кроме 0 индекса, берем то значение которое относится к номеру
		MPI_Recv(ly, len[np], MPI_DOUBLE, 0, np, MPI_COMM_WORLD, &st); // приняли свои локальные части

		lmin = dln(lx[0], ly[0]);

		for (i = 1; i < len[np]; i++)
		{
			if (lmin > dln(lx[i], ly[i]))
			{
				lmin = dln(lx[i], ly[i]);
				xmin = lx[i];
				ymin = ly[i];
			}
		}

		MPI_Send(&xmin, 1, MPI_DOUBLE, 0, np, MPI_COMM_WORLD); // отправляем в кол-ве одной штуки в 0 процесс
		MPI_Send(&ymin, 1, MPI_DOUBLE, 0, np, MPI_COMM_WORLD);
	}

	MPI_Finalize(); // освобождает память из под служебных объектов
	return 0;
}
