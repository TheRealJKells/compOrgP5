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
void Intrinsic(float * a, float * b, float * c, int size);
float SumOfSums(float * c, int size);
void fillArrays(float * a, float * b, int size);
void output(int size, int iter, float totalSum, timeval &start, timeval &otherEnd);
void neonOutput(int totalSum, timeval &start, timeval &otherEnd);
void clearArrays(float * a, float * b, float * c, int size);


int main(int argc, char *argv[])
{
	int size = 128;
	int iter = 1;

	timeval start;
    timeval otherEnd;

    

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


        gettimeofday(&start, NULL);
		SingleCore(a, b, c, size);
        gettimeofday(&otherEnd, NULL);
		float totalSum = SumOfSums(c, size);
		output(size, iter, totalSum, start, otherEnd);
		clearArrays(a, b, c, size);
		fillArrays(a, b, size);
		gettimeofday(&start, NULL);
		Intrinsic(a, b, c, size);
		gettimeofday(&otherEnd, NULL);
		totalSum = 0;
		totalSum = SumOfSums(c, size);
		neonOutput(totalSum, start, otherEnd);
		

		

		
		return 0;

}

void fillArrays(float * a, float * b, int size)
{
	srand(time(NULL));

	for (int i = 0; i < size; i++)
	{

		a[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		b[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		

	}
}

void neonOutput(int totalSum, timeval &start, timeval &otherEnd)
{
	double realEnd = 0.0;
	double realStart = 0.0;
	realEnd = (otherEnd.tv_usec / 1000000.0) + otherEnd.tv_sec;
    realStart = (start.tv_usec / 1000000.0) + start.tv_sec;

	cout << setprecision(10) << "Neon: " << realEnd - realStart << " Check " << totalSum << endl;

}

void output(int size, int iter, float totalSum, timeval &start, timeval &otherEnd)
{
    double realEnd = 0.0;
    double realStart = 0.0;
    realEnd = (otherEnd.tv_usec / 1000000.0) + otherEnd.tv_sec;
    realStart = (start.tv_usec / 1000000.0) + start.tv_sec;
    

	cout << "Array size: " << size << " total size in MB: " << size / 100000 << endl;
	cout << "Iterations: " << iter << endl;
	cout << "Single core timings..." << endl;
	cout << setprecision(10) << "Naive: " << realEnd - realStart <<  " Check: " << totalSum << endl;

}

void clearArrays(float * a, float * b, float * c, int size)
{
	
	a = NULL;
	b = NULL;
	c = NULL;

}

void Intrinsic(float * a, float * b, float * c, int size){
	assert((size & 0xf) == 0);
	size = size / 4;
	
	for (int i = 0; i < size; i ++)
	{

		float32x4_t A = { *(a++), *(a++), *(a++), *(a++) };
		float32x4_t B = { *(b++), *(b++), *(b++), *(b++) };
		
		float32x4_t C1 = vaddq_f32(A, B);
		*(c++) = C1[0];
		*(c++) = C1[1];
		*(c++) = C1[2];
		*(c++) = C1[3];
	}

}

void SingleCore(float * a, float * b, float * c, int size) {

	//cout << __FUNCTION__ << " " << hex << size << dec << " " << size << endl;
	assert((size & 0xf) == 0);
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
	float totalC = 0.0;
	for (int i = 0; i < size; i++)
	{
		totalC += c[i];
	}
	return totalC;
}