#ifndef GEOM_H
#define GEOM_H

/**
 * Funzione che riempie vec_d con il vettore differenza tra vec1 e vec2
 *
 * @param vec1 Puntatore al primo vettore di long double
 * @param vec2 Puntatore al secondo vettore di long double
 * @param vec_d Puntatore al vettore risultato di long double
 * @param dim Numero intero della dimensione dei vettori
 */
void vec_diff(const long double *vec1, const long double *vec2, long double *vec_d, const int dim);

/**
 * Funzione che calcola la distanza tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di long double
 * @param vec2 Puntatore al secondo vettore di long double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore long double della distanza euclidea tra i due vettori
 */
long double dist(const long double *vec1, const long double *vec2, const int dim);

/**
 * Funzione che calcola il prodotto scalare tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di long double
 * @param vec2 Puntatore al secondo vettore di long double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore long double risultato dal prodotto scalare
 */
long double scal(const long double *vec1, const long double *vec2, const int dim);

#endif
