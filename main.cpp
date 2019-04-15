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

int main()
{
    int size = 128;
    int iter = 1;

    while ((c = getopt(argc, argv, "i:s:h")) != -1) {
		switch (c) {
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

		if (size % 2 != 0)
		{
			size++;
		}
		while (size % 16 != 0)
		{
			size += 2;
		}

        float * a = (float *) alligned_alloc(16, size * sizeof(float));
        float * b = (float *) alligned_alloc(16, size * sizeof(float));
        float * c = (float *) alligned_alloc(16, size * sizeof(float));


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

		


    
    return 0;
}