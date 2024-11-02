+++
title="Mod 2: Program Interaction"
+++

## Appunti lezioni

{{% resources pattern="files/" /%}}

### Linux command line

__TODO:__

### Binary Files

Executable and Linkable Format: definisce la forma che deve avere un binario per poter essere eseguito su linux.
[Bel blogpost](https://www.intezer.com/blog/research/executable-linkable-format-101-part1-sections-segments/)

Un file ELF è divisibile in __segmenti__. questi definiscono dove le varie parti del binario verranno caricate in memoria.

Ortogonalmente alla divisione in segmenti, è possibile anche dividere il binario in __sezioni__, queste definiscono la semantica delle parti del binario.
A noi interesseranno le seguenti sezioni:

* `.text`: sezione eseguibile del binario
* `.plt`, `.got`: sezione in cui verranno caricate le librerie dinamiche utilizzate dal binario
* `.data`: variabili dichiarate a compile time
* `.rodata`: costanti a compile time (tipo le stringhe)
* `.bss`: variabili dichiarate a compile time ma non inizializzate

Per interagire con l'ELF, possiamo utilizzare i seguenti tools:

* `gcc` to make your ELF.
* `readelf` to parse the ELF header.
* `objdump` to parse the ELF header and disassemble the source code.
* `nm` to view your ELF's symbols.
* `patchelf` to change some ELF properties.
* `objcopy` to swap out ELF sections.
* `strip` to remove otherwise-helpful information (such as symbols).
* [`kaitai struct`](https://ide.kaitai.io/) to look through your ELF interactively.

### Linux Process Loading

__TODO:__

### Linux Process Execution

__TODO:__

## Materiale aggiuntivo

[pwntools](https://github.com/Gallopsled/pwntools): libreria molto utile per semplificare l'interazione con i processi a runtime.
__QUESTO VA IMPARATO BENE__, leggere almeno una volta line by line il [tutorial](https://github.com/Gallopsled/pwntools-tutorial#readme)

## Challenges

### Opinioni generali

__TODO:__ scrivere opinioni una volta finite le challenges

### embryoio 1-28

flag gratis, ti chiede di fare cose su:

* stdin/stdout
* argomenti
* variabili di ambiente
  * `env -i` per azzerare l'ambiente
  * dalla manpage di `bash`, sezione `exec`: <br> `The -c option      causes command to be executed with an empty environment.`
* redirezione
* lanciare script bash/python

### embryoio 29-85,88-97,102-105,107,110,115-123

qui inizia a chiederti di generare nuovi processi (fork), gestire redirezione con pipe e fifo, argv, env e segnali inizi a dover scrivere qualche riga di C.
Le challenge sono speculari in C, bash e python.

* La chall 66 chiede di utilizzare `find`, questa è interessante (anche se molto facile)
* La chall 73, 79, 85 chiede roba sulla CWD (working dir) tra padre e figlio: __TODO:__ check
* La chall 94 chiedere di avere come input il file descriptor 283. Questo e\` facile da fare in bash (__TODO:__ provare a rifarla in sh posix):

```
exec 283< tmp
/challenge/embryoio_level94 <&283
```

Questa soluzione funziona sono in bash, non is shell posix. La motivazione la troviamo [qui](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_07):
> Open files are represented by decimal numbers starting with zero.
> The largest possible value is implementation-defined; however, all implementations shall support at least 0 to 9, inclusive, for use by the application.
> These numbers are called "file descriptors".

Quindi, il file descriptor 283 e\` accettato dallo standard ma non implementato in diverse shell posix (tipo dash).

#### Ecco le informazioni utili per svolgere questi esecizi

* un processo figlio eredita dal padre i file descriptor
  * i due processi condividono anche la posizione nel file (fseek).<br> __TODO:__ cercare docs su sta cosa
* il figlio passa a zombie (aka orfani, con padre init) se il padre vero muore
* i figli ereditano l'environment dal padre
* ricorda che il programma per mandare un segnale ad un processo e\` `kill`

### embryoio 86,87,99,100,106,112,113,126-142

qui bisogna comunicare con il processo che sta girando.
Per comunicare, ti chiede di utilizzare vari medodi, quali:

* stdin/stdout
* fifo
* tcp
* (forse altro)
In pratica sono date dal sw di checking delle operazioni matematiche da completare, e lo scopo e\` quello di risolverle programmaticamente.
Un'altra sottocategoria, sempre simile, consiste nel dover programmaticamente mandare n segnali, scelti dinamicamente dalla challenge al processo figlio.
Anche questi sono divisi tra bash *, C e python*, la soluzione per comunicare interattivamente e\`:
* bash: __TODO:__ capire come si fa (maybe <http://tiswww.case.edu/php/chet/bash/bashref.html#Coprocesses>)
* C: semplice ciclo consumer con pipe. `man pipe` contiene un esempio
* python: pwntools e\` fatto apposta
