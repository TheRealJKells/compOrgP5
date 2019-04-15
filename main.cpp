#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <iomanip>
#include <cassert>
#include <arm_neon.h>
#include <vector>
#include <thread>
#include <sys/time.h>
#include <cstring>
#include <thread>
#include <stdlib.h>

using namespace std;

void SingleCore(float * a, float * b, float * c, int size);
float SumOfSums(float * c, int size);
void fillArrays(float * a, float * b, int size);


int main(int argc, char *argv[])
{
	int size = 128;
	int iter = 1;

	int d;
	while ((d = getopt(argc, argv, "i:s:h")) != -1) {
		switch (d) {
		default:
		case 'h':
			cout << "Help: " << endl;
			cout << "-h         prints this" << endl;
			cout << "-s size    sets size (default is 128 - will be made divisible by 16)" << endl;
			cout << "-i iter    sets iterations (default is 1)" << endl;
			return false;

		case 'i':
			iter = atoi(optarg);
			break;

		case 's':
			size = atoi(optarg);
			break;
		}
	}
		if (size % 2 != 0)
		{
			size++;
		}
		while (size % 16 != 0)
		{
			size += 2;
		}


		float * a = (float *)aligned_alloc(16, size * sizeof(float *));
		float * b = (float *)aligned_alloc(16, size * sizeof(float *));
		float * c = (float *)aligned_alloc(16, size * sizeof(float *));

		fillArrays(a, b, size);


		SingleCore(a, b, c, size);
		float totalSum = SumOfSums(c, size);

		return 0;

}

void fillArrays(float * a, float * b, int size)
{
	srand(time(NULL));

	for (int i = 0; i < size; i++)
	{
		a[i] = rand() % 2;
		b[i] = rand() % 2;
	}
}
void SingleCore(float * a, float * b, float * c, int size) {

	//cout << __FUNCTION__ << " " << hex << size << dec << " " << size << endl;
	assert((size & 0x7) == 0);
	size = size / 16;

	for (int i = 0; i < size; i++) {
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
	}
}

float SumOfSums(float * c, int size)
{

	return 0.0f;
}