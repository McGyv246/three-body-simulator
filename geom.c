// gcc geom.c -std=c99 -o geom.exe -lm -Wall -Wpedantic

/**prova di vec_dist
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
#define N_OPS 5
// a cosa serve questo? Non ho capito bene
#define MAX_LEN 100
#define SPATIAL_DIM 3

double dist(const double *vec1, const double *vec2, const int dim);
double scal(const double *vec1, const double *vec2, const int dim);
void vec_diff(const double *vec1, const double *vec2, double *vec_d, const int dim);
double Ekin(const double *velVec, const double *masses, const int nBodies);
double Epot(const double *posVec, const double *masses, const double G, const int nBodies);

/**
 * Funzione che riempie vec_d con il vettore differenza tra vec1 e vec2 
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param vec_d Puntatore al vettore risultato di double
 * @param dim Numero intero della dimensione dei vettori
 */
void vec_diff(const double *vec1, const double *vec2, double *vec_d, const int dim)
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
 * Funzione che calcola l'energia cinetica del sistema di un numero di corpi pari a nBodies
 *
 * @param velVec Puntatore al primo di double contenente le velocità dei corpi
 * @param masses Puntatore al vettore di double contenente le masse dei corpi
 * @param nBodies Numero intero del numero di corpi del sistema
 *
 * @return Valore double dell'energia cinetica
 */

double Ekin(const double *velVec, const double *masses, const int nBodies)
{
    int count = 0;
    double kinEnergyTot = 0;
    double v;
    // sono molto in dubbio su questa parte: fare un ciclo in più ogni volta rende tutto troppo inefficiente; il gioco non 
    // vale la candela
    for (int i = 0; i < nBodies + 1; i++)
    {
        if (masses[i] == masses[i + 1])
        {
            count = count + 1;
        }
    }
    if (count == masses[0] * nBodies)
    {
        kinEnergyTot = 0.5 * scal(velVec, velVec, SPATIAL_DIM * nBodies);
    }
    else
    {
        for (int i = 0; i < nBodies; i++)
        {
            kinEnergyTot += 0.5 * masses[i] * scal(velVec + SPATIAL_DIM * i, velVec + SPATIAL_DIM * i, SPATIAL_DIM);
        }
    }
    return kinEnergyTot;
}

/**
 * Funzione che calcola l'energia potenziale del sistema di un numero di corpi pari a nBodies
 *
 * @param posVec Puntatore al vettore di double contenente le posizioni dei corpi
 * @param masses Puntatore al vettore di double contenente le masse dei corpi
 * @param G costante di gravitazione universale
 * @param nBodies Numero intero del numero di corpi del sistema
 *
 * @return Valore double dell'energia potenziale
 */
double Epot(const double *posVec, const double *masses, const double G, const int nBodies)
{
    double potEnergyTot = 0;
    for (int i = 0; i < nBodies; i++)
    {
        for (int j = i + 1; j < nBodies; j++)
        {
            double distance = dist((posVec + i * SPATIAL_DIM), (posVec + j * SPATIAL_DIM), SPATIAL_DIM);
            potEnergyTot += -G * masses[i] * masses[j] / distance;
        }
    }

    return potEnergyTot;
}