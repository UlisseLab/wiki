+++
title = "Mod 3: Assembly Crash Course"
+++

## Appunti lezioni

{{% attachments /%}}

### Lezione 0: Computer Architecture

CPU composta da gate logici AND, OR, XOR e i corrispondenti negati -> prendono in input 0/1 e restituiscono un output che dipende dal tipo di gate usato.
Combianando tali componenti si generano reti logiche più complesse come ADDER, MUX, ecc.
La CPU è composta da registri (memoria), Control Unit (CU), Arithmetic Logic Unit (ALU).

### Lezione 1: Assembly

Rappresentazione del codice binario dato in pasto alla CPU -> Assembly (primo layer di astrazione creato sopra il codice binario).
L' assembly è strettamente correlato all'architetettura in cui si lavora, quella presa in analisi è x86, con sintassi Intel perchè quella AT&T fa schifo.

### Lezione 2: Data

I dati nei calcolatri sono rappresentati in binario(base2) (solo cifre (bit) 0/1), solitamente risulta semplice esprimere i numeri binari in basi fatte da potenze di 2, le più diffuse sono base 8 (ottale), base 16 (esadecimale).
Il testo viene anch'esso codificato in binario, ASCII -> codifica a 7 bit; UTF-8 -> codifica a 8 bit (98% del web è codificato così)
Ogni i bit vengono raggruppati nel seguente modo (storicamente):

* Nibble: 1/2 byte,4 bits
* Byte: 1byte, 8 bits
* Half Word (word): 2 bytes, 16 bits
* Double Word (dword): 4bytes, 32 bits
* Quad Word (qword): 8 bytes, 64 bits

__in realtà usiamo word per intendere anche la dimesione del bus dell'architettura (ie: 8 byte per amd64)__

Per quanto riguarda i numeri nell'architettura a 64 bit ne possiamo esprimere 2^64.
Se facciamo overflow (superiamo il valore massimo) il bit che avanza viene messo in carry bit e si riaprte a contare da 0.
Per quanto riguarda la notazione dei numeri negativi si utilizza la notazione in complemento a 2 i numeri negativi hanno la stessa rappresentazione dei numeri positivi correlati, esempio: se il massimo numero è 256 e per interi unsigned abbviamo un range da 0-255 invece per i signed avremo -128-128, lo 0 viene rappresentato dal 256esimo numero, quindi se noi facciamo 0-1 il numero sarà 255 ma rappresenterà il -1.

### Lezione 3: Registers

![registri_amd64.png](registri_amd64.png)

La CPU salva i dati temporanei nei registri, nelle architetture moderne sono solitamente da 64 bit, e possiamo effettuarel'accesso anche parzialmente ai primi 16/32 bits, inoltre in alcuni casi si può accedere singolarmente anche ai primi due byte.
"mov" è una delle istruzioni assembly per caricare i registri (esempio: `mov rax, 0x539`) possiamo caricarli sia con valori immediati, come l'esempio, sia con valori di altri registri.
Registri speciali:

* `rip`: indirizzo di memoria della prossima istruzione (non si può direttamente leggere o scrivere)
* `rsp`: contiene l'indirizzo dello stack

#### Utilizzo registri più piccoli

Quando si utilizza la versione 32 bit del registro, il resto viene azzerato.
Quando si utilizzano le sezioni più piccole del registo, invece, viene mantenuto il resto del registro invariato.
_EX:_

```
        mov rdx, 0x1122334455667788
        mov edx, 0x99
        # Risultato: rdx contiene
        #        0x0000000000000099
```

```
        mov rdx, 0x1122334455667788
        mov dx, 0x99
        # Risultato: rdx contiene
        #        0x1122334455660099
```

**I registri 8 bit si comportano come il secondo esempio**

In caso di numeri con segno l'azzeramento causerebbe un problema, quindi si usa `movxs` (move sign-extending) che salva in complemento a due il numero all'interno del registro.

### Lezione 4: Memory

Memoria RAM: indirizzata linearmente da 0x10000 (non inizia da 0 per motivi di sicurezza  __TODO:__ why?) a 0x7fffffffffffffff (rchitettura a 64 bit) ogni indirizzo di memoria è referenziato da un byte -> 127 TB di memoria massima (virtuale).
Stack (lifo): i dati si carico sullo stack con il comando "push" (rsp viene decrementato di 8), con "pop" si rimuovono (rsp viene aumentato di 8).

