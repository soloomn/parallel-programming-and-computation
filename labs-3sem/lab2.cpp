#include <iostream>
#include <math.h>
#include <mpi.h>
#define n 100 // ��������� n 100 ���������
using namespace std;

// ����� ����� �������� ���������/���������� � ������ ���������

static double dln(double x, double y) // ���������� �� ����� ����� �� ������ �������
{
	return sqrt(x * x + y * y);
}

int main(int argc, char* argcv[]) // ��������� �� ��������� �������, � ���������� ���������� � ���������� ������
{
	double x[n], y[n], * lx, * ly, min, xmin, ymin, lmin, a, b; // ������������ ����� �������
	int i, np, size, * len; // ����� � �������, ��� ��� len - ���������� ��������(������� ����� ��������� *)

	xmin = ymin = 0;

	MPI_Status st; // �����������  � MPI

	MPI_Init(&argc, &argcv); // ������������ � �������� ��������� � ���������� ��� �������� (����� ����� ������ ���-�� ��������� � �����)
	MPI_Comm_size(MPI_COMM_WORLD, &size); // 1.��� �������������; 2.���������� ���-�� �� 1 ������ (��� �������)
	MPI_Comm_rank(MPI_COMM_WORLD, &np); // ����� �������� � np, ������ ������� ����� ���� ���

	len = new int[size]; // ���������� �����

	for (i = 0; i < size; i++) // ���������� ������� �� 0 �� size
	{
		len[i] = n / size; // ������� ������
	}

	len[size - 1] += n % size; // ������� ������� �� �������, size -1 (������ ��� ���������� ���������� � 0)

	lx = new double[len[np]]; // �������� ������ ��� ��������� ������ (���� ��� - ��� ����), �� ������ ������� ������� ���� ���������.
	ly = new double[len[np]];

	if (np == 0) // ��� ��� ������ ��������, ����� �������� ��������
	{
		cout << "initializied lab2; " << "mpi were started " << "\n";
		
		for (i = 0; i < n; i++)
		{
			x[i] = rand() % 200 + (double)rand() / RAND_MAX; // �������������� �������������� � ��������������: ������ ��������� ��� ����� ����� � ������ - ���������� ��������
			y[i] = rand() % 100 + (double)rand() / RAND_MAX;

			cout << x[i] << "\t" << y[i] << endl; // ������ �� ��������
		}

		for (i = 1; i < size; i++) // ������ ���������� 0 �������
		{
			MPI_Send(&x[i * len[0]], len[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD); // ������������ ������� � ������� ��������, �������������� &x - ����� ����� ������ �������� ���������� �� ��������� ��������
			MPI_Send(&y[i * len[0]], len[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD);
		}

		/*������� ��������(1. �� ��� �� ���������� ������� ��������(����� ������, �������� ��������), 2.���������� ��������� (���� ���������� � �������) 3. ���� ���������� (����� �������� ����������), ��� ������������ (�����������, ����������� �������� ������������� �������� ������) 4. ������������*/

		min = dln(x[0], y[0]);

		for (i = 1; i < len[0]; i++)
		{
			if (min > dln(x[i], y[i]))
			{
				min = dln(x[i], y[i]);
				xmin = x[i];
				ymin = y[i];
			}
		} // ����� �� ������� �������� ����� ����� ������ � ������

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
		MPI_Recv(lx, len[np], MPI_DOUBLE, 0, np, MPI_COMM_WORLD, &st); // ��������� ���������, ��������� ��� ����� 0 �������, ����� �� �������� ������� ��������� � ������
		MPI_Recv(ly, len[np], MPI_DOUBLE, 0, np, MPI_COMM_WORLD, &st); // ������� ���� ��������� �����

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

		MPI_Send(&xmin, 1, MPI_DOUBLE, 0, np, MPI_COMM_WORLD); // ���������� � ���-�� ����� ����� � 0 �������
		MPI_Send(&ymin, 1, MPI_DOUBLE, 0, np, MPI_COMM_WORLD);
	}

	MPI_Finalize(); // ����������� ������ �� ��� ��������� ��������
	return 0;
}
