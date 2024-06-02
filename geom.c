//gcc GeomCalc.c -std=c99 -o GeomCalc.exe -lm -Wall -Wpedantic

/**prova di vec_dist
 * v1 = ( 0.000000 0.000000)
 * v2 = ( 1.000000 1.000000)
 * la norma del vettore distanza: 1.414214
 */
/**prova di *vec_sum
 * v1 = ( 1.000000 4.000000 1.000000)
 * v2 = ( 2.000000 2.000000 2.000000)
 * Il vettore somma risulta: 
 * v_tot = ( 3.000000 6.000000 3.000000)
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N_OPS 6
#define MAX_LEN 100
double *vec_sum(const double *vec1, const double *vec2, const int dim);
double vec_dist(const double *vec1, const double *vec2, const int dim);

double scal(const double *vec1, const double *vec2, const int dim);
double dist(const double *vec1, const double *vec2, const int dim);
double norm(const double *vec, const int dim);
int min_norm(const double *vec1, const double *vec2, const int dim);
int cross(const double *vec1, const double *vec2, double *vecCross, const int dim);
void read_ops(char **operations, int *order);
void read_vec(double *vec, const int dim, const int vecNumber);
void print_vector_v(const double *vec, const int dim, const char *vectorName);


int main(void)
{
    int order[N_OPS] = {0};
    // vettore contenente i nomi delle operazioni da stampare a schermo
    char *operations[N_OPS] = {"prodotto scalare", "prodotto vettoriale", "distanza", "norma minima", "somma componenti vettore", "vettore distanze dei corpi"};

    int dim;
    int isInputCorrect = 0; // 0 se l'input non è avvenuto correttamente, 1 se l'input è avvenuto correttamente

    do
    {
        printf("Inserire la dimensione dei 3 vettori:\n");
        int scanfResult = scanf("%d", &dim);

        // Se non è stato letto nessun carattere vuol dire che il valore inserito non era un numero.
        // Questo controllo è necessario perché se viene inserito una lettera
        // il programma andrebbe in ciclo infinito.
        if (scanfResult != 1)
        {
            fprintf(stderr, "\nInput non valido. Inserire un numero intero positivo minore o uguale a %d\n\n", MAX_LEN);
        }
        else if (dim < 1 || dim > MAX_LEN)
        {
            fprintf(stderr, "\nIl programma ammette solo dimensione positiva, minore o uguale a %d\n\n", MAX_LEN);
        }
        else
        {
            isInputCorrect = 1;
        }

        // Svuoto il buffer di input. Nel caso sia stata inserita una stringa previene un ciclo infinito,
        // nel caso sia stato inserito un numero con la virgola impedisce al prossimo scanf di prendere
        // il numero dopo la virgola come input. Lo scanf corrente avrà preso la parte intera.
        while (getchar() != '\n');

    } while (isInputCorrect != 1);

    double vec1[dim];
    double vec2[dim];
    
    read_vec(vec1, dim, 1);
    read_vec(vec2, dim, 2);
    

    print_vector_v(vec1, dim, "v1");
    print_vector_v(vec2, dim, "v2");
    

    read_ops(operations, order);

    double crossResult[dim];
    int i = 0;

    while (order[i] != 0 && i < N_OPS)
    {
        switch (order[i])
        {
        case 1:
            printf("Il prodotto scalare vale %lf\n", scal(vec1, vec2, dim));
            break;

        case 2:
            if (cross(vec1, vec2, crossResult, dim) == 0)
            {
                print_vector_v(crossResult, dim, "v1 X v2");
                printf("La norma del prodotto vettoriale vale %lf\n", norm(crossResult, dim));
            }

            break;

        case 3:
            printf("La distanza vale %lf\n", dist(vec1, vec2, dim));
            break;

        case 4:
            printf("Il vettore v%d ha la norma minima\n", min_norm(vec1, vec2, dim));
            break;

        case 5:
            printf("Il vettore somma risulta: \n");
            print_vector_v(vec_sum(vec1, vec2, dim), dim, "v_tot");
            break;

        case 6:
            printf("la norma del vettore distanza: %f\n", vec_dist(vec1, vec2, dim));
                                  
            break;

        default:
            fprintf(stderr, "\nIl programma non è in grado di gestire una delle operazioni specificate.\n\n");
            return 1;
        }
        i++;
    }

    return 0;
}

/**
 * Funzione che calcola il vettore somma tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Vettore double risultato della somma vettoriale
 */
