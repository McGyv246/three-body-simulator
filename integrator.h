#ifndef INTEGRATOR_H
#define INTEGRATOR_H

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
 * delle posizioni di nBodies corpi. La funzione lo aggiorna con le coordinate nuove.
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
 * @return -1 in caso di errore, 0 di default.
 *
 * @note f_o dovrà essere liberato con la funzione free() dato che allocato nell'heap.
 */
int velverlet_ndim_npart(const long double dt, const long double forceConst, const int nBodies, const int spatialDim, const long double *masses,
                         long double *coord, long double *vel, long double *force, long double **f_o, void (*F)(const long double *, const long double *, const long double, const int, long double *));

#endif
