// gcc -std=c99 -lm -Wall -Wpedantic -O3 main.c -o main.exe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geom.h"
#include "integrator.h"

#define MAX_LEN 1024
#define SPATIAL_DIM 3

// per rendere più facile l'uso del programma poniamo i nomi dei file di output come macro
#define OUTPUT_SYSTEM "traj.dat"
#define OUTPUT_ENERGIES "energies.dat"

// creazione della struct physicalSystem, contenente le variabili di interesse del sistema e le posizioni e velocità dei corpi 
// ad un dato istante
struct physicalSystem{
    int N;
    double G;
    double dt;
    double tdump;
    double T;
    double *masses;
    double *pos;
    double *vel;
    double *acc;
};

int read_input(FILE *inFile, struct physicalSystem *system);
void print_system(FILE *outFile, struct physicalSystem *system);
void print_energies(FILE *outFile, struct physicalSystem *system);
void grav_force(double *pos, double *masses, int G, int N, double *force);

int main(int argc, char const *argv[]) 
{
    FILE *inFile;
    int ans;
    struct physicalSystem system = {.N = -1, .G = -1, .dt = -1, .tdump = -1, .T = -1};
    
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

    fclose(inFile);   
    
    
    FILE *outSystem;
    FILE *outEnergies;
    outSystem = fopen(OUTPUT_SYSTEM, "w");
    outEnergies = fopen(OUTPUT_ENERGIES, "w");

    if (outSystem == NULL || outEnergies == NULL)
    {
        fprintf(stderr, "Errore nell'apertura dei file di output");
        return 1;
    }

    system.acc = (double *)malloc( system.N * SPATIAL_DIM * sizeof(double) );
    
    // ciclo generale che stampa nei file di output ogni "system.tdump" integrazioni
    int totPrint = (int)(system.T/system.tdump);
    for( int i=0; i<totPrint; i++ )
    {
        print_system(outSystem, &system);
        print_energies(outEnergies, &system);

        for( int j=0; j<system.tdump; j++ )
        {
            velverlet_ndim(system.dt,system.pos,system.vel,system.masses,system.acc);
        }
    }

    free(system.masses);
    free(system.pos);
    free(system.vel);
    free(system.acc);

    return 0;
}

/**
 * Funzione che legge i dati di input a partire dal file fornito in esecuzione (ogni volta che viene chiamata legge una riga)
 * 
 * @param inFile Puntatore al file fornito in esecuzione
 * @param system Puntatore alla struct contenente i dati relativi al sistema fisico considerato
 * 
 * @return -1 per End of File; -2 in caso di errore; 0 di default
 */
int read_input(FILE *inFile, struct physicalSystem *system)
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
    static double *masses = NULL;
    if(!masses) masses = (double *)malloc( system->N * sizeof(double) );

    static double *pos = NULL;
    if(!pos) pos = (double *)malloc( system->N * SPATIAL_DIM * sizeof(double) );

    static double *vel = NULL;
    if(!vel) vel = (double *)malloc( system->N * SPATIAL_DIM * sizeof(double) );

    // assegnazione dei puntatori appena inizializzati ai puntatori della struct
    system->masses = masses;
    system->pos = pos;
    system->vel = vel;
    
    // lettura del numero di corpo di cui si stanno leggendo posizioni e velocità di partenza
    sscanf(line, "%d %n", &bodyNumber, &nTotChar);

    // lettura della massa del corpo specificato da bodyNumber
    sscanf(line + nTotChar, "%lf %n", system->masses + (bodyNumber-1), &nChar);
    nTotChar += nChar;
   
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

/**
 * Funzione che date le condizioni del sistema in un dato istante, stampa le posizioni, le velocità e le accelerazioni
 * del dato istante nel file specificato in outFile
 * 
 * @param outFile Puntatore al file in cui stampare posizioni, velocità e accelerazioni del sistema
 * @param system Puntatore alla struct contenente tutte le variabili in gioco nel sistema
 */
void print_system(FILE *outFile, struct physicalSystem *system)
{
    static double t=0.;
    printf("%lf ", t);

    for( int i=0; i<system->N*SPATIAL_DIM; i++)
    {
        printf("%lf ", system->pos[i]);
    }

    for( int i=0; i<system->N*SPATIAL_DIM; i++)
    {
        printf("%lf ", system->vel[i]);
    }    

    for( int i=0; i<system->N*SPATIAL_DIM; i++)
    {
        printf("%lf ", system->acc[i]);
    }        
    
    printf("\n");

    t++;
}

/**
 * Funzione che, date le condizioni del sistema in un dato istante, calcola energia cinetica, potenziale e totale nel dato istante.
 * Stampa poi tutto nel file specificato in outFile
 * 
 * @param outFile Puntatore al file in cui stampare energia cinetica, potenziale e totale del sistema in un dato istante
 * @param system Puntatore alla struct contenente tutte le variabili in gioco nel sistema
 */
void print_energies(FILE *outFile, struct physicalSystem *system)
{
    double kEnergy, potEnergy, totEnergy;
    
    kEnergy = Ekin(system->vel, system->N);
    potEnergy = Epot(system->pos, system->G, system->N);
    totEnergy = kEnergy + potEnergy;

    printf("%16.9lf %16.9lf %16.9lf\n", kEnergy, potEnergy, totEnergy);
}

/**
 * Funzione che, date le posizioni di un numero di corpi specificato in un dato istante, calcola le accelerazioni gravitazionali
 * agenti tra questi nel dato istante
 * 
 * @param pos Puntatore al vettore di double contenente le posizioni dei corpi un corpo alla volta: x11, x12, ..., 
 * @param force Puntatore al vettore di double in cui salvare le forze calcolate
 * @param N Numero di corpi che compongono il sistema considerato
 */
void grav_force(double *pos, double *masses, int G, int N, double *force)
{
    double forceComp;
    for( int i=0; i<N; i++ )
    {
        for( int j=i+1; j<N; j++ )
        {
            for (int k=0; k<SPATIAL_DIM; k++)
            {   
                
                forceComp = -G * masses[i] * masses[j] * vec_dist(*(pos +i ), *(pos + j * SPATIAL_DIM))
                / pow(dist(*(pos + i), *(pos + j * SPATIAL_DIM)),3);
                *(force + k + i * SPATIAL_DIM) += forceComp;
                *(force + k + j * SPATIAL_DIM) -= forceComp;
            }
        }
    }
}