double *vec_sum(const double *vec1, const double *vec2, int dim)
{
    double *v_tot = malloc(dim * sizeof(double));
    for (int i = 0; i < dim; i++)
    { 
        v_tot[i] = vec1[i] + vec2[i];
    }
    return v_tot;
}

/**
 * Funzione che restituisce un vettore contenente le distanze delle posizioni tra il i-esimo corpo e il j-esimo (i,j = 1, 2, 3)
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param vec3 Puntatore al terzo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return puntatore a vettore double risultato delle distanze delle posizioni dei corpi
 */
double vec_dist(const double *vec1, const double *vec2, int dim)
{
    double *v_dist = malloc(dim * sizeof(double));
    double dist;     
    for (int i = 0; i < dim; i++)
    {
        v_dist[i] = abs(vec1[i] - vec2[i]);
    }
    dist = norm(v_dist, dim);
    return dist;
}



/**
 * Funzione che calcola il prodotto scalare tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore double risultato dal prodotto scalare
 */
double scal(const double *vec1, const double *vec2, const int dim)
{
    double sum = 0;

    for (int i = 0; i < dim; i++)
    {
        sum += vec1[i] * vec2[i];
    }

    return sum;
}

/**
 * Funzione che calcola la distanza tra 2 vettori di dimensione specificata
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return Valore double della distanza euclidea tra i due vettori
 */
double dist(const double *vec1, const double *vec2, const int dim)
{
    double sumSquared = 0;

    // NOTA: qui sarebbe stato più elegante calcolare un array con le differenze quadratiche e
    // poi usare la funzione scal. Abbiamo optato per questa versione più efficiente.
    for (int i = 0; i < dim; i++)
    {
        sumSquared += pow(vec1[i] - vec2[i], 2);
    }

    return sqrt(sumSquared);
}

/**
 * Funzione che calcola il prodotto vettoriale tra 2 vettori se questi sono tridimensionali.
 * Altrimenti restituisce errore e ritorna 1
 *
 * @param vec1 Puntatore al primo vettore di double
 * @param vec2 Puntatore al secondo vettore di double
 * @param dim Numero intero della dimensione dei vettori
 *
 * @return 0 se il prodotto vettoriale è stato fatto correttamente, 1 se si sono verificati errori
 */
