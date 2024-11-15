+++
title = "15 Novembre 2024"
date = 2024-11-15
draft = false
publishdate = 2024-11-14
author = "Renato"
description = "Buffer Overflow, C internals e pwntools"
+++

## Preambolo

Durante questo incontro vedremo come funziona un buffer overflow, come sfruttarlo e una breve introduzione a pwntools.

Cosa ci serve:

- `gcc`
- `gdb`
- `python`
- `pwntools`
- conoscenza basilare di C
- voglia di imparare

Tutti i tool sono disponibili di default su Kali.

Su Ubuntu si installano con :

```bash
$ sudo apt install build-essential gdb python3 python3-pip python3-dev git libssl-dev libffi-dev gcc-multilib
```

```bash
$ pip install --break-system-packages pwntools
```

> [!TIP] Comandi shell
> Di solito i comandi shell nelle varie risorse online sono preceduti da un `$`, che rappresenta il prompt del terminale.
> Questo viene usato per:
>
> - indicare che il comando va eseguito come **utente normale** (ovvero non come root).
> - rendere il copia e incolla più difficile e costringere a modificare il comando prima di eseguirlo, riducendo il rischio di eseguire comandi pericolosi per errore.

## Buffer Overflow

Ogni problema si può comprendere su diversi livelli di astrazione. Solitamente, ragioniamo su un programma C (o qualsiasi altro linguaggio) al livello del linguaggio stesso.
Da questo punto di vista il programma non è altro che un insieme di variabili e funzioni, e tutta l'esecuzione parte dal `main`. Sappiamo però che ciò che esegue il nostro computer è un binario generato dal compilatore e dal linker -> **cosa succede tra un' astrazione e l'altra?**

### Digging deeper

Senza entrare nelle specifiche, concettualmente il nostro computer funziona in modo "lineare" consuma un "nastro" di codice, eseguendone le istruzioni.
Questo differisce abbastanza dalla visione strutturata di un programma C, dove abbiamo diverse funzioni innestate che si chiamano fra di loro (o se stesse)

### Come funziona una funzione?

Oltre al gioco di parole, è importante capire come funziona una funzione a basso livello.

Non esiste un concetto nativo di funzione per il computer esegue un programma : il programma è semplicemente un insieme (tendenzialmente) monolitico di istruzioni, da caricare in memoria (il nastro).

A un alto livello, cosa serve per implementare una funzione?

**Dati** :

- Un modo di comunicare degli _argomenti_ -> li carico (in memoria) da qualche parte
- Un modo di comunicare il _risultato_ -> lo carico da qualche parte...

**Metadati** :

- Un modo di identificare una funzione -> un indirizzo in memoria
- E infine... un modo di tornare alla funzione precedente! -> lo salvo prima della chiamata

D'ora in poi considereremo nello specifico l'architettura x86_32, i cui eseguibili sono facili da eseguire sulla maggior parte dei computer moderni

### Un semplice programma-cavia

Per capire meglio come funzionano le chiamate a funzione, consideriamo il seguente programma:

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

Il programma è molto semplice, chiede il nostro nome e lo stampa a schermo. Da notare la funzione `gets`, che è **molto** pericolosa e non dovrebbe essere usata mai in un programma reale.

Se provate a compilare questo programma normalmente, il compilatore si arrabbierà (con buone ragioni). **Perchè**?

```bash
$ gcc dim.c

dim.c: In function ‘funzione_normalissima’:
dim.c:10:5: error: implicit declaration of function ‘gets’; did you mean ‘fgets’? [-Wimplicit-function-declaration]
   10 |     gets(buffer);  // Non usarmi!
      |     ^~~~
      |     fgets
```

{{% resources pattern="ex1-gets/" style="primary" /%}}

### 1. Che succede se passo più di 64 caratteri?

A prima vista, il programma sembra funzionare correttamente. Notiamo però che la variabile `buffer` è grande solo 64. Cosa succede se passiamo più di 64 caratteri?

Facciamolo!

```shell
$ ./dim
Qualche professore amerebbe questo programma
Come ti chiami? AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Ciao AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!
Errore di segmentazione (core dump creato)
```

Il programma crasha! Perchè?

### 1.a Strumenti utili

- `gdb` : debugger, ci aiuterà a capire meglio cosa fa il nostro programma e a capire perchè crasha
- `pwntools` : libreria di python, ci permette di interagire in modo programmatico con il nostro binario

Entrambi installati di default su Kali!

### 2. Cosa sta succedendo?

Proviamo ad analizzare il problema con `gdb`:

```shell
$ gdb ./dim
(gdb) run
```

Inseriamo il nostro input e vediamo cosa succede:

```
Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
```

L'errore di segmentazione (SIGSEG) è il più classico, ma cosa significa `0x41414141 in ??`?

Normalmente `gdb` ci mostra sia l'indirizzo di memoria in cui si è verificato l'errore, sia la funzione in cui si è verificato.

