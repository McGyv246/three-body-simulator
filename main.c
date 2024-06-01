// gcc -std=c99 -lm -Wall -Wpedantic -O3 main.c -o main.exe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geom.h"
#include "integrator.h"

#define MAX_LEN 1024
#define SPATIAL_DIM 3

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
    struct physical_system system;
    
    if (argc < 2)
    {
        fprintf(stderr, "Non è stato specificato un file di input.\n");
        return 1;
    }

    inFile = fopen(argv[1], "r");

    if (inFile == NULL)
    {
        fprintf(stderr, "Impossibile aprire il file: %s\n", argv[1]);
        return 1;
    }

    while ( (ans = read_input(inFile, &system)) != -1 )
    {   
        int n=0;
        printf("%d", n);
        if ( ans == -2 )
        {
            fprintf(stderr, "Errore nella lettura del file in input: il file deve contenere N, G, dt, tdump e T");
        }
    }
    /*
    for (int i=0; i<(SPATIAL_DIM*system.N); i++)
    {
        printf("%lf\t", system.vel[i]);
    }
    */

    fclose(inFile);
    free(system.pos);
    free(system.vel);
}

int read_input(FILE *inFile, struct physical_system *system)
{
    char line[MAX_LEN], str[5];
    char var[6];

    if ( fgets(line, MAX_LEN, inFile) == NULL )
    {
        return -1;
    }
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
    if ( !system->N || !system->G || !system->dt || !system->tdump || !system->T )
    {
        return -2;
    }

    int nChar, nTotChar, nBody;
    if ((system->pos = (double *)malloc(system->N*SPATIAL_DIM*sizeof(double))) == NULL) {
        fprintf(stderr, "errore nella collocazione della memoria");
        exit(EXIT_FAILURE);
    }
    if ((system->vel = (double *)malloc(system->N*SPATIAL_DIM*sizeof(double))) == NULL) {
        fprintf(stderr, "errore nella collocazione della memoria");
        exit(EXIT_FAILURE);
    }

    sscanf(line, "%d %n", &nBody, &nTotChar);
    printf("%d", nBody);
   
    for (int i=0; i<SPATIAL_DIM; i++)
    {   
        sscanf(line + nTotChar, "%lf %n", system->pos + i + SPATIAL_DIM * (nBody-1), &nChar);
        printf("%lf", system->pos[i+SPATIAL_DIM*(nBody-1)]); 
        nTotChar += nChar;
    }

    for (int i=0; i<SPATIAL_DIM; i++)
    {
        sscanf(line + nTotChar, "%lf %n", system->vel + i + SPATIAL_DIM * (nBody-1), &nChar);
        nTotChar += nChar;
    }
    return 0;
}