int cross(const double *vec1, const double *vec2, double *vecCross, const int dim)
{
    if (dim == 3)
    {
        // potrebbero esserci modi più generali per calcolare il prodotto vettoriale, ma scrivere
        // i calcoli a mano è il modo più semplice ed efficiente
        vecCross[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
        vecCross[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
        vecCross[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];

        return 0;
    }
    else
    {
        fprintf(stderr, "Il programma è in grado di calcolare il prodotto vettoriale solo per vettori tridimensionali\n");
        return 1;
    }
}

/**
 * Funzione che calcola la norma del vettore dato in input
 *
 * @param vec Puntatore al vettore di double di cui calcolare la norma
 * @param dim Numero intero che contiene la dimensione del vettore
 *
 * @return Valore double della norma del vettore
 */
double norm(const double *vec, const int dim)
{
    double sumSquared = 0;

    for (int i = 0; i < dim; i++)
    {
        sumSquared += vec[i] * vec[i];
    }

    return sqrt(sumSquared);
}

/**
 * Funzione che ritorna il numero del vettore con norma minore tra quelli dati in input.
 *
 * @param vec1 Puntatore al primo vettore di double da confrontare
 * @param vec2 Puntatore al secondo vettore di double da confrontare
 *
 * @return 1 se il primo vettore ha la norma minore o uguale al secondo, 2 in caso contrario.
 */
int min_norm(const double *vec1, const double *vec2, const int dim)
{
    // in caso di uguaglianza diamo di default il primo vettore come minore:
    // controllare l'uguaglianza esatta di di due numeri double risentirebbe di errori di arrotondamento del sistema
    if (norm(vec1, dim) <= norm(vec2, dim))
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

/**
 * Funzione che chiede all’utente di selezionare le operazioni desiderate tra quelle disponibili
 *
 * @param operations Puntatore ad un vettore di array contenente i nomi delle operazioni disponibili
 * @param order Puntatore al vettore da riempire con le scelte dell'utente in ordine,
 * rappresentate ciascuna dal numero intero inserito dall'utente
 */
void read_ops(char **operations, int *order)
{
    int pick, j = 0;
    int choices[N_OPS] = {0};
    int scanfResult = 0;

    do
    {
        // stampo a schermo le operazioni disponibili all'utente
        printf("Scegliere le operazioni da eseguire:\n");
        for (int i = 0; i < N_OPS; i++)
        {
            printf("%d -- %s\n", i + 1, operations[i]);
        }
        printf("digitare -1 [ INVIO ] per visualizzare il risultato e uscire dal programma\n");         
        scanfResult = scanf("%d", &pick);

        // salvo la scelta dell'utente
        if (pick > N_OPS || pick == 0 || pick < -1 || scanfResult != 1)
        {
            fprintf(stderr, "\nPer favore inserire un numero di operazione tra quelli disponibili\n\n");
        }
        else if (pick != -1)
        {
            if (choices[pick - 1] == 0)
            {
                order[j] = pick;

                j++;
                choices[pick - 1] = 1;
            }
        }

        // Svuoto il buffer di input. Nel caso sia stata inserita una stringa previene un ciclo infinito,
        // nel caso sia stato inserito un numero con la virgola impedisce al prossimo scanf di prendere
        // il numero dopo la virgola come input. Lo scanf corrente avrà preso la parte intera.
        while (getchar() != '\n');
    } while (pick != -1);
}

/**
 * Vettore che legge un vettore della dimensione specificata da stdin.
 *
 * @param vec1 Puntatore all'array di double in cui inserire i valori letti da stdin
 * @param dim Dimensione dell'array in ingresso
 * @param vecNumber Numero del vettore (utilizzato per stampare il messaggio che chiede di inserire gli elementi)
 */
void read_vec(double *vec, const int dim, const int vecNumber)
{
    int i = 0;
    int scanfResult = 0;

    printf("Inserire i %d elementi del vettore %d:\n", dim, vecNumber);
    while (i < dim)
    {
        scanfResult = scanf("%lf", vec + i);

        // Se non è stato letto nessun carattere vuol dire che il valore inserito non era un numero
        // Questo controllo è necessario perché se viene inserito una lettera o un numero con la virgola
        // il programma andrebbe in ciclo infinito.
        if (scanfResult != 1)
        {
            fprintf(stderr, "\nInput non valido. Inserire un numero.\n\n");
            // Svuota il buffer di input
            while (getchar() != '\n');
        }
        else // Input avvenuto correttamente, passo alla componente successiva
        {
            i++;
        }
    }
}

/**
 * Funzione che stampa a schermo un vettore di dimensione e nome specificati
 *
 * @param vec Puntatore all'array di double da stampare
 * @param dim Dimensione dell'array da stampare
 * @param vectorName Nome dell'array da stampare
 */
void print_vector_v(const double *vec, const int dim, const char *vectorName)
{
    printf("%s = (", vectorName);
    for (int i = 0; i < dim; i++)
    {
        printf(" %lf", vec[i]);
    }
    printf(")\n");
}