- `0x41414141` è un indirizzo di memoria, ma cosa rappresenta?
- `??` indica che `gdb` non sa cosa ci sia in quell'indirizzo di memoria

Il programma ha quindi provato ad eseguire una "funzione" all'indirizzo `0x41414141`. E' un caso?

Se proviamo a convertire `0x41414141` in ASCII otteniamo "AAAA", che è esattamente una parte del nostro input.

Possiamo quindi dedurre che **il programma ha provato ad accedere ad una cella di memoria che è stata sovrascritta con il nostro input**.

### 3. Come sono implementate le chiamate a funzione in x86_32?

Non entreremo troppo nei dettagli (layout dello stack e memoria), ciò che ci basta sapere per ora è questo: durante una chiamata a funzione, il programma salva i metadati della funzione chiamante nello stack, e poi salta (_jump_ incondizionato) alla funzione chiamata.

Lo stack è una struttura dati a pila, ovvero una struttura dati in cui l'ultimo elemento inserito è il primo ad essere rimosso, quindi, al momento di chiamare la funzione, i metadati della funzione chiamante si trovano in cima allo stack.

Durante l'inizializzazione della funzione chiamata, vengono pushati sullo stack i dati della funzione chiamata (ovvero le variabili locali) e poi viene eseguito il codice della funzione.

Quindi:

> I _metadati_ di una chiamata a funzione sono conservati in modo adiacente ai _dati locali_ della funzione chiamante;
  per ora immaginiamoci che siano situati negli indirizzi di memoria appena successivi.

Inoltre ricordiamoci che In C accedere ad un elemento in un array (`array[i]`) vuol dire accedere ad un elemento in un indirizzo = `array + i * sizeof(type)`.

Ad esempio, `buffer[65]` nonostante sia "fuori dal buffer" è un modo per accedere al 65° byte del buffer, che però non è del buffer stesso ma di un'altra variabile in memoria (o, in questo caso, dei metadati della funzione chiamante).

Utilizzando queste due informazioni, possiamo capire cosa è successo:

1. Abbiamo sovrascritto il buffer con più di 64 caratteri;
2. Abbiamo sovrascritto i metadati della funzione chiamata;
3. Nel momento in cui la funzione chiamata tenta di ritornare alla funzione chiamante (`return`), il program counter (PC) si sposta all'indirizzo che abbiamo scritto come 65° byte del buffer (in realtà non è detto sia proprio il 65° byte, in quanto il compilatore può inserire padding tra le variabili).

Possiamo quindi controllare il PC, ergo possiamo eseguire una funzione qualsiasi (con alcuni limiti, ma per ora non ci interessano)!

### 4. Cosa eseguiamo?

Proviamo, ad esempio, a chiamare la funzione `impossibile_da_chiamare`.

Sappiamo che ogni funzione è caricata in memoria ad un indirizzo specifico dello spazio di indirizzamento del processo
. Come facciamo a sapere quale è l'indirizzo di `impossibile_da_chiamare`?

Riapriamo `gdb`:

```
gdb ./a.out
```

e chiediamo l'indirizzo della funzione:

```
(gdb) x *impossibile_da_chiamare
```

oppure

```
(gdb) info functions
```

> [!WARNING] Address Space Layout Randomization
> Per motivi di sicurezza di solito gli indirizzi di memoria cambiano tra un esecuzione e l'altra (vedi _ASLR_).
> Possiamo però disabilitarlo temporaneamente con:
>
> ```bash
> $ echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
> ```

### 5. Come scriviamo un indirizzo arbitrario in input?

Da terminale non possiamo scrivere un indirizzo arbitrario, in quanto non possiamo scrivere caratteri non ASCII. Per questo è necessario un aiuto da parte di `python3`.

```shell
$ python3 -c "print(b'A'*64 + b'\x01\x02\x03\x04')" | ./dim
```

in questo modo stiamo passando al programma `dim` 64 caratteri `A` seguiti da `0x04030201` (in little-endian, ovvero con gli indirizzi in ordine inverso rispetto a come li scriviamo).

In alternativa, possiamo usare `pwntools`:

```python
r = process("./dim")
r.sendline(b"A"*64 + b"quello che voglio \x01\x02\x03")
r.interactive()
```

In sequenza:

- `r = process("./dim")` crea un sotto-processo eseguendo il programma `dim`;
- `r.sendline(b"A"*64 + b"quello che voglio \x01\x02\x03")` invia la stringa al programma seguita da un newline (`\n`);
- `r.interactive()` ci permette di interagire con il programma come se fosse un terminale.

Non mi rimane che mandare l'indirizzo giusto!

## Esercizio

Alcune sfide:

- Come faccio a trovare il numero di caratteri da scrivere prima dell'indirizzo? (padding)
- Come faccio a trovare l'indirizzo di una funzione nel caso io non abbia accesso al compilato?
- Cosa riesco a fare con questa tecnica?
- Cosa posso fare per evitare che il mio programma sia vulnerabile a questo tipo di attacco?
