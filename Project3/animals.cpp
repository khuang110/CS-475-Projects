#include "animals.h"
#include <omp.h>
#include <cstdio>

int NowYear;  // 2021 - 2026
int NowMonth; // 0 - 11

float NowPrecip;       // inches of rain per month
float NowTemp;         // temperature this month
float NowHeight;       // grain height in inches
int NowNumDeer;        // number of deer in the current population
float grain_reduction; // grain that didn't mature or died
float now_rel_hum;
const float GRAIN_GROWS_PER_MONTH = 9.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH = 7.0; // average
const float AMP_PRECIP_PER_MONTH = 6.0; // plus or minus
const float RANDOM_PRECIP = 2.0;        // plus or minus noise

const float AVG_TEMP = 60.0;    // average
const float AMP_TEMP = 20.0;    // plus or minus
const float RANDOM_TEMP = 10.0; // plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;


void Deer()
{
	while (NowYear < 2027)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		int nextNumDeer = NowNumDeer;
		int carryingCapacity = (int)(NowHeight);
		if (nextNumDeer < carryingCapacity)
			nextNumDeer++;
		else if (nextNumDeer > carryingCapacity)
			nextNumDeer--;

		if (nextNumDeer < 0)
			nextNumDeer = 0;

		// DoneComputing barrier:
		WaitBarrier();
		NowNumDeer = nextNumDeer;

		// DoneAssigning barrier:
		WaitBarrier();


		// DonePrinting barrier:
		WaitBarrier();
	}
}

void Grain()
{
	while (NowYear < 2027)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float next_height = NowHeight;
		float temp_factor = exp2f(-SQR((NowTemp - MIDTEMP) / 10.f));
		float precip_factor = exp2f(-SQR((NowPrecip - MIDPRECIP) / 10.f));

		next_height += precip_factor * temp_factor * GRAIN_GROWS_PER_MONTH;
		next_height -= ONE_DEER_EATS_PER_MONTH * (float)NowNumDeer;
		if (next_height < 0)
			next_height = 0;

		if (NUMT == 4 && grain_reduction > 0)
			next_height *= 1.f - (grain_reduction / 100.f);
		else if (NUMT == 4 && grain_reduction < 0)
			next_height *= 1.f + (abs(grain_reduction) / 100.f);


		// DoneComputing barrier:
		WaitBarrier();
		NowHeight = next_height;

		// DoneAssigning barrier:
		WaitBarrier();


		// DonePrinting barrier:
		WaitBarrier();
	}
}


void Watcher()
{
	while (NowYear < 2027)
	{
		// DoneComputing barrier:
		WaitBarrier();


		// DoneAssigning barrier:
		WaitBarrier();
		if (NUMT == 4)
			printf("%d-%d\t%.2f\t%.2f\t%.2f\t%d\t%.2f\n", NowMonth + 1, NowYear, NowPrecip, NowTemp, NowHeight,
			       NowNumDeer, now_rel_hum);
		else
			printf("%d-%d\t%.2f\t%.2f\t%.2f\t%d\n", NowMonth + 1, NowYear, NowPrecip, NowTemp, NowHeight,
			       NowNumDeer);

		if (NowMonth < 11)
			NowMonth++;
		else
		{
			NowYear++;
			NowMonth = 0;
		}

		float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

		float temp = AVG_TEMP - AMP_TEMP * cos(ang);
		NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
		NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
		if (NowPrecip < 0.)
			NowPrecip = 0.;

		// DonePrinting barrier:
		WaitBarrier();
	}
}


/** 
 * Calculate effect of grain growth based on relative humidity.
 * Grains will freeze and grow less if it is freezing temperature. 
 */
void MyAgent()
{
	while (NowYear < 2027)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float next_dead_grains = grain_reduction;
		RelHumid();

		if (NowTemp <= FREEZING) // Grain dies if its freezing out
			next_dead_grains += Ranf(&seed, 10.f, 30.f);
		else if (now_rel_hum < 8.f) // Relative humidity too low
			next_dead_grains += Ranf(&seed, 1.f, 6.f);
		else if (8.f <= now_rel_hum && now_rel_hum < 20.f) // Rh in ideal range for grain
			next_dead_grains -= Ranf(&seed, 1.f, 9.f);
		else if (20.f <= now_rel_hum && now_rel_hum < 25.f)	// Fair range for grain growth
			next_dead_grains -= Ranf(&seed, 1.f, 5.f);
		else // Rh on high end and conditions are poor for growing
			next_dead_grains += Ranf(&seed, 1.f, 3.f);

		// Limit the effect of humidity to +-15%
		if (now_rel_hum == 0.f)
			next_dead_grains = 0.f;
		else if (next_dead_grains > 15.f)
			next_dead_grains = 15.f;
		else if (next_dead_grains < -15.f)
			next_dead_grains = -15.f;

		// DoneComputing barrier:
		WaitBarrier();
		grain_reduction = next_dead_grains;

		// DoneAssigning barrier:
		WaitBarrier();


		// DonePrinting barrier:
		WaitBarrier();
	}
}


/**
 * Calculate relative humidity
 * 
 */
void RelHumid()
{
	if (NowTemp == 0 && NowPrecip == 0)
	{
		now_rel_hum = 0.f;
		return;
	}
	float c = NowTemp;
	//float precip_temp = NowPrecip
	float es = 6.11 * (10.f * ((7.5 * c) / (237.2 + c)));
	float e = 6.11 * (10.f * ((7.5 * NowPrecip) / (237.2 + NowPrecip)));
	now_rel_hum = (e / es) * 100.f;
}
