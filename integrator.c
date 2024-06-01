// gcc integrator.c -o integrator.exe -Wall -Wpedantic -O3 -lm

#include <stdio.h>
#include <stdlib.h>

#define N_DIM 3 // TODO: remove

int velverlet_ndim(double dt, double *coord, double *vel, double m, double **f_o, double *(F)(double *));
double *f_arm_3d(double *coords);

int main()
{
    // Test velverlet_ndim
    double dt = 0.001;
    double verl_coords[N_DIM] = {1., 1., 1.};
    double verl_vels[N_DIM] = {0., 0., 0.};
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

int velverlet_ndim(double dt, double *coord, double *vel, double m, double **f_o, double *(F)(double *))
{
    double *force_new = NULL;

    // Questo permette di non sapere come va inizializzata la variabile da fuori,
    // basta inizializzare un puntatore a double come NULL e poi passare
    // un puntatore a quel puntatore.
    if (*f_o == NULL)
    {
        *f_o = F(coord);
        if (*f_o == NULL)
        {
            return -1;
        }
    }

    for (int i = 0; i < N_DIM; i++)
    {
        *(coord + i) = *(coord + i) + dt * (*(vel + i)) + (1. / (2. * m)) * dt * dt * (*(*f_o + i));
    }

    force_new = F(coord);

    for (int i = 0; i < N_DIM; i++)
    {
        // Parentesi aggiuntive messe per ordine
        *(vel + i) = *(vel + i) + (1. / (2. * m)) * dt * (*(*f_o + i) + *(force_new + i));
    }

    *f_o = force_new;

    return 0;
}

// FUNZIONE DI TEST, DA RIMUOVERE
double *f_arm_3d(double *coords)
{
    double *force = (double *)malloc(sizeof(double) * N_DIM);
    if (force == NULL)
    {
        return force;
    }

    for (int i = 0; i < N_DIM; i++)
    {
        *(force + i) = -*(coords + i);
    }

    return force;
}