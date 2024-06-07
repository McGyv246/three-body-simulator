# Struttura codice
In geom.c abbiamo inserito le funzioni geometriche ved_diff, dist e scal.
In integrator.c abbiamo inserito soltanto l'implementazione di velocity verlet.
In main.c abbiamo inserito la lettura dell'input, la scrittura dell'output, le funzioni di calcolo della forza e dell'energia e il codice che orchestra l'esecuzione di tutto il programma.


# Note generali
Il programma è stato strutturato per funzionare ad un numero di corpi arbitrario (specificato nel file di input al programma) e anche in un numero di dimensioni arbitrario (modificabile tramite la macro SPATIAL_DIM).

## Utilizzo malloc e vettori
Visto che le funzioni geometriche, quelle dell'energia e quelle dell'integrazione vengono chiamate moltissime volte abbiamo evitato di utilizzare al loro interno malloc, dato che meno efficiente di array nello stack o array preallocati. Inoltre in molti casi abbiamo evitato l'uso di array nello stack per non rischiare di incorrere in stack overflow (che avverrebbe all'aumentare di corpi e/o dimensioni). Per questo tutte queste funzioni inseriscono in dei vettori passati in input il loro output. Ci siamo limitati ad utilizzare malloc per allocare i vettori necessari all'esecuzione del programma (una sola volta per vettore).

## Tipo di dato utilizzato
Abbiamo deciso di utilizzare long double al posto di double per attenuare la fluttuazione sulle ultime cifre decimali stampate dell'energia totale (con 9 cifre decimali di solito non cambia neanche l'ultima cifra).
Abbiamo però riscontrato un problema: questo funziona soltanto su windows con wsl (nello specifico con processori x86) e non MacOS con processori Arm64. Questo perché il mentre con x86 long double è di 128 bit, con Arm64 long double è di 64 bit, esattamente come double normale (si noti che con alcuni processori long double è a 80 bit).

Per verificare si provi ad eseguire il seguente codice:
printf("%lu  %lu\n", sizeof(double), sizeof(long double));

In base a quanto trovato su internet l'unico modo per aggirare questa limitazione sarebbero delle librerie che fanno simulazioni lato software, però molto più lente rispetto al supporto nativo da parte dell'hardware.

La differenza purtroppo è notevole. Utilizzando per esempio input_2.dat con long double su x86 si ottiene che la prima energia totale stampata è -2.085220066, mentre l'ultima (alla riga 100005 del file di output) è -2.085220066 (uguale), invece, su Arm64, la prima è sempre -2.085220066, mentre l'ultima è -2.085220034 (le ultime 2 cifre significative sono cambiate).


# Note su integrator.c
Abbiamo scritto l'implementazione di velocity verlet in modo che funzioni nel numero specificato di dimensioni per il numero specificato di corpi contemporaneamente.
Strutturarla così (rendendola un po' più specifica del velocity verlet generico, ma comunque generale per sistemi ad nBodies particelle) ha permesso di operare ottimizzazioni nel calcolo della forza (si legga la sezione sul main.c per spiegazione più dettagliata).

Un'altra importante ottimizzazione (sia nella leggibilità del codice che nella performance) è nel calcolo delle posizioni. Aggiornando le posizioni un corpo alla volta avrebbe causato problemi nel calcolo della forza applicata ai corpi successivi, il primo sarebbe stato nell'istante t + dt, gli altri nell'istante t, questo avrebbe prodotto risultati sbagliati.
Invece di tenere una copia del vettore posizioni, e fare giri poco chiari all'interno del main, per evitare questo problema (che peraltro è comune a ogni tipo di sistema di particelle) abbiamo deciso di fare il calcolo per tutti i corpi contemporaneamente, calcolando la forza per tutti i corpi una volta sola all'inizio e aggiornando tutte le posizioni di conseguenza.

Ultima ottimizzazione è stata quella di usare una variabile cache per la forza del giro precedente da utilizzare come forza vecchia senza doverla calcolare 2 volte ogni giro. Questa variabile è necessariamente esterna in quanto se si fosse dichiarata statica non si sarebbe potuta liberare (viene allocata con malloc alla prima esecuzione dato che usare VLA non è sicuro).

Inoltre abbiamo scritto tutti i parametri necessari e non abbiamo passato la struct physical system perché avrebbe aumentato di
molto il coupling tra questa funzione e il file main.
Riconosciamo che la firma della funzione sarebbe stata decisamente più leggibile utilizzando la struct ma dal punto di vista di design non era giustificato. Inoltre in questo modo è molto più chiaro quali parametri vengano modificati dalla funzione e quali siano soltanto un input costante.


# Note su main.c
Le funzioni grav_force, Ekin ed Epot non prendono la struct physicalSystem come parametro perché abbiamo ritenuto che le rendesse più generali e riutilizzabili in altri contesti, oltre a rendere molto più chiaro che cosa viene utilizzato per i calcoli della funzione (cosa che sarebbe nascosta utilizzando la struct).
In particolare la funzione grav_force va passata in input alla funzione velverlet_ndim_npart e quindi deve rispettare l'interfaccia lì definita per il puntatore a funzione (si legga il commento sopra per chiarimenti sul perché non prende come parametro la struct physicalSystem).

Il calcolo della forza è stato ottimizzato evitando di calcolare due volte la forza tra gli stessi due corpi. Questo è stato ottenuto salvando nel vettore delle forze nelle posizioni corrispondenti ai due corpi presi in esame la forza ottenuta tra i 2, una positiva e una negativa (dato che cambia il verso). Questo riduce notevolmente il numero di calcoli necessari, in particolare all'aumentare dei corpi considerati.