_Deprecato, in realtà molti compilatori il base pointer non lo usano proprio in amd64_
*Utilizziamo il base pointer (`%rbp`) come punto di riferimento per sapere dove trovare le variabili locali della funzione nello stack (spesso si fa, per esempio, `[%rbp + 0x16]`).
Il base pointer verrà spostato tutte le volte che si entra/esce da una funzione.*

__NB la notazione dei byte è little endian: il bit meno significativo va a sinistra__

Con l'struzione lea (load effective address) puà essere caricato il valore di un registro tramite il numero relativo esempio [rsp + 8]

### Lezione 5: Control Flow

#### jumps

x86 ha una miriade assurda di metodi per saltare.
I più interessanti sono (per il dudo che fa lezione):
`jmp`: salto non condizionale
`je` e tutti gli altri: salti condizionali, decidono se saltare o meno a seconda del contenuto di biut specifici in `rflags` (settati da comparison, overflow, zero, sign, ecc.)
`eb`: salto corto, prende un byte e salta relativamente rispetto a `rip`
__TODO:__ check quando le flag in `rflags` vengono azzerate

#### looping

jump condizionale nella stessa zona di codice (non credo di dover spiegare sta roba)

#### function call

sono interessanti le istruzioni
`call`: salva rip sullo stack e salta alla nuova istruzione
`ret`: recupera rip dallo stack e ci salta "dentro"

