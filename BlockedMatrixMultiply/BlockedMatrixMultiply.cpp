// BlockedMatrixMultiply.cpp : Defines the entry point for the console application.
//

#define ARRAY_SIZE 512

#include <iostream>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <stdint.h>

int64_t GetTimeMs64()
{
#ifdef WIN32
	/* Windows */
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	* to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64_t ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

	return ret;
#else
	/* Linux */
	struct timeval tv;

	gettimeofday(&tv, NULL);

	uint64 ret = tv.tv_usec;
	/* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
	ret /= 1000;

	/* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
	ret += (tv.tv_sec * 1000);

	return ret;
#endif
}

int main(int argc, char* argv[]) {
	int BLOCK_SIZE ;

	for(int blockSize = 2; blockSize < 9; blockSize++) {
		for(int experiment=1;experiment<2;experiment++){
			uint64_t starttime=GetTimeMs64();
			BLOCK_SIZE = (int) pow(2.0, blockSize);

			int** A = new int*[ARRAY_SIZE];
			int** B = new int*[ARRAY_SIZE];
			int** C = new int*[ARRAY_SIZE];

			for(int i = 0; i < ARRAY_SIZE; ++i) {
				A[i] = new int[ARRAY_SIZE];
				B[i] = new int[ARRAY_SIZE];
				C[i] = new int[ARRAY_SIZE]; }

			int x, y;
			for(x = 0; x < ARRAY_SIZE; x ++) {
				for(y = 0; y < ARRAY_SIZE; y ++) {
					A[x][y] = 1;
					B[x][y] = 1;
					C[x][y] = 1; } }

			for (int k = 0; k < ARRAY_SIZE; k += BLOCK_SIZE) {
				std::cout << "k " << k << "\n";
				for (int j = 0; j < ARRAY_SIZE; j += BLOCK_SIZE) {
					//std::cout << "j " << j << "\n";
					for (int i = 0; i < ARRAY_SIZE; i++) {
						//		  std::cout<<"i "<<i<<"\n";
						for (int jj = j; jj < max(j + BLOCK_SIZE, ARRAY_SIZE); jj++) {
							for (int kk = k; kk < max(k + BLOCK_SIZE, ARRAY_SIZE); kk++) {
								int product = *C[i, jj] + *A[i, kk]**B[kk, jj] ;
								*C[i, jj] = product; } } } } }

			// De-Allocate memory to prevent memory leak
			for (int i = 0; i < ARRAY_SIZE; ++i) {
				delete [] A[i];
				delete [] B[i];
				delete [] C[i]; }
			delete [] A;
			delete [] B;
			delete [] C; 
			uint64_t endtime=GetTimeMs64();
			uint64_t runtime=endtime-starttime;

			std::cout << "runtime " << runtime << " seconds ("<<runtime/60<<" minutes) for block size " << blockSize << " experiment " << experiment<<"\n";}
	}

	return 0; }

