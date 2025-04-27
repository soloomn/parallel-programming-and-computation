#include <iostream>
#include <math.h>
#include <mpi.h>
using namespace std;

struct pl //структура хран€ща€ глобальный индекс и площадь чтобы потом mpi_double_int собрало
{
	double s; //рассчитанна€ площадь 
	int ind; //индекс
};

int mainlab4(int argc, char *argcv[]) //указатель на командную строчку, и количество параметров в коммандной строке
{
	const int n = 20;
	double w[n], h[n], *lw, *lh; //вещественные числа массива
	int i, np, size, *len, *d; //целое с длинной, так как len - переменна€ величина(поэтому пишем указатель *), d - массив смещени€
	
	pl min; //глобальный индекс и площадь
	pl lmin; //локальна€ площадь и локальный индекс наход€щийс€ в процессе - потом транслируем на глобальный

	MPI_Status st; //подключение  к MPI
	
	MPI_Init(&argc, &argcv); //используетс€ в обменных операци€х и объедин€ет все процессы (нужно чтобы узнать кол-во процессов и номер)
	MPI_Comm_size(MPI_COMM_WORLD, &size); //1) им€ коммуникатора 2)переложить кол-во из 1 пункта (при запуске)
	MPI_Comm_rank(MPI_COMM_WORLD, &np); //номер процесса в np, каждый процесс знает свое им€
	
	d = new int[size];

	len = new int[size]; //заполнение длины

	if (size <= n) //если процессов меньше или равно числу значений
	{
		for (i = 0; i < size; i++) //заполнение массива от 0 до size
		{
			len[i] = n / size; //находим €чейки
			d[i] = i * len[0];
		}

		len[size - 1] += n%size; //находим остаток от делени€, size -1 (потому что индексаци€ нацинаетс€ с 0)
	}
	else  //если процессов больше то переделаем size
	{
		for (i = 0; i < n; i++) 
		{
			len[i] = 1; //находим €чейки
			d[i] = i * len[0];
		}
		for (i = n; i < size; i++)
		{
			len[i] = 0;
			d[i] = i * len[0];
		}
	}

	lw = new double[len[np]]; //выдел€ем пам€ть под локальный массив (один код - дл€ всех), из одного массива создаем свой маленький.
	lh = new double[len[np]];

	if (np == 0) //код дл€ одного процесса, через условный оператор
	{
		for (i = 0; i < n; i++)
		{
			w[i] = rand() % 10 + (double)rand() / RAND_MAX;
			h[i] = rand() % 10 + (double)rand() / RAND_MAX;
		}
	}
	
	MPI_Scatterv(w, len, d, MPI_DOUBLE, lw, len[np], MPI_DOUBLE, 0, MPI_COMM_WORLD); //шлЄм процессам распределение сначала дл€ сэнда 1. что 2. сколько 3. с каким смещением 4. тип; дальше дл€ ресива 5. куда принимаем 6. сколько 7. с каким типом 8. от кого 9. коммуникатор
	MPI_Scatterv(h, len, d, MPI_DOUBLE, lh, len[np], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	lmin.s = lw[0] * lh[0];
	lmin.ind = d[np];

	for (i = 1; i < len[np]; i++)
	{
		if ((lw[i] * lh[i]) < lmin.s)
		{
			lmin.s = lw[i] * lh[i];
			lmin.ind = d[np] + i;
		}
	}

	MPI_Reduce(&lmin, &min, 1, MPI_DOUBLE_INT, MPI_MINLOC, 0, MPI_COMM_WORLD); //коллективна€ сборка 1. что отправл€ем 2. куда получаем 3. сколько 4. тип 5. операци€ которую будем примен€ть 6. номер процесса на который отправл€ем 7. коммуникатор

	if (np == 0)
	{
		cout << "ploshad ravna = " << min.s << endl;
		cout << "visota i shirina: " << w[min.ind] << " " << h[min.ind] << endl;
	}

	MPI_Finalize(); //освобождает пам€ть из под служебных объектов
	return 0;
}
