#include <iostream>
#include <conio.h>
#include <omp.h>
using std::cout;
using std::endl;

int mainlab2()
{
	const static int n = 20, m = 50;
	int i, j, k;
	int a[n], b[m], c[n + m];

	for (i = 0; i < n; i++)
	{
		a[i] = rand() % 10;
		if (i < 10)
		{
			cout << a[i] << " ";
		}
	}
	cout << endl;

	for (i = 0; i < m; i++)
	{
		b[i] = rand() % 10;
		if (i < 10)
		{
			cout << b[i] << " ";
		}
	}
	cout << endl;

	omp_set_num_threads(100);
#pragma omp parallel for private(k, i, j) 
	for (k = 0; k < m + n; k++)
	{
		c[k] = 0;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < m; j++)
			{
				if (k == (i + j))
				{
					c[k] += a[i] * b[j];
				}
			}
		}
	}

	for (i = 0; i < n + m; i++)
	{
		if (i < 10)
		{
			cout << c[i] << " ";
		}
	}

	_getch();
	return 0;
}