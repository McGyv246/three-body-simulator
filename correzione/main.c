// gcc -std=c99 -Wall -Wpedantic -O3 main.c integrator.c geom.c -o main.exe -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "geom.h"
#include "integrator.h"

#define MAX_LEN 1024
#define SPATIAL_DIM 3
#define N_HEADERS 5

// per rendere più facile il mantenimento del programma poniamo i nomi dei file di output come macro
#define OUTPUT_SYSTEM "traj.dat"
#define OUTPUT_ENERGIES "energies.dat"

// Rimuovere la riga qui sotto per evitare le citazioni all'inizio dei file di output
#define FUNNY

#ifdef FUNNY
#include <time.h>
#define N_QUOTES 7
#endif

/**
 * Creazione della struct PhysicalSystem contenente le variabili di interesse per un sistema ad nBodies corpi soggetti a forze di natura
 * gravitazionale:
 * - nBodies : numero di corpi;
 * - G : costante di gravitazione;
 * - dt : intervallo di integrazione (vedere integrator.c);
 * - tdump : numero di integrazioni ogni cui stampare nei file di output;
 * - T : numero totale di integrazioni da eseguire (il formato long int consente di estendere il limite massimo di lettura di T);
 * - masses : puntatore a cui assegnare le masse dei corpi del sistema;
 * - coord : puntatore a cui assegnare le coordinate in SPATIAL_DIM dimensioni dei corpi del sistema in un dato istante;
 * - vel : puntatore a cui assegnare le velocità in SPATIAL_DIM dimensioni dei corpi del sistema in un dato istante;
 * - acc : puntatore a cui assegnare le accelerazioni in SPATIAL_DIM dimensioni dei corpi del sistema in un dato istante.
 *
 * NOTA : le accelerazioni sono calcolate solo prima di stampare nei file di output.
 */
//VAL -2 la struct non è stata utilizzata al pieno delle sue possibilità: cioè come argomento 
// del  velocity verlet, delle energie etc.
typedef struct
{
    int nBodies;
    long double G;
    long double dt;
    int tdump;
    long int T;
    long double *masses;
    long double *coord;
    long double *vel;
    long double *acc;
} PhysicalSystem;

int read_input(FILE *inFile, PhysicalSystem *system);
void grav_force(const long double *coord, const long double *masses, const long double G, const int nBodies, long double *force);
long double Ekin(const long double *vel, const long double *masses, const int nBodies);
long double Epot(const long double *coord, const long double *masses, const long double G, const int nBodies);
void print_header(FILE *outFile, const PhysicalSystem *system, char *format);
void print_system(FILE *outFile, const PhysicalSystem *system);
void print_energies(FILE *outFile, const PhysicalSystem *system);
void free_struct_pointers(PhysicalSystem *system);

