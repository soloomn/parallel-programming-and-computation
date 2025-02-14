#include <iostream>
#include <mpi.h>
using namespace std;
#define n 100 //��������� n 100 ���������

int mainlab1(int argc, char* argcv[]) //��������� �� ��������� �������, � ���������� ���������� � ���������� ������
{
	cout << "initializied lab1 " << " mpi were started " << "\n";

	double m[n], * lm, min, lmin; //������������ ����� �������
	int i, np, size, * len; //����� � �������, ��� ��� len - ��������� ��������(������� ����� ��������� *)
	MPI_Status st; //�����������  � MPI
	MPI_Init(&argc, &argcv); //����������� � �������� �������� � ���������� ��� �������� (����� ����� ������ ���-�� ��������� � �����)
	MPI_Comm_size(MPI_COMM_WORLD, &size); //1) �� ������������� 2)���������� ���-�� �� 1 ������ (��� �������)
	MPI_Comm_rank(MPI_COMM_WORLD, &np); //����� �������� � np, ������ ������� ����� ���� ��
	len = new int[size]; //���������� ������
	for (i = 0; i < size; i++) //���������� ������� �� 0 �� size
		len[i] = n / size; //������� ������
	len[size - 1] += n % size; //������� ������� �� ������, size -1 (������ ��� ��������� ��������� � 0)
	lm = new double[len[np]]; //������� ����� ��� ��������� ������ (���� ��� - �� ����), �� ������ ������� ������� ���� ���������.
	if (np == 0) //��� �� ������ ��������, ����� �������� ��������
	{
		for (i = 0; i < n; i++)
			m[i] = i + 1;
		m[16] = -2;
		for (i = 1; i < size; i++) //����� ��������� 0 �������
			MPI_Send(&m[i * len[0]], len[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD);//������ ��������(1. �� ��� �� ��������� ������� ��������(����� ������, �������� �������), 
		//2.���������� ��������� (���� ��������� � �������) 
		//3. ���� ��������� (����� �������� ���������), ��� ������������ (����������, ����������� �������� ������������� �������� ������) 
		//4. ������������
		min = m[0];
		for (i = 1; i < len[0]; i++)
			if (min > m[i])
				min = m[i];
		for (i = 1; i < size; i++)
		{
			MPI_Recv(&lmin, 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, &st);
			if (min > lmin)
				min = lmin;
			cout << lmin << " " << min << endl;
		}
	}
	else
	{
		MPI_Recv(lm, len[np], MPI_DOUBLE, 0, np, MPI_COMM_WORLD, &st); //��������� ���������, ��������� ��� ����� 0 �������, ����� �� �������� ������� �������� � ������
		lmin = lm[0]; //���������� ������� ������, �� ������ ���������� ��������
		for (i = 1; i < len[np]; i++)
			if (lmin > lm[i])
				lmin = lm[i];
		MPI_Send(&lmin, 1, MPI_DOUBLE, 0, np, MPI_COMM_WORLD); //��������� � ���-�� ����� ����� � 0 �������
		if (np == size - 1)
			for (i = 0; i < len[size - 1]; i++) //���� ����� �������� ���������, �� ������ �� ��� ��� �������
				cout << lm[i] << " " << np << endl;
	}
	MPI_Finalize(); //����������� ����� �� ��� ��������� ��������
	return 0;
}
