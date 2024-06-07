#include <stdio.h>
#include <stdlib.h>

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

    /*
    NOTE:
    Abbiamo scritto questa funzione in modo che funzioni in spatialDim dimensioni per nBodies corpi contemporaneamente.
    Strutturarla così (rendendola un po' più specifica del velocity verlet generico,
    ma comunque generale per sistemi ad nBodies particelle) ha permesso di operare ottimizzazioni nel calcolo della forza.
    Nello specifico ha permesso di evitare il calcolo della forza tra gli stessi due corpi sia dal punto di vista del primo
    che del secondo, ma di riutilizzare (opportunamente cambiato di segno) il calcolo fatto in uno dei due casi anche per l'altro caso.

    Un'altra importante ottimizzazione (sia nella leggibilità del codice che nella performance) è nel calcolo delle posizioni.
    Aggiornando le posizioni un corpo alla volta avrebbe causato problemi nel calcolo della forza applicata ai corpi successivi,
    il primo sarebbe stato nell'istante t + dt, gli altri nell'istante t, questo avrebbe prodotto risultati sbagliati.
    Invece di tenere una copia del vettore posizioni, e fare giri poco chiari all'interno del main per evitare questo problema,
    (che peraltro è comune a ogni tipo di sistema di particelle) abbiamo deciso di fare il calcolo per tutti i corpi contemporaneamente,
    calcolando la forza per tutti i corpi una volta sola all'inizio e aggiornando tutte le posizioni di conseguenza.

    Inoltre abbiamo scritto tutti i parametri necessari e non abbiamo passato la struct physical system perché avrebbe aumentato di
    molto il coupling tra questa funzione e il file main.
    Riconosciamo che la firma della funzione sarebbe stata deciamente più leggibile utilizzando la struct ma dal punto di vista
    logico non era giustificato. Inoltre in questo modo è molto più chiaro quali parametri vengono modificati dalla funzione e quali
    sono soltanto un input costante.
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

// FUNZIONE DI TEST, DA RIMUOVERE
void f_3d_nBodies(long double *coords, long double *force, int nBodies)
{
    int spatialDim = 3;
    for (int i = 0; i < spatialDim * nBodies; i++)
    {
        *(force + i) = -*(coords + i);
    }
}
