+++
title = "15 Novembre 2024"
date = 2024-11-15
draft = false
publishdate = 2024-11-14
author = "Renato"
description = "Buffer Overflow, C internals e pwntools"
+++


## Preambolo

cosa ci serve:

- `gcc`
- `gdb`
- `python`
- `pwntools`
- conoscenza basilare di C
- voglia di imparare

Tutti i tool sono disponibili di default su Kali.

Su Ubuntu si installano con :

```
sudo apt install build-essential gdb python3 python3-pip python3-dev git libssl-dev libffi-dev
pip install --break-system-packages pwntools
```

## ma come funziona veramente C?

Ogni problema si può comprendere su diversi livelli di astrazione. Solitamente, ragioniamo su un programma C (o qualsiasi altro linguaggio) al livello del linguaggio stesso.
Da questo punto di vista il programma non è altro che un insieme di variabili e funzioni, e tutta l'esecuzione parte dal `main`. Sappiamo però che ciò che esegue il nostro computer è un binario generato dal compilatore e dal linker -> **cosa succede tra un' astrazione e l'altra?**

### Digging deeper

Senza entrare nelle specifiche, concettualmente il nostro computer funziona in modo "lineare" consuma un "nastro" di codice, eseguendone le istruzioni.
Questo differisce abbastanza dalla visione strutturata di un programma C, dove abbiamo diverse funzioni innestate che si chiamano fra di loro (o se stesse)

### cos'è veramente una funzione?

Non esiste un concetto nativo di funzione per il computer esegue un programma : il programma è semplicemente un insieme (tendenzialmente) monolitico di istruzioni, da caricare in memoria (il nastro).

A un alto livello, cosa serve per implementare una funzione?

**Dati** :

- Un modo di comunicare degli _argomenti_ -> li carico (in memoria) da qualche parte
- Un modo di comunicare il _risultato_ -> lo carico da qualche parte...

**Metadati** :

- Un modo di identificare una funzione -> un indirizzo in memoria
- E infine... un modo di tornare alla funzione precedente! -> lo salvo prima della chiamata

D'ora in poi considereremo nello specifico l'architettura x86_32, i cui eseguibili sono facili da eseguire sulla maggior parte dei computer moderni

### un semplice programma-cavia

Prendiamoci un attimo di pausa dalla teoria per analizzare un semplice programma C che prende input dall'utente con la `gets`:

```c
#include <stdio.h>

void impossibile_da_chiamare() {
    printf("Come hai fatto???\n");
}

void funzione_normalissima() {
    char buffer[64];
    printf("Come ti chiami? ");
    gets(buffer);  // Non usarmi!
    printf("Ciao %s!!\n", buffer);
}

int main() {
    printf("Qualche professore amerebbe questo programma\n");
    funzione_normalissima();
    return 0;
}
```

Se provate a compilare questo programma normalmente, il compilatore si arrabbierà (con buone ragioni). **Perchè**?

{{% resources pattern="ex1-gets/" /%}}

### che succede se passo più di 64 caratteri?

Facciamolo!

A prima vista, il programma sembra crashare, **perchè**? Cos'altro può succedere?

### strumenti utili

- `gdb` : debugger, ci aiuterà a capire meglio cosa fa il nostro programma e a capire perchè crasha
- `pwntools` : libreria di python, ci permette di interagire in modo programmatico con il nostro binario

Entrambi installati di default su Kali!

### cosa sta succedendo?

Aprendo il programma con gdb ed eseguendolo vediamo qualcosa del genere:

```
Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
```

Il programma ha provato ad eseguire una "funzione" all'indirizzo 0x41414141. è un caso?

"AAAA", codificato come indirizzo è proprio 0x41414141 -> abbiamo sovrascritto qualcosa di importante con il nostro input

### come sono implementate le chiamate a funzione in x86_32?

Non entreremo troppo nei dettagli (layout dello stack e memoria), ciò che ci basta sapere per ora è questo:

- è che i _metadati_ di una chiamata sono conservati in modo adiacente ai _dati_ -> per ora immaginiamoci che vengano direttamente dopo in memoria
- accedere ad un elemento in un array (array[i]) vuol dire accedere ad un elemento in un indirizzo (array + i * sizeof(type)) -> buffer[65] ad ex. è un'espressione sensata in questo contesto

Possiamo sovrascrivere l'indirizzo di ritorno -> possiamo eseguire una funzione qualsiasi!

### cosa eseguiamo?

Sappiamo che una funzione è identificata da un indirizzo in memoria -> come troviamo quello di `impossibile_da_chiamare`?

Riapriamo gdb:

```
x *impossibile_da_chiamare
```

NB : per motivi di sicurezza di solito gli indirizzi di memoria cambiano tra un esecuzione e l'altra (ASLR). Possiamo disabilitare questa feature su Linux con:

```
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

### come scriviamo un indirizzo arbitrario in input?

Non tutti i byte dell'indirizzo corrispondono a caratteri ASCII. A prima vista dal terminale non riesco a codificare l'indirizzo!

In realtà un `char` può avere qualsiasi valore da 0 a 255, basta riuscire a mandarlo in input -> `pwntools`

```
r.sendline(b"A"*64 + b"quello che voglio \x01\x02\x03")
```

Non mi rimane che mandare l'indirizzo giusto. Alcune sfide:

- Dopo quanti caratteri lo metto?
- Come lo codifico?
- **Cosa altro posso combinare?**
