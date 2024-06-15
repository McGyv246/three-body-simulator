#include <stdio.h>
#include <math.h>

//VAL -0.5 le funzioni usano long double

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
        //VAL -0.5 per variabili long double avreste dovuto usare powl altrimenti
        // non ha davvero senso usare dei long double
        sumSquared += pow(vec1[i] - vec2[i], 2);
    }

    //VAL -0.5 per variabili long double avreste dovuto usare sqrtl altrimenti
    // non ha davvero senso usare dei long double
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
