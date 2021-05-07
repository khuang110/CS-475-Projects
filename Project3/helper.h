#pragma once

float SQR(float x);
float Ranf(unsigned int *seedp, float low, float high);
int Ranf(unsigned int *seedp, int ilow, int ihigh);
int rand_r(unsigned int *seed);
void InitBarrier(int n);
void WaitBarrier();
