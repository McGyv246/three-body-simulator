// gcc -std=c99 -lm -Wall -Wpedantic -O3 main.c -o main.exe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geom.h"
#include "integrator.h"

#define MAX_LEN 1024
#define SPATIAL_DIM 3

// creazione della struct physical system, contenente le variabili di interesse del sistema e le posizioni e velocità dei corpi 
// ad un dato istante
struct physical_system{
    int N;
    double G;
    double dt;
    double tdump;
    double T;
    double *pos;
    double *vel;
};

int read_input(FILE *inFile, struct physical_system *system);

int main(int argc, char const *argv[]) 
{
    FILE *inFile;
    int ans;
    struct physical_system system = {.N = -1, .G = -1, .dt = -1, .tdump = -1, .T = -1};
    
    // errore in caso non sia stato letto alcun file in input
    if (argc < 2)
    {
        fprintf(stderr, "Non è stato specificato un file di input.\n");
        return 1;
    }

    inFile = fopen(argv[1], "r");
    // errore in caso ci siano stati problemi nell'apertura del file
    if (inFile == NULL)
    {
        fprintf(stderr, "Impossibile aprire il file: %s\n", argv[1]);
        return 1;
    }

    // ciclo per la lettura dei dati di input dal file fornito in esecuzione
    while ( (ans = read_input(inFile, &system)) != -1 )
    {   
        if ( ans == -2 )
        {
            fprintf(stderr, "Errore nella lettura del file in input: il file deve contenere N, G, dt, tdump e T");
            return 1;
        }
    }
    
    // -DA TOGLIERE- serve solo per stampare i dati di input in modo da capire se tutto funzioni
    for (int i=0; i<SPATIAL_DIM*system.N; i++)
    {
        printf("%lf\t", system.vel[i]);
    }
    
    fclose(inFile);

    free(system.pos);
    free(system.vel);

    return 0;
}

/**
 * Funzione che legge i dati di input a partire dal file fornito in esecuzione (ogni volta che viene chiamata legge una riga)
 * 
 * @param inFile puntatore al file fornito in esecuzione
 * @param system puntatore alla struct contenente i dati relativi al sistema fisico considerato
 * 
 * @return -1 per End of File; -2 in caso di errore; 0 di default
 */
int read_input(FILE *inFile, struct physical_system *system)
{
    char line[MAX_LEN], str[5];
    char var[6];

    if ( fgets(line, MAX_LEN, inFile) == NULL )
    {
        return -1;
    }

    // serie di controlli che cerca il N, G, dt, tdump e T nell'header e che esclude eventuali commenti 
    if (line[0] == '#')
    {
        sscanf(line, "%4s", str);
        if (strcmp(str, "#HDR") == 0)
        {
            sscanf(line, "%*s %s", var);
            if (strncmp(var,"N",1) == 0)
            {
                sscanf(line, "%*s %*s %d", &system->N);
                return 0;
            }
            if (strncmp(var,"G",1) == 0)
            {
                sscanf(line, "%*s %*s %lf", &system->G);
                return 0;
            }
            if (strncmp(var,"dt",2) == 0)
            {
                sscanf(line, "%*s %*s %lf", &system->dt);
                return 0;
            }
            if (strncmp(var,"tdump",5) == 0)
            {
                sscanf(line, "%*s %*s %lf", &system->tdump);
                return 0;
            }
            if (strncmp(var,"T",1) == 0)
            {
                sscanf(line, "%*s %*s %lf", &system->T);
                return 0;
            }
        }
        return 0;
    }

    // controllo che siano stati letti i dati necessari per l'esecuzione del programma
    if ( system->N == -1 || system->G == -1 || system->dt == -1 || system->tdump == -1 || system->T == -1 )
    {
        return -2;
    }

    int nChar, nTotChar, bodyNumber;

    // codice che permette di puntare in modo "static" alla heap, in questo modo creiamo due
    // vettori di dimensione dinamica direttamente nella funzione (che vengono inizializzati solo una volta per esecuzione)
    static double *pos = NULL;
    if(!pos) pos = (double *)malloc( system->N * SPATIAL_DIM * sizeof(double) ); 
    static double *vel = NULL;
    if(!vel) vel = (double *)malloc( system->N * SPATIAL_DIM * sizeof(double) );

    // assegnazione dei puntatori appena inizializzati ai puntatori della struct
    system->pos = pos;
    system->vel = vel;
    
    // lettura del numero di corpo di cui si stanno leggendo posizioni e velocità di partenza
    sscanf(line, "%d %n", &bodyNumber, &nTotChar);
   
    // ciclo per la lettura della posizione di partenza del corpo specificato da bodyNumber
    for (int i=0; i<SPATIAL_DIM; i++)
    {   
        sscanf(line + nTotChar, "%lf %n", system->pos + i + SPATIAL_DIM * (bodyNumber-1), &nChar);
        nTotChar += nChar;
    }

    // ciclo per la lettura della velocità di partenza del corpo specificato da bodyNumber
    for (int i=0; i<SPATIAL_DIM; i++)
    {
        sscanf(line + nTotChar, "%lf %n", vel + i + SPATIAL_DIM * (bodyNumber-1), &nChar);
        nTotChar += nChar;
    }
    
    return 0;
}
