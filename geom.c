#include <stdio.h>
#include <math.h>

void vec_diff(const long double *vec1, const long double *vec2, long double *vec_d, const int dim)
{
    for (int i = 0; i < dim; i++)
    {
        vec_d[i] = vec1[i] - vec2[i];
    }
}

long double dist(const long double *vec1, const long double *vec2, const int dim)
{
    long double sumSquared = 0.L;

    for (int i = 0; i < dim; i++)
    {
        sumSquared += pow(vec1[i] - vec2[i], 2);
    }

    return sqrt(sumSquared);
}

long double scal(const long double *vec1, const long double *vec2, const int dim)
{
    long double sum = 0.L;

    for (int i = 0; i < dim; i++)
    {
        sum += vec1[i] * vec2[i];
    }

    return sum;
}
