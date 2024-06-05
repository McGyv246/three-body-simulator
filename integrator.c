#include <stdio.h>
#include <stdlib.h>

/**
 * Funzione che utilizza l'algoritmo Velocity Verlet a spatialDim dimensioni (numero specificato in argomento alla funzione) per
 * calcolare posizioni e velocità di un sistema nBodies particelle soggette a forza specificata.
 *
 * @param dt Differenziale del tempo utilizzato per l'integrazione numerica.
 * @param forceConst long double della costante da utilizzare nel calcolo della forza.
 * @param nBodies Numero intero del numero di corpi considerato nel sistema.
 * @param spatialDim Dimensione spaziale in cui si sta considerando il sistema.
 * @param masses Puntatore ad un array di long double di nBodies elementi contenente le masse dei corpi considerati.
 * @param coord Puntatore ad un array di long double contenente spatialDim * nBodies elementi che corrispondono alle spatialDim componenti
 * delle posizioni di nBodies corpi.
 * La funzione lo aggiorna con le coordinate nuove.
 * @param vel Puntatore ad un array di long double contenente spatialDim * nBodies elementi che corrispondono alle spatialDim componenti
 * delle velocità di nBodies corpi. La funzione lo aggiorna con le velocità nuove.
 * @param force Puntatore ad un array di long double di spatialDim * nBodies elementi che corrispondono alle spatialDim componenti delle
 * forze applicate a nBodies corpi. Deve essere passato da fuori e la funzione lo sovrascrive con le forze calcolate.
 * @param f_o Puntatore a puntatore a un array di long double che contiene le componenti della forza del passo precedente.
 * Per utilizzarlo correttamente bisogna inizializzare un puntatore a long double con NULL e poi passarlo come riferimento
 * (oppure bisogna creare un nuovo puntatore che punta al primo e passare quello). Il resto viene gestito dalla funzione e il contenuto
 * non va modificato fuori. Ad esempio: long double *f_o = NULL; velverlet_ndim(..., &f_o, ...);
 * @param F Funzione che calcola la forza in spatialDim dimensioni di nBodies corpi.
 * Richiede:
 * - un puntatore ad un vettore di spatialDim * nBodies elementi long double che contiene le spatialDim componenti delle posizioni di
 * nBodies corpi.
 * - un puntatore ad un vettore di nBodies elementi long double che contiene le masse dei corpi in studio.
 * - un numero long double che contiene la costante di riferimento per il calcolo della forza.
 * - un numero intero che contiene il numero di corpi considerato nel sistema.
 * - un puntatore a un vettore di spatialDim * nBodies elementi long double in cui la funzione inserisce le spatialDim componenti delle
 * forze applicate a nBodies corpi.
 *
 * @note f_o dovrà essere liberato con la funzione free() dato che allocato nell'heap.
 */
int velverlet_ndim_npart(const long double dt, const long double forceConst, const int nBodies, const int spatialDim, const long double *masses,
                         long double *coord, long double *vel, long double *force, long double **f_o, void (*F)(const long double *, const long double *, const long double, const int, long double *))
{
    /*
    Codice di test:
    long double dt = 0.001;
    long double verl_coords[9] = {1., 1., 1., 1., 1., 1., 1., 1., 1.};
    long double verl_vels[9] = {0., 0., 0., 0., 0., 0., 0., 0., 0.};
    long double masses[3] = {1., 1., 1.};
    long double *force_old_ptr = NULL;
    int nBody = 3;

    long double *force = (long double *)malloc(sizeof(long double) * spatialDim * nBody);

    velverlet_ndim_part(dt, verl_coords, verl_vels, masses, force, nBody, f_3d_nBodies, &force_old_ptr);
    for (int i = 0; i < spatialDim * nBody; i++)
    {
        printf("%d %le %le\n", i, verl_coords[i], verl_vels[i]);
    }

    printf("\n");
    for (int i = 0; i < spatialDim * nBody; i++)
    {
        printf("%d %le\n", i, *(force_old_ptr + i));
    }

    printf("\n");

    velverlet_ndim_npart(dt, verl_coords, verl_vels, masses, force, nBody, f_3d_nBodies, &force_old_ptr);
    for (int i = 0; i < spatialDim * nBody; i++)
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
    // basta inizializzare un puntatore a long double come NULL e poi passare
    // un puntatore a quel puntatore.
    if (*f_o == NULL)
    {
        *f_o = (long double *)malloc(sizeof(long double) * spatialDim * nBodies);
        if (*f_o == NULL)
        {
            fprintf(stderr, "\nErrore nell'allocazione dinamica della memoria.\n\n");
            return -1;
        }

        F(coord, masses, forceConst, nBodies, *f_o);
    }

    for (int j = 0; j < nBodies; j++)
    {
        for (int i = 0; i < spatialDim; i++)
        {
            coord[i + j * spatialDim] = coord[i + j * spatialDim] + dt * vel[i + j * spatialDim] + 1. / (2. * masses[j]) * dt * dt * *(*f_o + i + j * spatialDim);
        }
    }

    F(coord, masses, forceConst, nBodies, force);

    for (int j = 0; j < nBodies; j++)
    {
        for (int i = 0; i < spatialDim; i++)
        {
            vel[i + j * spatialDim] = vel[i + j * spatialDim] + 1. / (2. * masses[j]) * dt * (*(*f_o + i + j * spatialDim) + force[i + j * spatialDim]);

            // Utilizzare malloc nella funzione che calcola la forza sarebbe stato dispendioso in termini di prestazioni.
            // I valori vengono quindi copiati per componente dopo essere stati utilizzati nel conto.
            // Questo risulta più efficiente assumendo che in numero di componenti è piccolo (cosa che ha senso assumere).
            *(*f_o + i + j * spatialDim) = force[i + j * spatialDim];
        }
    }

    return 0;
}

// FUNZIONE DI TEST, DA RIMUOVERE
void f_3d_nBodies(long double *coords, long double *force, int nBodies)
{
    int spatialDim = 3;
    for (int i = 0; i < spatialDim * nBodies; i++)
    {
        *(force + i) = -*(coords + i);
    }
}
