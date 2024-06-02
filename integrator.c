// gcc integrator.c -o integrator.exe -Wall -Wpedantic -O3 -lm

#include <stdio.h>
#include <stdlib.h>

#define SPATIAL_DIM 3

/**
 * Funzione che utilizza l'algoritmo Velocity Verlet ad SPATIAL_DIM (macro definita nel file integrator.c) dimensioni per calcolare
 * posizioni e velocità di un sistema nBodies particelle soggette a forza specificata.
 *
 * @param dt Differenziale del tempo utilizzato per l'integrazione numerica
 * @param forceConst Double della costante da utilizzare nel calcolo della forza
 * @param coord Puntatore ad un array di double contenente SPATIAL_DIM * nBodies elementi che corrispondono alle SPATIAL_DIM componenti
 * delle posizioni di nBodies corpi.
 * La funzione lo aggiorna con le coordinate nuove.
 * @param vel Puntatore ad un array di double contenente SPATIAL_DIM * nBodies elementi che corrispondono alle SPATIAL_DIM componenti
 * delle velocità di nBodies corpi. La funzione lo aggiorna con le velocità nuove.
 * @param m Puntatore ad un array di double di nBodies elementi contenente le masse dei corpi considerati.
 * @param force Puntatore ad un array di double di SPATIAL_DIM * nBodies elementi che corrispondono alle SPATIAL_DIM componenti delle
 * forze applicate a nBodies corpi. Deve essere passato da fuori e la funzione lo sovrascrive con le forze calcolate.
 * @param nBodies Numero intero del numero di corpi considerato nel sistema.
 * @param F Funzione che calcola la forza in SPATIAL_DIM dimensioni di nBodies corpi.
 * Richiede:
 * - un puntatore ad un vettore di SPATIAL_DIM * nBodies elementi double che contiene le SPATIAL_DIM componenti delle posizioni di
 * nBodies corpi.
 * - un puntatore ad un vettore di nBodies elementi double che contiene le masse dei corpi in studio.
 * - un numero double che contiene la costante di riferimento per il calcolo della forza.
 * - un numero intero che contiene il numero di corpi considerato nel sistema.
 * - un puntatore a un vettore di SPATIAL_DIM * nBodies elementi double in cui la funzione inserisce le SPATIAL_DIM componenti delle
 * forze applicate a nBodies corpi.
 * @param f_o Puntatore a puntatore a un array di double che contiene le componenti della forza del passo precedente.
 * Per utilizzarlo correttamente bisogna inizializzare un puntatore a double con NULL e poi passarlo come riferimento
 * (oppure bisogna creare un nuovo puntatore che punta al primo e passare quello). Il resto viene gestito dalla funzione e il contenuto
 * non va modificato fuori. Ad esempio: double *f_o = NULL; velverlet_ndim(..., &f_o, ...);
 *
 * @note f_o dovrà essere liberato con la funzione free() dato che allocato nell'heap.
 * Se si ridefinisce SPATIAL_DIM ad un valore maggiore di 0 la funzione funziona lo stesso (solo se anche F funziona lo stesso).
 */
