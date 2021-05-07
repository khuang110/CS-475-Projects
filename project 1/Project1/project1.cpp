#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cstdlib>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "helper.h"

// print debugging messages?
#ifndef DEBUG
#define DEBUG	false
#endif

// setting the number of threads:
int Numt = 2;

// setting the number of trials in the monte carlo simulation:
int NumTrials = 50000;

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	8
#endif

// ranges for the random numbers:
const float GMIN = 20.0;	// ground distance in meters
const float GMAX = 30.0;	// ground distance in meters
const float HMIN = 10.0;	// cliff height in meters
const float HMAX = 20.0;	// cliff height in meters
const float DMIN = 10.0;	// distance to castle in meters
const float DMAX = 20.0;	// distance to castle in meters
const float VMIN = 10.0;	// intial cnnonball velocity in meters / sec
const float VMAX = 30.0;	// intial cnnonball velocity in meters / sec
const float THMIN = 30.0;	// cannonball launch angle in degrees
const float THMAX = 70.0;	// cannonball launch angle in degrees

const float GRAVITY = -9.8;	// acceleraion due to gravity in meters / sec^2
const float TOL = 5.0;		// tolerance in cannonball hitting the castle in meters
				// castle is destroyed if cannonball lands between d-TOL and d+TOL

// degrees-to-radians:
inline
float Radians(float d)
{
	return (M_PI / 180.f) * d;
}


// main program:
int
main(int argc, char* argv[])
{
#ifndef _OPENMP
	fprintf(stderr, "No OpenMP support!\n");
	return 1;
#endif

	TimeOfDaySeed();		// seed the random number generator

	HandleArgs(argc, argv, Numt, NumTrials);
	
	omp_set_num_threads(Numt);	// set the number of threads to use in parallelizing the for-loop:`

	// better to define these here so that the rand() calls don't get into the thread timing:
	float* vs = new float[NumTrials];
	float* ths = new float[NumTrials];
	float* gs = new float[NumTrials];
	float* hs = new float[NumTrials];
	float* ds = new float[NumTrials];

	// fill the random-value arrays:
	for (int n = 0; n < NumTrials; n++)
	{
		vs[n] = Ranf(VMIN, VMAX);
		ths[n] = Ranf(THMIN, THMAX);
		gs[n] = Ranf(GMIN, GMAX);
		hs[n] = Ranf(HMIN, HMAX);
		ds[n] = Ranf(DMIN, DMAX);
	}

	// get ready to record the maximum performance and the probability:
	double maxPerformance = 0.;	// must be declared outside the NUMTRIES loop
	int numHits;			// must be declared outside the NUMTRIES loop

	// looking for the maximum performance:
	for (int tries = 0; tries < NUMTRIES; tries++)
	{
		double time0 = omp_get_wtime();
		 
		numHits = 0;

		#pragma omp parallel for shared(numHits, vs, ths, gs, hs, ds)
		for (int n = 0; n < NumTrials; n++)
		{
			// randomize everything:
			float v = vs[n];
			float thr = Radians(ths[n]);
			float vx = v * cos(thr);
			float vy = v * sin(thr);
			float  g = gs[n];
			float  h = hs[n];
			float  d = ds[n];

			// see if the ball doesn't even reach the cliff:`
			float t = -(2.f * vx / GRAVITY);
			float x = 0.f + vy * t;
				if (x <= g)
				{
					if (DEBUG)	fprintf(stderr, "Ball doesn't even reach the cliff\n");
				}
				else
				{
					// see if the ball hits the vertical cliff face:
					t = 0.f + g / vx;
					float y = 0.f + vy * t + (0.5f*GRAVITY * pow(t, 2));
					if (y <= h)
					{
						if (DEBUG)	fprintf(stderr, "Ball hits the cliff face\n");
					}
					else
					{ 
						// the ball hits the upper deck:
						// the time solution for this is a quadratic equation of the form:
						// at^2 + bt + c = 0.
						// where 'a' multiplies time^2
						//       'b' multiplies time
						//       'c' is a constant
						float a = 0.5 * GRAVITY;
						float b = vy;
						float c = -h;
						float disc = b * b - 4.f * a * c;	// quadratic formula discriminant

						// ball doesn't go as high as the upper deck:
						// this should "never happen" ... :-)
						if (disc < 0.)
						{
							if (DEBUG)	fprintf(stderr, "Ball doesn't reach the upper deck.\n");
							exit(1);	// something is wrong...
						}

						// successfully hits the ground above the cliff:
						// get the intersection:
						disc = sqrtf(disc);
						float t1 = (-b + disc) / (2.f * a);	// time to intersect high ground
						float t2 = (-b - disc) / (2.f * a);	// time to intersect high ground

						// only care about the second intersection
						float tmax = t1;
						if (t2 > t1)
							tmax = t2;

						// how far does the ball land horizontlly from the edge of the cliff?
						float upperDist = vx * tmax - g;

						// see if the ball hits the castle:
						if (fabs(upperDist - d) > TOL)
						{
							if (DEBUG)  fprintf(stderr, "Misses the castle at upperDist = %8.3f\n", upperDist);
						}
						else
						{
							if (DEBUG)  fprintf(stderr, "Hits the castle at upperDist = %8.3f\n", upperDist);
							
							numHits++;
						}
						} // if ball clears the cliff face
				} // if ball gets as far as the cliff face
		} // for( # of  monte carlo trials )

		double time1 = omp_get_wtime();
		double megaTrialsPerSecond = (double)NumTrials / (time1 - time0) / 1000000.;
		if (megaTrialsPerSecond > maxPerformance)
			maxPerformance = megaTrialsPerSecond;
	} // for ( # of timing tries )

	float probability = (float)numHits / (float)(NumTrials);	// just get for last NUMTRIES run
	fprintf(stderr, "%2d threads : %8d trials ; probability = %6.2f%% ; megatrials/sec = %6.2lf\n",
		Numt, NumTrials, 100. * probability, maxPerformance);

	// Pipe only data to stdout.
	fprintf(stdout, "%2d %8d %6.2f %6.2f", Numt, NumTrials, 100. * probability, maxPerformance);
	fflush(stdout);
	return 0;
}