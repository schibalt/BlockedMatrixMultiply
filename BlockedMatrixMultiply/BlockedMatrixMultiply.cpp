// BlockedMatrixMultiply.cpp : Defines the entry point for the console application.
//

#define BUFSIZE 26
#define ARRAY_SIZE 128

#include<stdio.h>
#include <ctime>
#include <fstream>
#include <sstream>
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
	ret /= 10; // From 100 nano seconds (10^-7) 

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

	std::ofstream myfile;
	myfile.open ("bmmexperiments.txt");

	double logBaseTwo =  log(ARRAY_SIZE)/log(2);
	uint64_t programstarttime = GetTimeMs64();

	for(int blockSizeExponent = 2; blockSizeExponent< logBaseTwo; blockSizeExponent++) {
		for(int experiment = 1;experiment<2;experiment++){
			uint64_t starttime = GetTimeMs64();
			BLOCK_SIZE = (int) pow(2.0, blockSizeExponent);

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
					C[x][y] = 0; } }

			for (int k = 0; k < ARRAY_SIZE; k += BLOCK_SIZE) 
			{
				uint64_t blockstarttime=GetTimeMs64();
				int a;
				int b;
				int c;
				int product;

				for (int j = 0; j < ARRAY_SIZE; j += BLOCK_SIZE) {
					//std::cout << "j " << j << "\n";
					for (int i = 0; i < ARRAY_SIZE; i++) {
						//		  std::cout<<"i "<<i<<"\n";
						for (int jj = j; jj < max(j + BLOCK_SIZE, ARRAY_SIZE); jj++) {
							for (int kk = k; kk < max(k + BLOCK_SIZE, ARRAY_SIZE); kk++) {
								a= *A[i, kk];
								b = *B[kk, jj];
								c = *C[i, jj];
								product = c + (a * b);
								*C[i, jj] = product; } } } }

				uint64_t blockendtime = GetTimeMs64();
				uint64_t blockruntime = blockendtime - blockstarttime;

				std::stringstream ss;
				ss << "\tblock " << k / BLOCK_SIZE << " runtime " << blockruntime << " microseconds (" << blockruntime / 1000000 << " seconds/" 
					<< blockruntime / 1000000 / 60 << " minutes)\n";

				std::string logElement=ss.str();
				std::cout << logElement;
				myfile << logElement;
			}

			for(x = 0; x < ARRAY_SIZE; x ++) {
				for(y = 0; y < ARRAY_SIZE; y ++) {
					printf("%d ",*C[x,y]);
				} printf("\n\n");
			}

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

			time_t ltime;
			struct tm now;
			char timebuf[BUFSIZE];
			errno_t err;

			_time64( &ltime ); 
			localtime_s( &now,&ltime );

			// Convert to an ASCII representation. 
			err = strftime(timebuf, BUFSIZE,"%c", &now);

			std::stringstream ss;
			ss << timebuf <<  ": \t runtime " << runtime << " microseconds (" << runtime / 1000000 << " seconds/" << runtime / 1000000 / 60 
				<< " minutes) for block size " << BLOCK_SIZE << " experiment " << experiment << "\n";
			std::string logElement=ss.str();

			std::cout << logElement;
			myfile << logElement;
		}
	}
	uint64_t programendtime = GetTimeMs64();
	uint64_t programruntime = programendtime - programstarttime;

	std::stringstream ss;
	ss << "total run time " << programruntime << " microseconds (" << programruntime / 1000000 << " seconds/" << programruntime / 1000000 / 60 
		<< " minutes)\n";

	std::string logElement=ss.str();
	std::cout << logElement;
	myfile << logElement;
	myfile.close();
	return 0;

}