int velverlet_ndim_npart(double dt, double forceConst, double *coord, double *vel, double *m, double *force, int nBodies, void (*F)(double *, double *, double, int, double *), double **f_o)
{
    /*
    Codice di test:
    double dt = 0.001;
    double verl_coords[9] = {1., 1., 1., 1., 1., 1., 1., 1., 1.};
    double verl_vels[9] = {0., 0., 0., 0., 0., 0., 0., 0., 0.};
    double m[3] = {1., 1., 1.};
    double *force_old_ptr = NULL;
    int nBody = 3;

    double *force = (double *)malloc(sizeof(double) * SPATIAL_DIM * nBody);

    velverlet_ndim_part(dt, verl_coords, verl_vels, m, force, nBody, f_3d_nBodies, &force_old_ptr);
    for (int i = 0; i < SPATIAL_DIM * nBody; i++)
    {
        printf("%d %le %le\n", i, verl_coords[i], verl_vels[i]);
    }

    printf("\n");
    for (int i = 0; i < SPATIAL_DIM * nBody; i++)
    {
        printf("%d %le\n", i, *(force_old_ptr + i));
    }

    printf("\n");

    velverlet_ndim_npart(dt, verl_coords, verl_vels, m, force, nBody, f_3d_nBodies, &force_old_ptr);
    for (int i = 0; i < SPATIAL_DIM * nBody; i++)
    {
        printf("%d %le %le\n", i, verl_coords[i], verl_vels[i]);
    }

    Output atteso:
    0 9.999995e-01 -9.999997e-04
    1 9.999995e-01 -9.999997e-04
    2 9.999995e-01 -9.999997e-04
    3 9.999995e-01 -9.999997e-04
    4 9.999995e-01 -9.999997e-04
    5 9.999995e-01 -9.999997e-04
    6 9.999995e-01 -9.999997e-04
    7 9.999995e-01 -9.999997e-04
    8 9.999995e-01 -9.999997e-04

    0 -9.999995e-01
    1 -9.999995e-01
    2 -9.999995e-01
    3 -9.999995e-01
    4 -9.999995e-01
    5 -9.999995e-01
    6 -9.999995e-01
    7 -9.999995e-01
    8 -9.999995e-01

    0 9.999980e-01 -1.999999e-03
    1 9.999980e-01 -1.999999e-03
    2 9.999980e-01 -1.999999e-03
    3 9.999980e-01 -1.999999e-03
    4 9.999980e-01 -1.999999e-03
    5 9.999980e-01 -1.999999e-03
    6 9.999980e-01 -1.999999e-03
    7 9.999980e-01 -1.999999e-03
    8 9.999980e-01 -1.999999e-03
    */

    // Questo permette di non sapere come va inizializzata la variabile da fuori,
    // basta inizializzare un puntatore a double come NULL e poi passare
    // un puntatore a quel puntatore.
    if (*f_o == NULL)
    {
        *f_o = (double *)malloc(sizeof(double) * SPATIAL_DIM * nBodies);
        if (*f_o == NULL)
        {
            fprintf(stderr, "Si è verificato un errore nell'allocazione di memoria.");
            return -1;
        }

        F(coord, m, forceConst, nBodies, *f_o);
    }

    for( int i=0; i<nBodies; i++ )
        {
            for( int j=0; j<SPATIAL_DIM; j++)
            {
                coord[j + i * SPATIAL_DIM] = coord[j + i * SPATIAL_DIM] + dt * vel[j + i * SPATIAL_DIM] + 1. / (2. * m[i]) * 
                dt * dt * (*(*f_o + j + i * SPATIAL_DIM));           
            }
        }

    F(coord, m, forceConst, nBodies, force);

    for( int i=0; i<nBodies; i++ )
        {
            for( int j=0; j<SPATIAL_DIM; j++)
            {
                vel[j + i * SPATIAL_DIM] = vel[j + i * SPATIAL_DIM] + 1. / (2. * m[i]) * dt * (*(*f_o + j + i * SPATIAL_DIM) + 
                force[j + i * SPATIAL_DIM]);

                // Utilizzare malloc nella funzione che calcola la forza sarebbe stato dispendioso in termini di prestazioni.
                // I valori vengono quindi copiati per componente dopo essere stati utilizzati nel conto.
                // Questo risulta più efficiente assumendo che in numero di componenti è piccolo (cosa che ha senso assumere).
                *(*f_o + j + i * SPATIAL_DIM) = force[j + i * SPATIAL_DIM];
            }
        }

    return 0;
}

// FUNZIONE DI TEST, DA RIMUOVERE
void f_3d_nBodies(double *coords, double *force, int nBodies)
{
    for (int i = 0; i < SPATIAL_DIM * nBodies; i++)
    {
        *(force + i) = -*(coords + i);
    }
}