È importante la calling convenction, ovvero il contratto tra chiamante e chiamato su chi fa pulizia dello stack e cosa deve essere dove.
[amd64 Sys-V](https://web.archive.org/web/20220618203822/https://refspecs.linuxfoundation.org/elf/x86_64-abi-0.99.pdf)
I punti salienti sono:

* registri scrath che si possono usare: `%rax, %rcx, %rdx, %r8, %r9, %r10`
* lo stack deve essere sempre allineato a 16 bit
* [red zone](https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64): 128 byte dopo `%rsp` sono riservati per dati temporanei all'interno della funzione. È un ottimizzazione fatta per non dover spostare `%rsp`.
* parameter passing:
  * per i dati di tipo INTEGER (check documento p. 17) vengono usati i registri `%rdi,
%rsi, %rdx, %rcx, %r8, %r9`
  * per i dati di tipo MEMORY, X87 viene usato lo stack
  * per la roba SSE, SSEUP vengono usati i registri vettoriali
  * quanto i registri sono pieni, si usa lo stack in ordine right-to-left
  * smthing about `varargs or stdargs`, registro %al contiene l'upper bound del numero di parametri
* ritorno:
  * se classe MEMORY, il chiamante riserva dello spazio e lo passa in `%rdi` (come se fosse il primo argomento). Al ritorno, la funzione setterà `%rax = %rdi`
  * se classe INTEGER, verranno usati `%rax, %rdx`
  * classe SSE, X87 non mi interessano

### Lezione 6: System Calls

Le systemcall interagiscono con il sistema operativo; in assembly è necessario caricare il registro `rax` con la codifica della system call corrispondente e poi usare l'istruzione "syscall", esempio:

```
mov rax, 0 #read
syscall
```

Per le system call che hanno come input le stringhe è necessario caricare i caratteri sullo stack (puntato da `rsp`).
__TODO:__ sta parte è un po' mhe, sinceramente non credo sia necessario utilizzare lo stack per forza.

Per quanto riguarda la syscall exit() bisogna inserire il codice di ritorno in `rdi` e poi chiamarla con il valore 60 in `rax`:

```
 mov rdi, 42 #return code
 mov rax, 60
 syscall
```

### Lezione 8: Building Programs

Una volta scritto un sorgente in assembly è necessario assemblarlo in un binario con il comando `gcc -nostdlib -o binario sorgente`. `-nostdlib` sta a specificare che il programma non è scritto in C.
Per leggere un binario: `objdump -M intel -d binario` (da l'output in binario (little endian) e accanto l'assembly corrispondente.
Per debuggare l'assembly usa l'istruzione `int3`
Programmi utili:

* `strace`: fa vedere tutte le system call utilizzate dal binario
* `gdb`: debugger
* `rappel` fa esplorare gli effetti delle istruzioni (???repl per asm???)

## Altre risorse

<https://www.amd.com/system/files/TechDocs/24592.pdf>
<https://www.felixcloutier.com/x86/index.html>

### Load Effective Address (lea)

__TODO:__ tbh non ho ancora capito bene che succede:

```
        ...
        lea rsi, [rip+.hello]
        ...
        .hello:
        .string "hello word"
```

## Challenges

### embryoasm 1

Questa challenge è la flag gratis. Ti chiede di spostare un valore in un registro, quindi istruzione `mov`. L'utilità sta nel farti capire il contesto, noi procederemo così:

* pwntools per comunicare con la challenge
* la challenge si aspetta codice binario in `stdin`
* pwntools gestisce quindi anche la parte di assembly

__pwntools va saputo usare__, rimando al [Mod2](mod2) per risorse in merito

### embryoasm 2-9

* semplici operazioni tra registri
* divisione intera (qui c'è weird stuff happening)
* utilizzare la parte bassa dei registri
* utilizzare op logiche

### embryoasm 10-13

* utilizzare la memoria come puntatori (dereferenziare)
* roba sul little endian

### embryoasm 14-16

* introdotto lo stack

### embryoasm 17-23

* introdotti i salti
* questo ti permette di avere i cicli
* le funzioni le considero un tipo speciale di salto tbh (amo troppo risc)

#### 17

Qui theres a bit of a tricky situation:
chiede di fare un salto relativo di 0x51 byte in avanti
la soluzione è la seguente:

```

        jmp .jumpstart+0x51
        .jumpstart:
```

Questo produce infatti il bytecode: `eb51`, ovvero salto corto di 0x51 byte

_Ecco le altre soluzioni possibili, e perchè sono sbagliate_

```
       jmp .+0x51
```

Questa soluzione ha logicamente senso: `.` significa "posizione attuale" per l'assembler.
Non funziona, perchè viene assemblato in `eb4f`, dato che l'istruzione è composta da 2 byte.
Infatti, `jmp .+0x53` funziona.

_Altra cosa interessante:_
I `jmp` con valore immediato sono sempre relativi (non ho proprio una gran fonte ma mi fido).
Per fare un salto assoluto con un valore immediato, le soluzioni sono mandarlo sullo stack:

```
push 0xdeadbeef
ret
```

oppure usare un registro temporaneo

```
mov rax, 0xdeadbeef
jmp rax
```

#### 18

Qui senza dire niente a nessuno la challenge voleva il risultato in 32 bit

```
...
                    correct = (
                        self.init_values[0] + self.init_values[1] + self.init_values[2]
                    ) & 0xFFFFFFFF
...
                assert self.rax == correct
        except AssertionError:
            return False
        return True
```

Soluzione quite ez a quel punto, ecco il codice (lo metto qui perchè utilizzo qualche istruzione che può tornare utile in futuro):

```
  movsx rax, dword ptr [rdi]

  movsx rbx, dword ptr [rdi+4]
 movsx rcx, dword ptr [rdi+8]
 movsx rdx, dword ptr [rdi+12]
 int3

 cmp rax, 0x7f454c46
 je .yolo1
 cmp rax, 0x00005a4d
 je .yolo2
 jmp .yolo3

...

 .yolo3:
# qui bisogna stare attenti, imul sovrascrive rdx
 mov rax, rdx
 imul rbx
 imul rcx
 jmp .end

 .end:
 and eax, 0xFFFFFFFF
 int3
 nop
```

#### 19

Guardando [la documentazione](https://www.felixcloutier.com/x86/jcc#description), non esistono salti assoluti condizionali. È quindi necessario passare ad un trampolino e poi saltare dove vogliamo.

```
0x400000:    cmp       rdi, 3
0x400004:    jle       0x400009
0x400006:    jmp       qword ptr [rsi + 0x20]
0x400009:    jmp       qword ptr [rsi + rdi*8]
```

#### 20, 21

straightforward, ricorda che esiste l'istruzione `loop` che in 2 byte fa un for

#### 22, 23

si inizia a creare funzioni, quindi calling convention e tutto
