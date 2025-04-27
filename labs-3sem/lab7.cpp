#include <mpi.h>
#include <iostream>
using namespace std;

void mainlab7(int argc, char *argv[])
{
	int proc, proc1, proc2, proc3;
	const int g1_size = 3, g2_size = 2, g3_size = 2;

	MPI_Group grp_world, grp1, grp2, grp3;
	MPI_Comm comm1 = MPI_COMM_NULL, comm2 = MPI_COMM_NULL, comm3 = MPI_COMM_NULL; /* интра-коммуникаторы */
	MPI_Comm intercomm, incm2; /* интер-коммуникаторы */

	MPI_Status stat;

	int ranks1[g1_size] = { 0, 1, 2 };
	int ranks2[g2_size] = { 4, 5 };
	int ranks3[g3_size] = { 3, 6 };

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &proc);
	MPI_Comm_group(MPI_COMM_WORLD, &grp_world);

	MPI_Group_incl(grp_world, 3, ranks1, &grp1); // 1. дескриптор базовой группы 2. количество процессов 3. номера процессов 4. дескриптор выходной группы 
	MPI_Comm_create(MPI_COMM_WORLD, grp1, &comm1); // 1. базовый коммуникатор 2. идентификатор группы 3. полученный коммуникатор

	MPI_Group_incl(grp_world, 2, ranks2, &grp2);
	MPI_Comm_create(MPI_COMM_WORLD, grp2, &comm2);

	MPI_Group_incl(grp_world, 2, ranks3, &grp3);
	MPI_Comm_create(MPI_COMM_WORLD, grp3, &comm3);

	if (comm1 != MPI_COMM_NULL)
	{
		MPI_Comm_rank(comm1, &proc1);
		MPI_Intercomm_create(comm1, 0, MPI_COMM_WORLD, 4, 100, &intercomm); // 1. локальный коммуникатор 2. номер процесса отвечающего за св€зь 3. коммуникатор посредник 4. номер процесса с которым свз€ь (номер по нумерации посредника) 5. тег 6. выходной коммуникатор
		
		if (proc1 == 0)    
		{
			int b[10];
			MPI_Recv(b, 10, MPI_INT, 0, 99, intercomm, &stat);

			for (int i = 0; i < 10; i++)
			{
				cout << b[i] << endl;
			}
		}
	}

	if (comm2 != MPI_COMM_NULL)
	{
		MPI_Comm_rank(comm2, &proc2);
		MPI_Intercomm_create(comm2, 0, MPI_COMM_WORLD, 0, 100, &intercomm); // 1. локальный коммуникатор 2. номер процесса отвечающего за св€зь 3. коммуникатор посредник 4. номер процесса с которым св€зь (номер по нумерации посредника) 5. тег 6. выходной коммуникатор
		MPI_Intercomm_create(comm2, 0, MPI_COMM_WORLD, 3, 200, &incm2);

		if (proc2 == 0)
		{
			int a[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			MPI_Send(a, 10, MPI_INT, 0, 99, intercomm);
			MPI_Send(a, 10, MPI_INT, 0, 99, incm2);
		}
	}

	if (comm3 != MPI_COMM_NULL)
	{
		MPI_Comm_rank(comm3, &proc3);
		MPI_Intercomm_create(comm3, 0, MPI_COMM_WORLD, 4, 200, &incm2);

		if (proc3 == 0)
		{
			int b[10];
			MPI_Recv(b, 10, MPI_INT, 0, 99, incm2, &stat);
			for (int i = 0; i < 10; i++)
			{
				cout << b[i] << endl;
			}
		}
	}

	if (MPI_Group_free(&grp_world) != MPI_SUCCESS)
	{
		cerr << "error freeing group grp_world" << endl;
	}
	if (MPI_Group_free(&grp1) != MPI_SUCCESS)
	{
		cerr << "error freeing group grp1" << endl;
	}
	if (MPI_Group_free(&grp2) != MPI_SUCCESS)
	{
		cerr << "error freeing group grp2" << endl;
	}
	if (MPI_Group_free(&grp3) != MPI_SUCCESS)
	{
		cerr << "error freeing group grp3" << endl;
	}

	MPI_Finalize();
}