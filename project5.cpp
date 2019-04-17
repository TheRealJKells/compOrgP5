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
void ThreadingNaive(float * a, float * b, float * c, int size, unsigned int cores);
void ThreadingNeon(float * a, float * b, float * c, int size, unsigned int cores);
float SumOfSums(float * c, int size);
void fillArrays(float * a, float * b, int size);
void clearArrays(float * a, float * b, float * c, int size);

void output(int size, int iter, unsigned int cores, float totalSumNaive, float totalSumNeon, float totalSumThreadNaive, float totalSumThreadNeon,
timeval &start, timeval &end, timeval &otherStart, timeval &otherEnd, 
timeval &threadNaiveStart, timeval &threadNaiveEnd, timeval &threadNeonStart, timeval &threadNeonEnd);

int main(int argc, char *argv[])
{
	int size = 128;
	int iter = 1;
	unsigned int cores = thread::hardware_concurrency();

	timeval start;
	timeval end;
	timeval otherStart;
    timeval otherEnd;
	timeval threadNaiveStart;
	timeval threadNaiveEnd;
	timeval threadNeonStart;
	timeval threadNeonEnd;

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

		//Single core timings
		//Naive
        gettimeofday(&start, NULL);
		SingleCore(a, b, c, size);
        gettimeofday(&end, NULL);
		float totalSumNaive = SumOfSums(c, size);
		clearArrays(a, b, c, size);
		fillArrays(a, b, size);
		//Neon
		gettimeofday(&otherStart, NULL);
		Intrinsic(a, b, c, size);
		gettimeofday(&otherEnd, NULL);
		float totalSumNeon = SumOfSums(c, size);

		//Threaded timings
		//Naive
		clearArrays(a, b, c, size);
		fillArrays(a, b, size);
		gettimeofday(&threadNaiveStart, NULL);
		ThreadingNaive(a, b, c, size, cores);
		gettimeofday(&threadNaiveEnd, NULL);
		float totalSumThreadNaive = SumOfSums(c, size);

		//Neon
		clearArrays(a, b, c, size);
		fillArrays(a, b, size);
		gettimeofday(&threadNeonStart, NULL);
		ThreadingNeon(a, b, c, size, cores);
		gettimeofday(&threadNeonEnd, NULL);
		float totalSumThreadNeon = SumOfSums(c, size);
		
		output(size, iter, cores, totalSumNaive, totalSumNeon, totalSumThreadNaive, totalSumThreadNeon, 
		start, end, otherStart, otherEnd, threadNaiveStart, threadNaiveEnd, threadNeonStart, threadNeonEnd);
	
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

void output(int size, int iter, unsigned int cores, float totalSumNaive, float totalSumNeon, 
float totalSumThreadNaive, float totalSumThreadNeon, timeval &start, timeval &end, timeval &otherStart, 
timeval &otherEnd, timeval &threadNaiveStart, timeval &threadNaiveEnd, 
timeval &threadNeonStart, timeval &threadNeonEnd)
{
    double realEndNaive = 0.0;
    double realStartNaive = 0.0;
    realEndNaive = (end.tv_usec / 1000000.0) + end.tv_sec;
    realStartNaive = (start.tv_usec / 1000000.0) + start.tv_sec;

	double realEndNeon = 0.0;
    double realStartNeon = 0.0;
    realEndNeon = (otherEnd.tv_usec / 1000000.0) + otherEnd.tv_sec;
    realStartNeon = (otherStart.tv_usec / 1000000.0) + otherStart.tv_sec;
    
	double realEndThreadNaive = 0.0;
	double realStartThreadNaive = 0.0;
	realEndThreadNaive = (threadNaiveEnd.tv_usec / 1000000.0) + threadNaiveEnd.tv_sec;
    realStartThreadNaive = (threadNaiveStart.tv_usec / 1000000.0) + threadNaiveStart.tv_sec;

	double realEndThreadNeon = 0.0;
	double realStartThreadNeon = 0.0;
	realEndThreadNeon = (threadNeonEnd.tv_usec / 1000000.0) + threadNeonEnd.tv_sec;
    realStartThreadNeon = (threadNeonStart.tv_usec / 1000000.0) + threadNeonStart.tv_sec;

	cout << "Array size: " << size << " total size in MB: " << size / 100000 << endl;
	cout << "Iterations: " << iter << endl;
	cout << "Available cores: " << cores << endl;
	cout << "Single core timings..." << endl;
	cout << setprecision(10) << "Naive: " << realEndNaive - realStartNaive <<  " Check: " << totalSumNaive << endl;
	cout << setprecision(10) << "Neon: " << realEndNeon - realStartNeon << " Check " << totalSumNeon << endl;
	cout << "Threaded timings..." << endl;
	cout << setprecision(10) << "Naive: " << realEndThreadNaive - realStartThreadNaive << " Check " << totalSumThreadNaive << endl;
	cout << setprecision(10) << "Neon: " << realEndThreadNeon - realStartThreadNeon << " Check " << totalSumThreadNeon << endl;
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


//TODO
void ThreadingNaive(float * a, float * b, float * c, int size, unsigned int cores)
{
	//After dividing size by the number of cores that result may not be a factor of 16. 
    //Therefore, you may need to shrink size a little more of each chunk sent to each core. 
    //If there is a remainder, one of your threads need to get a larger size than the 
    //others to account for it.

	vector <std::thread> threads(cores);
	int workloadForEachCore = size / cores;
	int actualWorkload = size / cores;
	int remainder = 0;

	if (workloadForEachCore % 16 != 0)
	{
		remainder = workloadForEachCore % 16;
	}

	//tack on remainder to one of the threads
	//if two cores
	int f = 0;
	for(int i =  0; i < cores; i++)
	{
		if (i == 0)
		{
			actualWorkload = workloadForEachCore + remainder;
		}
		float * workloadA = (float *)aligned_alloc(16, actualWorkload * sizeof(float *));
		float * workloadB = (float *)aligned_alloc(16, actualWorkload * sizeof(float *));

		for(int j = 0; j < actualWorkload; j++)
		{
			workloadA[j] = a[f];
			workloadB[j] = a[f];
			f++;
		}

		//push it to vector threads
		threads.at(i) = thread (SingleCore, workloadA, workloadB, c, actualWorkload);
	}

	for(size_t i = 0; i < threads.size(); i++)
	{
		threads.at(i).join();
	}
}


//TODO
void ThreadingNeon(float * a, float * b, float * c, int size, unsigned int cores)
{
	//After dividing size by the number of cores that result may not be a factor of 16. 
    //Therefore, you may need to shrink size a little more of each chunk sent to each core. 
    //If there is a remainder, one of your threads need to get a larger size than the 
    //others to account for it.

	/*thread first (Intrinsic, a, b, c, size);

	first.join();*/

    vector <std::thread> threads(cores);
	int workloadForEachCore = size / cores;
	int actualWorkload = size / cores;
	int remainder = 0;

	if (workloadForEachCore % 16 != 0)
	{
		remainder = workloadForEachCore % 16;
	}

	//tack on remainder to one of the threads
	//if two cores
	int f = 0;
	for(int i =  0; i < cores; i++)
	{
		if (i == 0)
		{
			actualWorkload = workloadForEachCore + remainder;
		}
		float * workloadA = (float *)aligned_alloc(16, actualWorkload * sizeof(float *));
		float * workloadB = (float *)aligned_alloc(16, actualWorkload * sizeof(float *));

		for(int j = 0; j < actualWorkload; j++)
		{
			workloadA[j] = a[f];
			workloadB[j] = a[f];
			f++;
		}

		//push it to vector threads
		threads.at(i) = thread (Intrinsic, workloadA, workloadB, c, actualWorkload);
	}

	for(size_t i = 0; i < threads.size(); i++)
	{
		threads.at(i).join();
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