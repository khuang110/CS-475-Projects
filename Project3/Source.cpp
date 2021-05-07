#include <iostream>
#include <omp.h>
#include "animals.h"


int main(int argc, char *argv[])
{
	InitBarrier(NUMT);
	// Starting year and month
	NowYear = 2021;
	NowMonth = 0;

	// Starting state
	NowNumDeer = 1;
	NowHeight = 1.f;

	omp_set_num_threads(NUMT); // same as # of sections

#pragma omp parallel sections
	{
#pragma omp section
		{
			Deer();
		}

#pragma omp section
		{
			Grain();
		}

#pragma omp section
		{
			Watcher();
		}

#pragma omp section
		{
			MyAgent();
		}
	} // implied barrier -- all functions must return in order
	// to allow any of them to get past here
}