int main(int argc, char const *argv[])
{
    FILE *inFile;
    int ans;

    //VAL -0.5 ma perchè fate questa cosa? se davvero volevate un puntatore
    // a una struct di tipo PhysicalSystem bastava fare
    //
    // PhysicalSystem system;
    //
    // e usare &system quando serve.
    PhysicalSystem *system = (PhysicalSystem *)malloc(sizeof(PhysicalSystem));
    system->nBodies = -1;
    system->G = -1.L;
    system->dt = -1.L;
    system->tdump = -1;
    system->T = -1;
    system->masses = NULL;
    system->coord = NULL;
    system->vel = NULL;

#ifdef FUNNY
    srand(time(NULL));
#endif

    // errore in caso non sia stato letto alcun file in input
    if (argc < 2)
    {
        fprintf(stderr, "\nNon è stato specificato un file di input.\n\n");
        free_struct_pointers(system);
        return 1;
    }

    inFile = fopen(argv[1], "r");

    // errore in caso ci siano stati problemi nell'apertura del file
    if (!inFile)
    {
        fprintf(stderr, "\nImpossibile aprire il file: %s\n\n", argv[1]);
        free_struct_pointers(system);
        return 1;
    }

    // ciclo per la lettura dei dati di input dal file fornito in esecuzione
    while ((ans = read_input(inFile, system)) != -1)
    {
        if (ans == -2)
        {
            fprintf(stderr, "\nErrore nella lettura del file in input: il file deve contenere nBodies, G, dt, tdump e T positivi.\n\n");

            fclose(inFile);

            free_struct_pointers(system);
            return 1;
        }
    }

    fclose(inFile);

    FILE *outSystem;
    FILE *outEnergies;
    outSystem = fopen(OUTPUT_SYSTEM, "w");
    outEnergies = fopen(OUTPUT_ENERGIES, "w");

    if (!outSystem || !outEnergies)
    {
        fprintf(stderr, "\nErrore nell'apertura dei file di output\n\n");

        // "Chiudere" un puntatore null è undefined behaviour, per questo c'è questo controllo
        if (outSystem)
        {
            fclose(outSystem);
        }
        if (outEnergies)
        {
            fclose(outEnergies);
        }

        free_struct_pointers(system);
        return 1;
    }

    system->acc = (long double *)malloc(system->nBodies * SPATIAL_DIM * sizeof(long double));
    long double *force, *f_o = NULL;
    force = (long double *)malloc(system->nBodies * SPATIAL_DIM * sizeof(long double));

    if (!system->acc || !force)
    {
        fprintf(stderr, "\nErrore nell'allocazione dinamica della memoria.\n\n");

        fclose(outSystem);
        fclose(outEnergies);

        free_struct_pointers(system);
        free(force); // Liberato in caso l'allocazione fallita sia quella di system->acc
        return 1;
    }

    // calcolo la forza iniziale per ottenere l'accelerazione da stampare nell'istante iniziale
    grav_force(system->coord, system->masses, system->G, system->nBodies, force);

    // stampa dell'header nei due file di output
    print_header(outSystem, system, "system");
    print_header(outEnergies, system, "energies");

    // ciclo generale che stampa nei file di output ogni "system.tdump" integrazioni
    // NOTA: non serve verificare l'overflow perché questa divisione ritorna un numero minore di system->T, non maggiore.
    long int totPrint = (long int)(system->T / system->tdump);
    for (long int i = 0; i < totPrint; i++)
    {
        for (int j = 0; j < system->nBodies; j++)
        {
            for (int k = 0; k < SPATIAL_DIM; k++)
            {
                system->acc[k + SPATIAL_DIM * j] = force[k + SPATIAL_DIM * j] / system->masses[j];
            }
        }

        print_system(outSystem, system);
        print_energies(outEnergies, system);

        for (int j = 0; j < system->tdump; j++)
        {
            int resultCode = velverlet_ndim_npart(system->dt, system->G, system->nBodies, SPATIAL_DIM, system->masses, system->coord,
                                                  system->vel, force, &f_o, &grav_force);
            if (resultCode == -1)
            {
                fclose(outSystem);
                fclose(outEnergies);

                free_struct_pointers(system);
                free(force);
                free(f_o);
                return 1;
            }
        }
    }

    fclose(outSystem);
    fclose(outEnergies);

    free_struct_pointers(system);
    free(force);
    free(f_o);

    return 0;
}

/**
 * Funzione che legge i dati di input a partire dal file fornito in esecuzione (ogni volta che viene chiamata legge una riga).
 *
 * @param inFile Puntatore al file fornito in esecuzione.
 * @param system Puntatore alla struct contenente i dati relativi al sistema fisico considerato.
 *
 * @return -1 per End of File; -2 in caso di errore; 0 di default.
 */
