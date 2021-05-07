#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <omp.h>
#define _USE_MATH_DEFINES

#define XMIN     -1.f
#define XMAX      1.f
#define YMIN     -1.f
#define YMAX      1.f

#define N	0.70

#define min(a, b) a<b?a:b

#ifndef NUMTRIES
#define NUMTRIES 10
#endif


#ifndef ARGS
static uint8_t numt = 4;
static uint32_t num_nodes = 10000;
#endif


float Height(int, int); // function prototype
void HandleArgs(int, char *[]);

int
main(int argc, char *argv[])
{
#ifndef _OPENMP
	fprintf(stderr, "No OpenMP support!\n");
	return 1;
#endif

	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)
	uint8_t sys_max_threads = omp_get_max_threads();
	HandleArgs(argc, argv);
	fprintf(stderr, "Num nodes: %d\n", num_nodes);
	fprintf(stderr, "Using %d threads\n\n", min(numt, sys_max_threads));
	omp_set_num_threads(min(numt, sys_max_threads));

	const float fullTileArea = (((XMAX - XMIN) / (float)(num_nodes - 1)) *
		((YMAX - YMIN) / (float)(num_nodes - 1)));

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:

	// Record Max performance
	double max_performance = 0.0;
	long double volume = 0.;

	for (int j = 0; j < NUMTRIES; j++)
	{
		double t_start = omp_get_wtime();

#pragma omp parallel for default(none), shared(fullTileArea), reduction(+:volume)
		for (int_fast64_t i = 0; i < num_nodes * num_nodes; i++)
		{
			uint32_t iu = i % num_nodes;
			uint32_t iv = i / num_nodes;
			float z = Height(iu, iv);
			if (z == 0)
				continue;
			bool iv_edge = iv == 0 || iv == num_nodes - 1,
			     iu_edge = iu == 0 || iu == num_nodes - 1;


			if (iu_edge && iv_edge) // Corner case
				volume += z * (fullTileArea * 0.25f);
			else if (iu_edge || iv_edge) // Edge case
				volume += z * (fullTileArea * 0.5f);
			else // Center case
				volume += z * fullTileArea;
		}
		double t_end = omp_get_wtime();

		double performance = (double)(num_nodes * num_nodes) / (t_end - t_start) / 1000000.0;
		if (max_performance < performance)
			max_performance = performance;

		fprintf(stderr, "Volume: %Lf\n", volume * (long long)2.);
	}
	fprintf(stderr, "Performance: %8.lf MegaNodes/Sec\n", max_performance);

	// Output to capture from python script
	//fflush(stderr);
	fprintf(stdout, "%d %d %Lf", numt, num_nodes, volume);
}


float
Height(int iu, int iv) // iu,iv = 0 .. num_nodes-1
{
	float x = -1. + 2. * (float)iu / (float)(num_nodes - 1.); // -1. to +1.
	float y = -1. + 2. * (float)iv / (float)(num_nodes - 1.); // -1. to +1.

	float xn = pow(fabs(x), (double)N);
	float yn = pow(fabs(y), (double)N);
	float r = 1. - xn - yn;
	if (r <= 0.)
		return 0.;
	float height = pow(r, 1. / (float)N);
	return height;
}


void
HandleArgs(int argc, char *argv[])
{
	if (argc >= 2)
	{
		numt = atoi(argv[1]);
		num_nodes = atoi(argv[2]);
	}
	else
	{
		// Not enough args
		fprintf(stderr, "Not enough cmd line arguments!\n");
		fprintf(stderr, "Usage:\n\t\u001b[31;1m./proj2 <NUMT> <NUMNODES>\u001b[37m\n");
		exit(1);
	}
}
