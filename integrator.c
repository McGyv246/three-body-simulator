// gcc integrator.c -o integrator.exe -Wall -Wpedantic -O3 -lm

#include <stdio.h>
#include <stdlib.h>

#define N_DIM 3

/**
 * Funzione che utilizza l'algoritmo Velocity Verlet ad N_DIM (macro definita nel file integrator.c) dimensioni per calcolare posizioni
 * e velocità di un corpo soggetto a forza specificata.
 *
 * @param dt Differenziale del tempo utilizzato per l'integrazione numerica
 * @param coord Puntatore ad un array di double contenente le N_DIM componenti della posizione del corpo.
 * La funzione lo aggiorna con le coordinate nuove.
 * @param vel Puntatore ad un array di double contenente le N_DIM componenti della velocità del corpo.
 * La funzione lo aggiorna con le velocità nuove.
 * @param m Double contenente la massa del corpo considerato.
 * @param f_o Puntatore a puntatore a un array di double che contiene le componenti della forza del passo precedente.
 * Per utilizzarlo correttamente bisogna inizializzare un puntatore a double a NULL e poi passarlo come riferimento
 * (oppure bisogna creare un nuovo puntatore che punta al primo e passare quello).
 * Ad esempio: double *f_o = NULL; velverlet_ndim(..., &f_o, ...);
 * @param F Funzione che calcola la forza in N_DIM dimensioni.
 * Richiede un puntatore a un vettore di N_DIM coordinate e restituisce un vettore di N_DIM componenti della forza.
 *
 * @note f_o dovrà essere liberato con la funzione free() dato che allocato nell'heap.
 * Se si ridefinisce N_DIM ad un valore maggiore di 0 la funzione funziona lo stesso (solo se anche F funziona lo stesso).
 */
int velverlet_ndim(double dt, double *coord, double *vel, double *m, double *force, double nBodies, void (*F)(double *, double *, int), double **f_o)
{
    /*
    Codice di test:
    double dt = 0.001;
    double verl_coords[9] = {1., 1., 1., 1., 1., 1., 1., 1., 1.};
    double verl_vels[9] = {0., 0., 0., 0., 0., 0., 0., 0., 0.};
    double m[3] = {1., 1., 1.};
    double *force_old_ptr = NULL;
    int nBody = 3;

    double *force = (double *)malloc(sizeof(double) * N_DIM * nBody);

    velverlet_ndim(dt, verl_coords, verl_vels, m, force, nBody, f_3d_nBodies, &force_old_ptr);
    for (int i = 0; i < N_DIM * nBody; i++)
    {
        printf("%d %le %le\n", i, verl_coords[i], verl_vels[i]);
    }

    printf("\n");
    for (int i = 0; i < N_DIM * nBody; i++)
    {
        printf("%d %le\n", i, *(force_old_ptr + i));
    }

    printf("\n");

    velverlet_ndim(dt, verl_coords, verl_vels, m, force, nBody, f_3d_nBodies, &force_old_ptr);
    for (int i = 0; i < N_DIM * nBody; i++)
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
        *f_o = (double *)malloc(sizeof(double) * N_DIM * nBodies);
        if (*f_o == NULL)
        {
            fprintf(stderr, "Si è verificato un errore nell'allocazione di memoria.");
            return -1;
        }

        F(coord, *f_o, nBodies);
    }

    for (int i = 0; i < N_DIM * nBodies; i++)
    {
        double currentM = *(m + (int)(i / nBodies));
        // Parentesi aggiuntive messe per ordine intorno alle dereferenziazioni dei puntatori in una moltiplicazione
        *(coord + i) = *(coord + i) + dt * (*(vel + i)) + 1. / (2. * currentM) * dt * dt * (*(*f_o + i));
    }

    F(coord, force, nBodies);

    for (int i = 0; i < N_DIM * nBodies; i++)
    {
        double currentM = *(m + (int)(i / nBodies));
        *(vel + i) = *(vel + i) + 1. / (2. * currentM) * dt * (*(*f_o + i) + *(force + i));

        // Utilizzare malloc nella funzione che calcola la forza sarebbe stato dispendioso in termini di prestazioni.
        // I valori vengono quindi copiati per componente dopo essere stati utilizzati nel conto.
        // Questo risulta più efficiente assumendo che in numero di componenti è piccolo (cosa che ha senso assumere).
        *(*f_o + i) = *(force + i);
    }

    return 0;
}

// FUNZIONE DI TEST, DA RIMUOVERE
void f_3d_nBodies(double *coords, double *force, int nBodies)
{
    for (int i = 0; i < N_DIM * nBodies; i++)
    {
        *(force + i) = -*(coords + i);
    }
}
