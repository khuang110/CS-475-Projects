#pragma once
#include <cmath>

#include "helper.h"
#ifndef M_PI
#define M_PI 3.1415
#endif

#define FREEZING 32
#define NUMT 4

extern int NowYear;     // 2021 - 2026
extern int NowMonth;    // 0 - 11
extern float NowPrecip; // inches of rain per month
extern float NowTemp;   // temperature this month
extern float NowHeight; // grain height in inches
extern int NowNumDeer;  // number of deer in the current population

static unsigned int seed = 0;


void Deer();
void Grain();
void Watcher();
void MyAgent();
void RelHumid();