int read_input(FILE *inFile, PhysicalSystem *system)
{
    char line[MAX_LEN], str[5], var[6];
    //VAL -0.5 non state controllando di aver letto la riga intera entro MAX_LEN caratteri
    if (!fgets(line, MAX_LEN, inFile))
    {
        return -1;
    }

    static int readHeadersCounter = 0;

    /*
    Serie di controlli che cerca nBodies, G, dt, tdump e T nell'header e che esclude eventuali commenti.

    Abbiamo optato per questa versione non molto elegante perché, anche utilizzando un vettore contenente i nomi degli header
    e un vettore in cui inserire i valori letti nelle posizioni corrispondenti, ad un certo punto si sarebbe dovuto assegnare
    i valori ricavati alla struct con un codice simile. La nostra versione è inoltre più facilmente mantenibile: aggiungendo un header
    basterebbe aggiungere un if qui e aggiornare la macro N_HEADERS, mentre, nel caso alternativo spiegato sopra, oltre alla macro
    bisognerebbe anche modificare il vettore con i nomi degli header e la funzione che assegna le variabili alla struct.
    Quell'approccio alternativo avrebbe potuto creare problemi anche nel casting da long double a integer delle variabili intere lette,
    con il rischio di troncamenti sconvenienti.
    */
    if (line[0] == '#')
    {
        sscanf(line, "%4s", str);
        if (strncmp(str, "#HDR", 4) == 0)
        {
            long int intRead = -1;
            long double doubleRead = -1.L;
            sscanf(line, "%*s %s", var);

            sscanf(line, "%*s %*s %Lf", &doubleRead);
            if (doubleRead <= 0)
                return -2;

            if (strncmp(var, "G", 1) == 0 && system->G < 0)
            {
                system->G = doubleRead;
                readHeadersCounter++;
                return 0;
            }
            else if (strncmp(var, "dt", 2) == 0 && system->dt < 0)
            {
                system->dt = doubleRead;
                readHeadersCounter++;
                return 0;
            }

            // Se si arriva qui allora il valore atteso è un numero intero, quindi si può controllare che sia maggiore di 0
            // se si fosse fatto prima allora sarebbe potuto essere 0 in caso fosse un double minore di 1 per via di troncamento
            sscanf(line, "%*s %*s %ld", &intRead);

            if (intRead <= 0)
                return -2;

            if (strncmp(var, "N", 1) == 0 && system->nBodies < 0)
            {
                system->nBodies = intRead;
                readHeadersCounter++;
            }
            else if (strncmp(var, "tdump", 5) == 0 && system->tdump < 0)
            {
                system->tdump = intRead;
                readHeadersCounter++;
            }
            else if (strncmp(var, "T", 1) == 0 && system->T < 0)
            {
                system->T = intRead;
                readHeadersCounter++;
            }
        }
        return 0;
    }

    // controllo che siano stati letti i dati necessari per l'esecuzione del programma
    if (readHeadersCounter != N_HEADERS)
    {
        return -2;
    }

    int nChar, nTotChar, bodyNumber;

    // I puntatori nella struct sono inizializzati soltanto quando sono NULL, quindi una volta per esecuzione del programma.
    if (!system->masses)
    {
        system->masses = (long double *)malloc(system->nBodies * sizeof(long double));
    }

    if (!system->coord)
    {
        system->coord = (long double *)malloc(system->nBodies * SPATIAL_DIM * sizeof(long double));
    }

    if (!system->vel)
    {
        system->vel = (long double *)malloc(system->nBodies * SPATIAL_DIM * sizeof(long double));
    }

    if (!system->masses || !system->coord || !system->vel)
    {
        fprintf(stderr, "\nErrore nell'allocazione dinamica della memoria.\n\n");
        return -2;
    }

    // lettura del numero di corpo di cui si stanno leggendo posizioni e velocità di partenza
    sscanf(line, "%d %n", &bodyNumber, &nTotChar);

    // lettura della massa del corpo specificato da bodyNumber
    sscanf(line + nTotChar, "%Lf %n", system->masses + (bodyNumber - 1), &nChar);
    nTotChar += nChar;

    // ciclo per la lettura della posizione di partenza del corpo specificato da bodyNumber
    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        sscanf(line + nTotChar, "%Lf %n", system->coord + i + SPATIAL_DIM * (bodyNumber - 1), &nChar);
        nTotChar += nChar;
    }

    // ciclo per la lettura della velocità di partenza del corpo specificato da bodyNumber
    for (int i = 0; i < SPATIAL_DIM; i++)
    {
        sscanf(line + nTotChar, "%Lf %n", system->vel + i + SPATIAL_DIM * (bodyNumber - 1), &nChar);
        nTotChar += nChar;
    }

    return 0;
}

