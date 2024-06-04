#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "integrator.h"

#define SPATIAL_DIM 3
#define N_BODIES 3

int velverlet(double dt, double *x, double *v, double (*F)(int, double *), int index, double *m_f)
{
    double force_old;
    double force_new;

    // calcoliamo la forza al tempo t
    // force_old = F(*x);
    force_old = F(index, m_f);

    // aggiorniamo la posizione
    *x = *x + dt * (*v) + 0.5 * dt * dt * force_old;

    // calcoliamo la forza al tempo t+dt
    // force_new = F(*x);
    force_new = F(index, m_f);

    // aggiorniamo la velocita'
    *v = *v + 0.5 * dt * (force_old + force_new);

    return 0;
}

void grav_force(double *coord, double *masses, double G, int N, double *force)
{
    double forceComp, d;
    double vec_d[SPATIAL_DIM];

    for (int i = 0; i < SPATIAL_DIM * N; i++)
    {
        force[i] = 0;
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            vec_dist((coord + j * SPATIAL_DIM), (coord + i * SPATIAL_DIM), vec_d, SPATIAL_DIM);
            d = dist((coord + j * SPATIAL_DIM), (coord + i * SPATIAL_DIM), SPATIAL_DIM);

            for (int k = 0; k < SPATIAL_DIM; k++)
            {

                forceComp = -G * masses[i] * masses[j] * vec_d[k] / pow(d, 3);
                *(force + k + i * SPATIAL_DIM) += forceComp;
                *(force + k + j * SPATIAL_DIM) -= forceComp;
            }
        }
    }
}

double force2(int i, double *force)
{
    return force[i];
}

int main(int argc, char const *argv[])
{
    double coords[9] = {
        -1.0000, // 1
        0.00000,
        0.000000,
        1.0000, // 2
        0.00000,
        0.000000,
        0.0000, // 3
        0.00000,
        0.000000};

    double vels[9] =
        {
            0.392955223941802, 0.0975792352080344, 0.0000000000000000,
            0.392955223941802, 0.0975792352080344, 0.0000000000000000,
            -0.785910447883604, -0.1951584704160688, 0.0000000000000000};

    double *force = (double *)malloc(sizeof(double) * SPATIAL_DIM * N_BODIES);

    double masses[3] = {1., 1., 1.};

    printf("Giro 1\n");

    grav_force(coords, masses, 1, N_BODIES, force);

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        printf("%lf ", force[i]);
    }

    printf("\n\n\n");

    double coordsCp[9];
    double velsCp[9];

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        coordsCp[i] = coords[i];
        velsCp[i] = vels[i];
        /* code */
    }

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        velverlet(0.0000001, &coords[i], &vels[i], force2, i, force);
    }

    double *f_o = NULL;

    velverlet_ndim_npart(0.0000001, 1, coordsCp, velsCp, masses, force, N_BODIES, grav_force, &f_o);

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        printf("%lf ", coords[i]);
        printf("%lf\n", vels[i]);
    }

    printf("\n\n");

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        printf("%lf ", coordsCp[i]);
        printf("%lf\n", velsCp[i]);
    }

    printf("\n\n\n\n");

    for (int i = 0; i < 10000000; i++)
    {
        grav_force(coords, masses, 1, N_BODIES, force);

        // for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
        // {
        //     printf("%lf ", force[i]);
        // }

        // printf("\n\n\n");

        for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
        {
            velverlet(0.0000001, &coords[i], &vels[i], force2, i, force);
        }

        velverlet_ndim_npart(0.0000001, 1, coordsCp, velsCp, masses, force, N_BODIES, grav_force, &f_o);

        // for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
        // {
        //     printf("%lf ", coords[i]);
        //     printf("%lf\n", vels[i]);
        // }

        // printf("\n\n");

        // for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
        // {
        //     printf("%lf ", coordsCp[i]);
        //     printf("%lf\n", velsCp[i]);
        // }
    }

    printf("Giro 10000000\n");

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        printf("%lf ", coords[i]);
        printf("%lf\n", vels[i]);
    }

    printf("\n\n");

    for (int i = 0; i < N_BODIES * SPATIAL_DIM; i++)
    {
        printf("%lf ", coordsCp[i]);
        printf("%lf\n", velsCp[i]);
    }

    printf("\n\n\n\n");

    printf("\n\n\n\n");

    return 0;
}
