/**
 * Funzione che riempie vec_d con il vettore differenza tra vec1 e vec2
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param vec_d Puntatore al vettore risultato di double
 * @param dim Numero intero della dimensione dei vettori
 */
void vec_diff(const double *vec1, const double *vec2, double *vec_d, const int dim);

/**
 * Funzione che calcola la distanza tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore double della distanza euclidea tra i due vettori
 */
double dist(const double *vec1, const double *vec2, const int dim);

/**
 * Funzione che calcola il prodotto scalare tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore double risultato dal prodotto scalare
 */
double scal(const double *vec1, const double *vec2, const int dim);