/**
 * Funzione che, date le posizioni di un numero di corpi specificato in un dato istante, calcola le forze gravitazionali
 * agenti tra questi nel dato istante.
 *
 * @param coord Puntatore al vettore di long double contenente le posizioni dei corpi un corpo alla volta: x11, x12, ..., x21, ...
 * @param masses Puntatore al vettore di long double contenente le masse dei corpi nel sistema.
 * @param G Costante di gravitazione considerata per il calcolo della forza gravitazionale.
 * @param nBodies Numero di corpi che compongono il sistema considerato.
 * @param force Puntatore al vettore di long double in cui salvare la risultante delle forze su ciascun corpo.
 */
void grav_force(const long double *coord, const long double *masses, const long double G, const int nBodies, long double *force)
{
    long double forceComp, d;
    long double vec_d[SPATIAL_DIM];

    for (int i = 0; i < SPATIAL_DIM * nBodies; i++)
    {
        force[i] = 0.L;
    }

    for (int i = 0; i < nBodies; i++)
    {
        for (int j = i + 1; j < nBodies; j++)
        {
            vec_diff((coord + i * SPATIAL_DIM), (coord + j * SPATIAL_DIM), vec_d, SPATIAL_DIM);
            d = dist((coord + i * SPATIAL_DIM), (coord + j * SPATIAL_DIM), SPATIAL_DIM);

            for (int k = 0; k < SPATIAL_DIM; k++)
            {

                forceComp = -G * masses[i] * masses[j] * vec_d[k] / pow(d, 3);
                *(force + k + i * SPATIAL_DIM) += forceComp;
                *(force + k + j * SPATIAL_DIM) -= forceComp;
            }
        }
    }
}

/**
 * Funzione che calcola l'energia cinetica del sistema di un numero di corpi pari a nBodies.
 *
 * @param vel Puntatore al vettore di long double contenente le velocità dei corpi.
 * @param masses Puntatore al vettore di long double contenente le masse dei corpi.
 * @param nBodies Numero intero del numero di corpi del sistema.
 *
 * @return Valore long double dell'energia cinetica.
 */

long double Ekin(const long double *vel, const long double *masses, const int nBodies)
{
    long double kinEnergyTot = 0.L;

    for (int i = 0; i < nBodies; i++)
    {
        kinEnergyTot += 0.5L * masses[i] * scal(vel + SPATIAL_DIM * i, vel + SPATIAL_DIM * i, SPATIAL_DIM);
    }

    return kinEnergyTot;
}

/**
 * Funzione che calcola l'energia potenziale del sistema di un numero di corpi pari a nBodies.
 *
 * @param coord Puntatore al vettore di long double contenente le coordinate spaziali dei corpi.
 * @param masses Puntatore al vettore di long double contenente le masse dei corpi.
 * @param G costante di gravitazione considerata.
 * @param nBodies Numero intero del numero di corpi del sistema.
 *
 * @return Valore long double dell'energia potenziale
 */
long double Epot(const long double *coord, const long double *masses, const long double G, const int nBodies)
{
    long double potEnergyTot = 0.L;
    for (int i = 0; i < nBodies; i++)
    {
        for (int j = i + 1; j < nBodies; j++)
        {
            long double distance = dist(coord + j * SPATIAL_DIM, coord + i * SPATIAL_DIM, SPATIAL_DIM);
            potEnergyTot += -G * masses[i] * masses[j] / distance;
        }
    }

    return potEnergyTot;
}

/**
 * Funzione che stampa l'header per i file di output.
 *
 * @param outFile Puntatore al file di output.
 * @param system Puntatore alla struct contenente le variabili relative al sistema.
 * @param format Stringa indicante il tipo di format scelto ("system" o "energies" a seconda che si vogliano stampare la traiettoria
 * del sistema o le energie del sistema).
 */
