#include <stdio.h>
#include <stdlib.h>

//VAL -0.5 le funzioni usano long double

int velverlet_ndim_npart(const long double dt, const long double forceConst, const int nBodies, const int spatialDim, const long double *masses,
                         long double *coord, long double *vel, long double *force, long double **f_o, void (*F)(const long double *, const long double *, const long double, const int, long double *))
{
    // Questo permette di non sapere come va inizializzata la variabile da fuori,
    // basta inizializzare un puntatore a long double come NULL e poi passare
    // un puntatore a quel puntatore.
    //VAL -0.5 potevate semplicemente passare il puntatore invece del puntatore che punta al puntatore...
    if (!*f_o)
    {
        *f_o = (long double *)malloc(sizeof(long double) * spatialDim * nBodies);
        if (!*f_o)
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
            coord[i + j * spatialDim] = coord[i + j * spatialDim] + dt * vel[i + j * spatialDim] + 1.L / (2.L * masses[j]) * dt * dt * *(*f_o + i + j * spatialDim);
        }
    }

    F(coord, masses, forceConst, nBodies, force);

    for (int j = 0; j < nBodies; j++)
    {
        for (int i = 0; i < spatialDim; i++)
        {
            vel[i + j * spatialDim] = vel[i + j * spatialDim] + 1.L / (2.L * masses[j]) * dt * (*(*f_o + i + j * spatialDim) + force[i + j * spatialDim]);

            // Utilizzare malloc nella funzione che calcola la forza sarebbe stato dispendioso in termini di prestazioni.
            // I valori vengono quindi copiati per componente dopo essere stati utilizzati nel conto.
            // Questo risulta più efficiente assumendo che in numero di componenti è piccolo (cosa che ha senso assumere).
            *(*f_o + i + j * spatialDim) = force[i + j * spatialDim];
        }
    }

    return 0;
}
