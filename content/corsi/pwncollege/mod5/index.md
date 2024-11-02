+++
title = "Mod 5: Sandboxing"
+++

## Appunti lezioni

{{% resources pattern="files/" /%}}

### Introduction

Qui spiega le motivazioni storiche per la creazione di sandbox, e la loro evoluzione nei decenni:

* __~1960__ Prime cpu girava tutto in [real mode](https://en.wikipedia.org/wiki/Real_mode)
* __~1980__ Viene introdotto, in hardware, supporto per memoria virtuale, diversi ring di autorizzazione per separare la roba user-space da kernel-space, protezione della memoria (prima con la segmentazione e poi con il paging) e altre cose belle
* __~1990__ separazione "in-process", quindi tra interprete e codice interpretato (__TODO:__ cerca more resources su sta roba)
* __~2000__ con l'espansione del www, i browser diventano sempre più complessi (e vengono creati sistemi per la distribuzione di [applicativi via web](https://en.wikipedia.org/wiki/Rich_web_application), _tipo Flash_). Con la complessità aumenta la superficie di attacco e la gravità di una possibile compromissione
* __~2010__ Gli engine JS diventano mostri enormi con performance di cristo, viene rilasciato HTML5 e muoiono gli applicativi custom su web. <br> Questo non fa altro che spostare il problema, quindi vengono introdotti layer di separazione tra il processo non privilegiato che esegue il codice web e il processo padre che funge da "sistema operativo", in some way (__TODO:__ anche qui la cosa mi è un po' fumosa, check better)

Alla fine il dudo parla di quanto sia figo il sandboxing e di come, anche se non sempre funziona, aggiunge un layer di security particolarmente efficace

#### Native Client Sandbox

Il dudo dice sta cosa, e io me la sono scritta per capire cosa fosse. Visto che Im lazy ecco che ne pensa chatgpt
>x86 Native Client (NaCl) is a sandboxing technology that allows developers to run native code securely in a web browser. It is based on the x86 architecture, which is the most commonly used microprocessor architecture for personal computers. NaCl uses a combination of hardware and software security measures to isolate the native code from the rest of the system, helping to prevent vulnerabilities and other security issues.
>
>NaCl uses a technique called "software fault isolation" to protect the system from potentially malicious code. This involves creating a sandbox, or isolated execution environment, in which the native code can run. The sandbox is designed to be as small and simple as possible, to minimize the potential attack surface for an attacker. It also includes various security measures, such as memory protection and code signing, to prevent the native code from accessing sensitive data or performing unauthorized actions.
>
>In addition to its security benefits, NaCl also provides performance advantages over other solutions for running native code in a web browser. Because it uses native code, it can run faster and more efficiently than other solutions that rely on interpreted or compiled code. This makes it an attractive option for developers who want to run native code in a web browser without sacrificing performance.
>
>Overall, x86 Native Client (NaCl) is a valuable tool for developers who want to run native code securely in a web browser. Its combination of security and performance makes it a popular choice for many applications.

### chroot

Chroot è una syscall unix che permette di cambiare la posizione di `/` per un certo processo.
Ha una qualche rilevanza lato security e se utilizzata correttamente non permette facilmente l'escape, ma in generale non va considerata un buon sistema di sandboxing.
I possibili modi per romperla sono:

* mantenere un fd aperto prima del chroot, questo non verrà chiuso e permette di scappare utilizzando una syscall che accetta path relativo (_ex: openat, execvat_) (`AT_FDCWD` contiene la dir corrente __TODO:__ check better)
* chroot non viene gestito come "stack", quindi quando viene riutilizzata la syscall viene completamente dimenticata la call vecchia. <br> Se hai `SUID == 0` e setuppi un'altra chroot e scappi dalla seconda, sei libero anche dalla prima.
* IPC, network e altro permette escape ad hoc. In pratica tutto quello che ha portato alla creazione dei [namespaces](https://en.wikipedia.org/wiki/Linux_namespaces)

### seccomp

Seccomp, basato su [eBPF](https://ebpf.io/), permette di utilizzare un meccanismo simile a quello dei firewall per le syscall.

È implementata come una librera, basta setuppare il seccomp all'inizio del proprio software.

Seccomp è ereditato da padre a figlio (__l'esempio della lezione its wrong tho__)

#### Come scappare da seccomp?

Guarda slides

* seccomp di default blocca tutte le syscall x86
