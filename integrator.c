// gcc integrator.c -o integrator.exe -Wall -Wpedantic -O3 -lm

#include <stdio.h>
#include <stdlib.h>

#define SPATIAL_DIM 3

/**
 * Funzione che utilizza l'algoritmo Velocity Verlet a SPATIAL_DIM (macro definita nel file integrator.c) dimensioni per calcolare posizioni
 * e velocità di un corpo soggetto a forza specificata.
 *
 * @param N Numero di corpi di cui si sta studiando il sistema
 * @param dt Differenziale del tempo utilizzato per l'integrazione numerica
 * @param coord Puntatore ad un array di double contenente le SPATIAL_DIM componenti della posizione del corpo.
 * La funzione lo aggiorna con le coordinate nuove.
 * @param vel Puntatore ad un array di double contenente le SPATIAL_DIM componenti della velocità del corpo.
 * La funzione lo aggiorna con le velocità nuove.
 * @param m Double contenente la massa del corpo considerato.
 * @param f_o Puntatore a puntatore a un array di double che contiene le componenti della forza del passo precedente.
 * Per utilizzarlo correttamente bisogna inizializzare un puntatore a double a NULL e poi passarlo come riferimento
 * (oppure bisogna creare un nuovo puntatore che punta al primo e passare quello).
 * Ad esempio: double *f_o = NULL; velverlet_ndim(..., &f_o, ...);
 * @param F Funzione che calcola la forza in SPATIAL_DIM dimensioni.
 * Richiede un puntatore a un vettore di SPATIAL_DIM coordinate e restituisce un vettore di SPATIAL_DIM componenti della forza.
 *
 * @note f_o dovrà essere liberato con la funzione free() dato che allocato nell'heap.
 * Se si ridefinisce SPATIAL_DIM ad un valore maggiore di 0 la funzione funziona lo stesso (solo se anche F funziona lo stesso).
 */
int velverlet_ndim(double N, double dt, double *coord, double *vel, double m, double **f_o, void (*F)(double *, double *))
{
    /*
    Codice di test:
    double dt = 0.001;
    double verl_coords[3] = {1., 1., 1.};
    double verl_vels[3] = {0., 0., 0.};
    double m = 1;
    double *force_old_ptr = NULL;

    velverlet_ndim(dt, verl_coords, verl_vels, m, &force_old_ptr, f_arm_3d);
    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        printf("%le %le\n", verl_coords[i], verl_vels[i]);
    }

    printf("\n");
    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        printf("%le\n", *(force_old_ptr + i));
    }

    printf("\n");

    velverlet_ndim(dt, verl_coords, verl_vels, m, &force_old_ptr, f_arm_3d);
    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        printf("%le %le\n", verl_coords[i], verl_vels[i]);
    }

    Output atteso:
    9.999995e-01 -9.999997e-04
    9.999995e-01 -9.999997e-04
    9.999995e-01 -9.999997e-04

    -9.999995e-01
    -9.999995e-01
    -9.999995e-01

    9.999980e-01 -1.999999e-03
    9.999980e-01 -1.999999e-03
    9.999980e-01 -1.999999e-03
    */
    double force_new[SPATIAL_DIM] = {0.};

    // Questo permette di non sapere come va inizializzata la variabile da fuori,
    // basta inizializzare un puntatore a double come NULL e poi passare
    // un puntatore a quel puntatore.
    if (*f_o == NULL)
    {
        *f_o = (double *)malloc(sizeof(double) * SPATIAL_DIM);
        if (*f_o == NULL)
        {
            fprintf(stderr, "Si è verificato un errore nell'allocazione di memoria.");
            return -1;
        }

        F(coord, *f_o);
    }

    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        *(coord + i) = *(coord + i) + dt * (*(vel + i)) + (1. / (2. * m)) * dt * dt * (*(*f_o + i));
    }

    F(coord, force_new);

    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        // Parentesi aggiuntive messe per ordine
        *(vel + i) = *(vel + i) + (1. / (2. * m)) * dt * (*(*f_o + i) + *(force_new + i));

        // Utilizzare malloc nella funzione che calcola la forza sarebbe stato dispendioso in termini di prestazioni.
        // I valori vengono quindi copiati per componente dopo essere stati utilizzati nel conto.
        // Questo risulta più efficiente assumendo che in numero di componenti è piccolo (cosa che ha senso assumere).
        *(*f_o + i) = *(force_new + i);
    }

    return 0;
}

// FUNZIONE DI TEST, DA RIMUOVERE
void f_arm_3d(double *coords, double *force)
{
    const unsigned int n_dim = 3;

    for (int i = 0; i < n_dim; i++)
    {
        *(force + i) = -*(coords + i);
    }
}
