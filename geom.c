// gcc GeomCalc.c -std=c99 -o GeomCalc.exe -lm -Wall -Wpedantic

/**prova di vec_dist
 * v1 = ( 0.000000 0.000000)
 * v2 = ( 1.000000 1.000000)
 * la norma del vettore distanza: 1.414214
 */

/**prova Ekin
 * vel = ( 0.000000 1.000000 1.000000)
 * mass = ( 1.000000 2.000000 3.000000)
 * energia cinetica risulta: 2.500000
 */

#include <stdio.h>
#include <math.h>
#define N_OPS 6
#define MAX_LEN 100
#define SPATIAL_DIM 2

double dist(const double *vec1, const double *vec2, const int dim);
double scal(const double *vec1, const double *vec2, const int dim);
double Ekin(const double *velVec, const double *mass, const int nBodies);
double Epot(const double *posVec, const double *mass, const double G, const int nBodies);
void vec_dist(const double *vec1, const double *vec2, double *vec_d, const int dim);

/**
 * Funzione che restituisce un vettore contenente le distanze delle posizioni tra il i-esimo corpo e il j-esimo (i,j = 1, 2, 3)
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param vec_d Puntatore al vettore risultato di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return puntatore a vettore double risultato delle distanze delle posizioni dei corpi
 */
void vec_dist(const double *vec1, const double *vec2, double *vec_d, const int dim)
{
    for (int i = 0; i < dim; i++)
    {
        vec_d[i] = vec1[i] - vec2[i];
    }
}

/**
 * Funzione che calcola la distanza tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore double della distanza euclidea tra i due vettori
 */
double dist(const double *vec1, const double *vec2, const int dim)
{
    double sumSquared = 0;

    // NOTA: qui sarebbe stato più elegante calcolare un array con le differenze quadratiche e
    // poi usare la funzione scal. Abbiamo optato per questa versione più efficiente.
    for (int i = 0; i < dim; i++)
    {
        sumSquared += pow(vec1[i] - vec2[i], 2);
    }

    return sqrt(sumSquared);
}

/**
 * Funzione che calcola il prodotto scalare tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore double risultato dal prodotto scalare
 */
double scal(const double *vec1, const double *vec2, const int dim)
{
    double sum = 0;

    for (int i = 0; i < dim; i++)
    {
        sum += vec1[i] * vec2[i];
    }

    return sum;
}

/**
 * Funzione che calcola l'energia cinetica del sistema a 3 corpi
 *
 * @param velVec Puntatore al primo vettore di doublle (velocità)
 * @param mass Puntatore al secondo vettore di double (masse)
 * @param nBodies Numero intero della dimensione dei vettori
 *
 * @return Valore double dell'energia cinetica
 */

double Ekin(const double *velVec, const double *mass, const int nBodies)
{
    int count = 0;
    double Ekin = 0;
    double v;
    for (int i = 0; i < nBodies + 1; i++)
    {
        if (mass[i] == mass[i + 1])
        {
            count = count + 1;
        }
    }
    if (count == mass[0] * nBodies)
    {
        Ekin = 0.5 * scal(velVec, velVec, SPATIAL_DIM * nBodies);
    }
    else
    {
        for (int i = 0; i < nBodies; i++)
        {
            v = velVec[i];
            Ekin = Ekin + 0.5 * mass[i] * (v * v);
        }
    }
    return Ekin;
}

/**
 * Funzione che calcola l'energia potenziale del sistema a 3 corpi
 *
 * @param posVec Puntatore al primo vettore di doublle (posizioni)
 * @param mass Puntatore al secondo vettore di double (masse)
 * @param G costante di gravitazione universale
 * @param nBodies Numero intero della dimensione dei vettori
 *
 * @return Valore double dell'energia potenziale
 */
double Epot(const double *posVec, const double *mass, const double G, const int nBodies)
{
    double potEnergyTot = 0;
    for (int i = 0; i < nBodies; i++)
    {
        for (int j = i + 1; j < nBodies; j++)
        {
            double coords1[SPATIAL_DIM] = {posVec[3 * i], posVec[3 * i + 1], posVec[3 * i + 2]};
            double coords2[SPATIAL_DIM] = {posVec[3 * j], posVec[3 * j + 1], posVec[3 * j + 2]};
            double distance = dist(coords1, coords2, 3);
            potEnergyTot += -G * (mass[i] * mass[i + 1]) / distance;
        }
    }

    return potEnergyTot;
}
