/**prova di vec_diff
 * v1 = ( 0.000000 0.000000)
 * v2 = ( 1.000000 1.000000)
 * la norma del vettore distanza: 1.414214
 */

/**prova Ekin
 * vel = ( 0.000000 1.000000 1.000000)
 * masses = ( 1.000000 2.000000 3.000000)
 * energia cinetica risulta: 2.500000
 */

#include <stdio.h>
#include <math.h>
#define N_OPS 3

void vec_diff(const long double *vec1, const long double *vec2, long double *vec_d, const int dim);
long double dist(const long double *vec1, const long double *vec2, const int dim);
long double scal(const long double *vec1, const long double *vec2, const int dim);

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

    // NOTA: qui sarebbe stato più elegante calcolare un array con le differenze quadratiche e
    // poi usare la funzione scal. Abbiamo optato per questa versione più efficiente.
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