void print_header(FILE *outFile, const PhysicalSystem *system, char *format)
{
#ifdef FUNNY
    char *quotes[N_QUOTES] =
        {"Above all, don't fear difficult moments. The best comes from them.  (Rita Levi-Montalcini)",
         "In physics, you don't have to go around making trouble for yourself. Nature does it for you.  (Frank Wilczek)",
         "If you thought that science was certain - well, that is just an error on your part.  (Richard P. Feynman)",
         "In science there is only physics; all the rest is stamp collecting.  (Lord Kelvin)",
         "Nothing happens until something moves.  (Albert Einstein)",
         "Not only is the Universe stranger than we think, it is stranger than we can think.  (Werner Heisenberg)",
         "Physics is the only profession in which prophecy is not only accurate but routine.  (Neil deGrasse Tyson)"};

    int r = rand() % N_QUOTES;

    fprintf(outFile, "#%s\n", quotes[r]);
#endif

    fprintf(outFile, "#HDR N\t%d\n", system->nBodies);
    fprintf(outFile, "#HDR G\t%Lf\n", system->G);
    fprintf(outFile, "#HDR m\t");
    for (int i = 0; i < system->nBodies; i++)
    {
        fprintf(outFile, "%Lf ", system->masses[i]);
    }
    fprintf(outFile, "\n");

    // controlli che scrivono il format dei dati nel file
    fprintf(outFile, "#format:\t ");
    if (strncmp(format, "system", 6) == 0)
    {
        fprintf(outFile, "time\t coords: (");
        for (int i = 0; i < SPATIAL_DIM; i++)
        {
            fprintf(outFile, " x%d", i);
        }
        fprintf(outFile, ")\t velocities: (");
        for (int i = 0; i < SPATIAL_DIM; i++)
        {
            fprintf(outFile, " v%d", i);
        }
        fprintf(outFile, ")\t accelerations: (");
        for (int i = 0; i < SPATIAL_DIM; i++)
        {
            fprintf(outFile, " a%d", i);
        }
        fprintf(outFile, ")\n");
    }
    else if (strncmp(format, "energies", 8) == 0)
    {
        fprintf(outFile, "kinetic energy\t potential energy\t total energy\n");
    }
    else
    {
        fprintf(stderr, "\nFormato specificato non supportato. La riga di formato verrà lasciata vuota.\n\n");
    }
}

/**
 * Funzione che date le condizioni del sistema in un dato istante, stampa le posizioni, le velocità e le accelerazioni
 * del dato istante nel file specificato in outFile.
 *
 * @param outFile Puntatore al file in cui stampare posizioni, velocità e accelerazioni del sistema.
 * @param system Puntatore alla struct contenente tutte le variabili in gioco nel sistema.
 */
void print_system(FILE *outFile, const PhysicalSystem *system)
{
    static double t = 0.;

    fprintf(outFile, "%lf ", t);

    for (int i = 0; i < system->nBodies * SPATIAL_DIM; i++)
    {
        fprintf(outFile, "%.16Lf ", system->coord[i]);
    }

    for (int i = 0; i < system->nBodies * SPATIAL_DIM; i++)
    {
        fprintf(outFile, "%.16Lf ", system->vel[i]);
    }

    for (int i = 0; i < system->nBodies * SPATIAL_DIM; i++)
    {
        fprintf(outFile, "%.16Lf ", system->acc[i]);
    }

    fprintf(outFile, "\n");

    t++;
}

/**
 * Funzione che, date le condizioni del sistema in un dato istante, calcola energia cinetica, potenziale e totale nel dato istante.
 * Stampa poi tutto nel file specificato in outFile.
 *
 * @param outFile Puntatore al file in cui stampare energia cinetica, potenziale e totale del sistema in un dato istante.
 * @param system Puntatore alla struct contenente tutte le variabili in gioco nel sistema.
 */
void print_energies(FILE *outFile, const PhysicalSystem *system)
{
    long double kEnergy, potEnergy, totEnergy;

    kEnergy = Ekin(system->vel, system->masses, system->nBodies);
    potEnergy = Epot(system->coord, system->masses, system->G, system->nBodies);
    totEnergy = kEnergy + potEnergy;

    fprintf(outFile, "%16.9Lf %16.9Lf %16.9Lf\n", kEnergy, potEnergy, totEnergy);
}

/**
 * Funzione che libera tutti i puntatori della struct PhysicalSystem passata in input e poi il puntatore alla struct stessa.
 *
 * @param system Puntatore alla struct PhysicalSystem da liberare.
 */
void free_struct_pointers(PhysicalSystem *system)
{
    free(system->masses);
    free(system->coord);
    free(system->vel);
    free(system->acc);
    free(system);
}
