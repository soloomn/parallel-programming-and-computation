#include <mpi.h>
#include <iostream>
#include <iso646.h>
#include <vector>
#include <iomanip>
using namespace std;

void print_array(const vector<int>& arr, int rank, const vector<int>& coords, const string& message) {
	cout << "Proc " << rank << " (coords: [" << coords[0] << ", " << coords[1] << "]): " << message;
	for (int val : arr) {
		cout << setw(3) << val; // Вывод с выравниванием
	}
	cout << endl;
}

int main(int argc, char *argv[])
{
	MPI_Comm grid_comm;
	
	vector<int> b(10);
	vector<int> c(10);

	int dims[2]; // размерность решётки (двумерная)
	int periodic[2]; // логический массив который определяет периодическая ли решётка
	int reorder = 1, q = 5, w = 3, ndims = 2, maxdims = 2;  // 1. системе разрешено менять номер процесса 2. количество строк 3. количество столбцов 4. координаты 5. максимальное число координат
	int coordinates[2]; // массив с координатами
	int grid_rank; // ранг процесса
	int coords[2]; // целочисленный массив в котором лежат координаты
	int ps, pd; // то что получим после шифта 1. процесс отправителя и процесс получателя

	dims[0] = q; // строки
	dims[1] = w; // столбцы
	periodic[0] = periodic[1] = 1; // решётка периодическая
	coords[0] = 0; coords[1] = 1;
	
	MPI_Status st;
	MPI_Init(&argc, &argv);
	MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periodic, reorder, &grid_comm);
	MPI_Comm_rank(MPI_COMM_WORLD, &grid_rank);
	
	MPI_Cart_coords(grid_comm, grid_rank, maxdims, coordinates);

	for (int j = 0; j < 10; j++)
	{
		b[j] = grid_rank;
	}

	vector<int> current_coords(begin(coordinates), end(coordinates));
	print_array(b, grid_rank, current_coords, "b pered sdvigom vpravo:");

	MPI_Cart_shift(grid_comm, 1, 1, &ps, &pd); // сдивг вправо на 1
	MPI_Send(b.data(), 10, MPI_INT, ps, 100, grid_comm);
	MPI_Recv(c.data(), 10, MPI_INT, pd, 100, grid_comm, &st);

	//print_array(c, grid_rank, current_coords, "c posle sdviga vpravo:");

	MPI_Cart_shift(grid_comm, 0, -2, &ps, &pd); // сдивг вверх на 2
	MPI_Send(c.data(), 10, MPI_INT, ps, 120, grid_comm);
	MPI_Recv(b.data(), 10, MPI_INT, pd, 120, grid_comm, &st);

	print_array(b, grid_rank, current_coords, "b posle sdviga vverh:");

	if ((coordinates[0] == 2) and (coordinates[1] == 1))
	{
		cout << "proc (2, 1) - b posle vseh sdvigov: ";
		for (int i = 0; i < 10; i++)
		{
			cout << b[i] <<" ";
		}
	}
	
	MPI_Finalize();
	return 0;
}