// gcc integrator.c -o integrator.exe -Wall -Wpedantic -O3 -lm

#include <stdio.h>
#include <stdlib.h>

#define N_DIM 3

int velverlet_ndim(double dt, double *coord, double *vel, double m, double **f_o, void (*F)(double *, double *));
void f_arm_3d(double *coords, double *force);

int main()
{
    // Test velverlet_ndim
    double dt = 0.001;
    double verl_coords[3] = {1., 1., 1.};
    double verl_vels[3] = {0., 0., 0.};
    double m = 1;
    double *force_old_ptr = NULL;

    velverlet_ndim(dt, verl_coords, verl_vels, m, &force_old_ptr, f_arm_3d);
    for (int i = 0; i < N_DIM; i++)
    {
        printf("%le %le\n", verl_coords[i], verl_vels[i]);
    }
    /*
    Output atteso:
    9.999995e-01 -9.999997e-04
    9.999995e-01 -9.999997e-04
    9.999995e-01 -9.999997e-04
    */
    // fine test velverlet_ndim

    return 0;
}

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
int velverlet_ndim(double dt, double *coord, double *vel, double m, double **f_o, void (*F)(double *, double *))
{
    double force_new[N_DIM] = {0.};

    // Questo permette di non sapere come va inizializzata la variabile da fuori,
    // basta inizializzare un puntatore a double come NULL e poi passare
    // un puntatore a quel puntatore.
    if (*f_o == NULL)
    {
        *f_o = (double *)malloc(sizeof(double) * N_DIM);
        if (*f_o == NULL)
        {
            fprintf(stderr, "Si è verificato un errore nell'allocazione di memoria.");
            return -1;
        }

        F(coord, *f_o);
    }

    for (int i = 0; i < N_DIM; i++)
    {
        *(coord + i) = *(coord + i) + dt * (*(vel + i)) + (1. / (2. * m)) * dt * dt * (*(*f_o + i));
    }

    F(coord, force_new);

    for (int i = 0; i < N_DIM; i++)
    {
        // Parentesi aggiuntive messe per ordine
        *(vel + i) = *(vel + i) + (1. / (2. * m)) * dt * (*(*f_o + i) + *(force_new + i));
    }

    *f_o = force_new;

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