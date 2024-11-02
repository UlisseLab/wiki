+++
title = "Mod 1: Program Misuse"
+++

{{% resources pattern="files/" /%}}

### Privilege escalation

bho, roba su suid :D
**!!!!SE QUALCUNO HA VOGLIA DI FARE GLI APPUNTI PER BENE FACCIA PURE!!!!**

## Materiale aggiuntivo

**OBBLIGATORIO:** nessuna persona sulla faccia della terra può identificarsi come sistemista UNIX senza aver visto [questo video](https://youtu.be/tc4ROCJYbm0).

Per chiunque non conosca abbastanza bene l'ambiente UNIX, consiglio di partire capendo il funzionamento della shell, cosa come e quando manipola l'input.

- [SLIDES](https://tkanon.ninja/Uni/OLD/LAUREA_INGEGNERIA_INFORMATICA/3/LABORATORIO_DI_AMMINISTRAZIONE_DI_SISTEMI_T/virtuale/shell_scripting-print.pdf)
- [LEZIONI](https://tkanon.ninja/roboEdu/registrazioni/IngegneriaInformatica%203/LABORATORIO_DI_AMMINISTRAZIONE_DI_SISTEMI_T/roboEdu/)

*(imho di fondamentale importanza è la seconda parte della lezione del 16/03/21, ma anche le altre su BASH non si buttano)*

- [Cyberchef](https://gchq.github.io/CyberChef/) è un tool per manipolare facilmente dati offuscati. Imparatelo ad usare che torna utile in fretta.

- Vim è utile.
  Lo troverete ovunque ed è in generale comodo per manipolare file di testo velocemente.
  Guardatevi [questo talk](https://youtu.be/wlR5gYd6um0) che spiega il concetto fondamentale, ovvero come vim sia un linguaggio [turing completo](https://buttondown.email/hillelwayne/archive/vim-is-turing-complete/), e non un semplice editor di testo con una serie di scorciatoie come tutti gli altri.

## Challenges

### Opinioni generali

Imho queste challenge hanno poco senso, data una conoscenza abbastanza approfondita dei tool standard UNIX e della shell.
Alcuni sono simpatici rompicapo, altri troppo facili per valerne la pena.
Consiglio di farne un campione tra quelle che non sembrano ovvie, giusto per entrare nell'ottica giusta.

### babysuid 1-9

Presente il bit SUID settato (con owner root) per i seguenti binari (che in un modo o nell'altro stampano su stdout il contenuto di un file)
I binari utilizzati sono i seguenti:

- `cat`
- `more`
- `less`
- `tail`
- `head`
- `sort`
- `vim`
- `emacs`
- `nano`

### babysuid 10-16

Qui verrà stampata la flag in stdout da tool che ne modificano il formato, sempre usando il sed SUID su binari con owner root.
La difficoltà aggiuntiva consiste nel ricavare il plaintext dai formati mostrari.
I binari utilizzati sono i seguenti:

- `rev`
- `od` (stampa in ottali)
- `hd` (hexdump)
- `xxd`
- `base32`
- `base64`
- `split` (da man: "split a file into pieces")

### babysuid 17-23

Stessa roba, a sto giro vengono utilizzati binari per creare/estrarre archivi.
I binari utilizzati sono i seguenti:

- `gzip`
- `bzip2`
- `zip`
- `tar`
- `ar`
- `cpio`
- `genisoimage`

### babysuid 24-32

Qui inizi ad utilizzare binari (sempre con SUID root) non esplicitamente creati per leggere file.
I binari utilizzati sono i seguenti:

- `env`
- `find`
- `make`
- `nice`
- `timeout`
- `stdbuf`
- `setarch`
- `watch`
- `socat`

### babysuid 33-36

Queste dicono di richiede un po' di programmazione, si tratta sempre di tool standard unix che riescono a leggere file da disco.
I binari utilizzati sono i seguenti:

- `whiptail`
- `awk`
- `sed`
- `ed` - **THE STANDARD UNIX EDITOR**

### babysuid 37-40

Qui iniziamo ad utilizzare SUID per modificare i permessi del fs, invece di leggere direttamente la flag.
I binari utilizzati sono i seguenti:

- `chown`
- `chmod`
- `cp`
- `mv`

### babysuid 41-44

SUID su interpreti di lang general purpose.
I binari utilizzati sono i seguenti:

- `perl`
- `python`
- `ruby`
- `bash`

### babysuid 45-50

Queste riguardano binari non banalmente esploitabili per leggere il contenuto della flag. Richiedono un po' di fantasia.
I binari utilizzati sono i seguenti:

- `date`
- `dmesg`
- `wc`
- `gcc`
- `as` (portable gnu assembler)
- `wget`
- `ssh-keygen`
