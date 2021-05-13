#include <cmath>
#include <omp.h>

#include "simd.p5.h"

static int NUMT = 1;

#define NUMTRIES 10

void NonSimdMul(float *, float *, float *, int);
float NonSimdMulSum(float *, float *, int);
float Ranf(float low, float high);
int get_args(int argc, char *argv[]);
void single_thread(int exp);
void multi_thread(int exp);

int
main(int argc, char *argv[])
{
    int exp = get_args(argc, argv);

    // Separate functions because no omp parallel is faster single core
    if (NUMT == 1)
        single_thread(exp);
    else
        multi_thread(exp);
    return 0;
}

void
single_thread(int exp)
{
    int arr_size = pow(2, exp);
    float *a = new float[arr_size];
    float *b = new float[arr_size];
    float *c = new float[arr_size];

    for (int i = 0; i < arr_size; i++)
    {
        a[i] = Ranf(0.f, 1.f);
        a[i] = Ranf(0.f, 1.f);
    }
    double non_simd_start,
           non_simd_end,
           simd_start,
           simd_end,
           non_simd_mega_calcs_sec = 0.,
           simd_mega_calcs_sec = 0.;

    for (int t = 0; t < NUMTRIES; t++)
    {
        memset(c, 0, arr_size);

        // Non-SIMD
        non_simd_start = omp_get_wtime();
        NonSimdMul(a, b, c, arr_size);
        NonSimdMulSum(a, b, arr_size);
        non_simd_end = omp_get_wtime();

        // SIMD
        simd_start = omp_get_wtime();
        SimdMul(a, b, c, arr_size);
        SimdMulSum(a, b, arr_size);
        simd_end = omp_get_wtime();

        double non_simd_perf = (double)arr_size / (non_simd_end - non_simd_start) / 1000000.;
        double simd_perf = (double)arr_size / (simd_end - simd_start) / 1000000.;
        if (non_simd_perf > non_simd_mega_calcs_sec)
            non_simd_mega_calcs_sec = non_simd_perf;
        if (simd_perf > simd_mega_calcs_sec)
            simd_mega_calcs_sec = simd_perf;
    }
    fprintf(stderr, "Non-SIMD MegaCalcs/Sec: \tSIMD MegaCalcs/Sec: \n");
    fprintf(stdout, "%f\t\t\t%f\n", non_simd_mega_calcs_sec, simd_mega_calcs_sec);
}

void
multi_thread(int exp)
{
    int arr_size = pow(2, exp);
    omp_set_num_threads(NUMT);
    float *a = new float[arr_size];
    float *b = new float[arr_size];
    float *c = new float[arr_size];
    int num_ele_per_core = arr_size / NUMT;
    for (int i = 0; i < arr_size; i++)
    {
        a[i] = Ranf(0.f, 1.f);
        a[i] = Ranf(0.f, 1.f);
    }
    double non_simd_start,
           non_simd_end,
           simd_start,
           simd_end,
           non_simd_mega_calcs_sec = 0.,
           simd_mega_calcs_sec = 0.;

    for (int t = 0; t < NUMTRIES; t++)
    {
        memset(c, 0, arr_size);


        // Multi-thread SIMD
        simd_start = omp_get_wtime();
#pragma omp parallel
        {
            int f = omp_get_thread_num() * num_ele_per_core;
            SimdMul(&a[f], &b[f], &c[f], num_ele_per_core);       
            SimdMulSum(&a[f], &b[f], num_ele_per_core);
        }
        simd_end = omp_get_wtime();

        // Calculate performance
   
        double simd_perf = (double)arr_size / (simd_end - simd_start) / 1000000.;
        if (simd_perf > simd_mega_calcs_sec)
            simd_mega_calcs_sec = simd_perf;
    }
    fflush(stdout);
    fprintf(stderr, "SIMD MegaCalcs/Sec: ");
    fprintf(stdout, "%d\t%f\n", NUMT, simd_mega_calcs_sec);
}


void
NonSimdMul(float *a, float *b, float *c, int len)
{
    for (int i = 0; i < len; i++)
    {
        c[i] = a[i] * b[i];
    }
}

float
NonSimdMulSum(float *a, float *b, int len)
{
    float sum[4] = {0., 0., 0., 0.};

    for (int i = 0; i < len; i++)
    {
        sum[0] += a[i] * b[i];
    }

    return sum[0] + sum[1] + sum[2] + sum[3];
}

float
Ranf(float low, float high)
{
    float r = (float)rand();
    float t = r / 1.f;

    return low + t * (high - low);
}

int
get_args(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of args!!\n");
        fprintf(stderr, "\tUsage: ./arrmult <arr size exponent> <NUMT>\n");
        exit(1);
    }
    NUMT = atoi(argv[2]);
    return atoi(argv[1]);
}
