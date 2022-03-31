#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* returns pseudo-random number in the [0, 1] */
double getrand(unsigned int* seed)
{
    return (double)rand_r(seed) / RAND_MAX;
}

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

double func(double x, double y)
{
    return exp((x + y) * (x + y)); // function (can be changed)
}

void serial()
{
    const int n = 100000000;
    unsigned int seed = 1;
    int in = 0;
    double s = 0;
    double t = wtime();
    for (int i = 0; i < n; i++)
    {
        double x = getrand(&seed); /* x in [0, 1] */
        double y = getrand(&seed); /* y in [0, 1-x] */
        if (y <= (1 - x))
        {
            in++;
            s += func(x, y);
        }
    }
    t = wtime() - t;
    double v = (double)in / n; // (double)in cause result must be double
    double res = v * s / in;
    printf("Result: %.12f, n %d\n", res, n);
    printf("Elapsed time (sec.): %.6f\n", t);
}

void parallel()
{
    const int n = 100000000;
    int in = 0;
    double s = 0;
    double t = wtime();
#pragma omp parallel num_threads(8)
    {
        double s_loc = 0;
        int in_loc = 0;
        unsigned int seed = omp_get_thread_num();
#pragma omp for nowait
        for (int i = 0; i < n; i++)
        {
            double x = getrand(&seed); /* x in [0, 1] */
            double y = getrand(&seed); /* y in [0, 1-x] */
            if (y <= (1 - x))
            {
                in_loc++;
                s_loc += func(x, y);
            }
        }
#pragma omp atomic
        s += s_loc;
#pragma omp atomic
        in += in_loc;
    }
    t = wtime() - t;
    double v = (double)in / n;
    double res = v * s / in;
    printf("Result: %.12f, n %d\n", res, n);
    printf("Elapsed time (sec.): %.6f\n", t);
}

int main()
{
    // serial();
    parallel();
}