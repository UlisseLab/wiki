+++
title = "Attack and Defense"
+++

Le gare Capture the Flag (CTF) sono divise principalmente in due tipi:

- Jeopardy
- **Attack and defence**

## Introduzione

Le CTF attack and defence (da ora in poi A/D) sono una tipologia di competizione che mette a confronto squadre di giocatori che devono difendere i propri servizi da attacchi e attaccare i servizi delle altre squadre.

Questo tipo di competizione è molto simile a quella che si svolge durante [DEFCON].

[DEFCON]: https://en.wikipedia.org/wiki/DEF_CON#Capture_the_flag

Alcune volte le challenge A/D sono simili a quelle Jeopardy, in quanto i servizi sono vulnerabili e i giocatori devono trovare le vulnerabilità e sfruttarle per rubare le flag. Altre volte però i servizi sono più particolari (o più complessi e formati da tante parti), le cui vulnerabilità sono:

- note a tutti, quindi il gioco si basa sulla capacità di difendere il servizio e sulla capacità di attaccare gli altri servizi
- in un contesto di "glue code" (ad es. un servizio che si connette ad un altro servizio) e quindi la vulnerabilità è data dalla configurazione del servizio

{{% notice style="info" title="Esempio" %}}
Un servizio che si connette a un database con password predefinita è a rischio se il database è accessibile dall'esterno. Le password predefinite sono uguali per tutti, rendendo il sistema vulnerabile.
{{% /notice %}}

> _In 2017, the Legitimate Business Syndicate came up with their very own CPU architecture called cLEMENCy: a middle-endian with 9 bits bytes CPU. With its specifications released only 24h before the beginning of the CTF, it was designed with the explicit goals of both surprising the teams, and leveling the playing field by breaking all their tools._

## Ruoli

### L'arbitro

Il compito dell'arbitro è quello di controllare che i **servizi** vulnerabili **siano online e funzionino**.

Lo fa attraverso richieste ai servizi (HTTP, TCP, UDP, ...) che hanno un esito positivo e **depositano una flag** nel servizio (in un DB, in un file di testo, ...).

Ogni round viene fatto il controllo sulla funzionalità del servizio (**SLA**: Service Level Agreement).

Nei round successivi al primo l'arbitro può anche controllare (= controlla) che **la flag del round precedente sia ancora disponibile**.

{{% notice style="warning" title="Attenzione" %}}
La SLA è un termine **MOLTIPLICATIVO** nel calcolo del punteggio. Se un servizio non è funzionante per il 50% del tempo, il punteggio sarà **dimezzato**.
{{% /notice %}}

### L'attaccante

Ha il compito di attaccare i servizi vulnerabili dei team avversari e **recuperare le flag**.

Le flag trovate devono essere **inviate al gameserver**, che le valida e restituisce una risposta (OK, DUP, INV, ERR, ...).

{{% notice style="info" title="Flag scadute" %}}
Le flag scadono dopo un certo numero di round. Se non vengono inviate al gameserver entro il tempo limite, non verranno più accettate.
{{% /notice %}}

### Il difensore

Ha il compito di **impedire** agli attaccanti **di rubare le flag**.

Deve però **garantire la funzionalità del servizio**, altrimenti l'arbitro toglie punti SLA!

Può farlo per mezzo di patch, WAF, firewall, ...

## Regole di gioco

Il gioco si svolge in round di `n` secondi (di solito 60 o 120), durante i quali un l'arbitro

- aggiunge nuove flag alla macchina vulnerabile
- controlla l'**integrità** (SLA) dei servizi interagendo con essi e recuperando le flag attraverso accessi legittimi.

e durante i quali gli attaccanti

- cercano di **rubare** le flag degli avversari
- inviano le flag rubate al gameserver

## Traffico

Il traffico di rete di solito passa attraverso un router centrale, il cosiddetto **router di gioco**.

Le macchine vulnerabili sono connesse al **network di gioco**.

{{% notice style="warning" title="Attenzione" %}}
Tutto il traffico che passa per il router di gioco è **analizzato** dagli organizzatori della gara per rilevare attacchi e violazioni del regolamento.

**Non è consentito** fare attacchi di tipo DoS, DDoS, scansione di rete che saturino la rete o attacchi che possano compromettere la sicurezza del router di gioco o di qualsiasi infrastruttura e servizio non facente parte del gioco.
{{% /notice %}}

### IP masquerading

Per evitare che i team possano riconoscere gli IP degli avversari, il router di gioco fa **masquerading** degli IP.

Il grado di mascheramento può variare da gara a gara, ma di solito è molto alto. In alcuni casi il router di gioco fa IP masquerading per:

- VM -> VM
- Player -> VM
- CheckSystem -> VM

### SSH tunneling

Spesso è desiderabile esporre dei servizi che non sono direttamente accessibili dall'esterno. Questo per evitare che gli attaccanti possano accedere al servizio se il servizio non è dotato di autenticazione ed in generale per evitare che il servizio sia attaccato.

Per fare ciò si può usare il **tunneling SSH**.

{{% notice style="info" title="Tunneling SSH" %}}
Il tunneling SSH permette di creare un tunnel sicuro tra due porte di due macchine in rete, in modo che il traffico tra le due macchine sia crittografato e autenticato.
{{% /notice %}}

### Esempio

```bash
ssh -L 8080:localhost:80 user@remote
```

Dove `8080` è la porta locale, `localhost` è l'host su cui è in ascolto il servizio remoto (ad es. il servizio è stato avviato con `--bind-address=localhost`) e `80` è la porta remota su cui è in ascolto il servizio.

## Tool

I tool utilizzati dal nostro team sono quelli disponibili su [GitHub](https://github.com/UlisseLab).

## Link utili

- <https://2023.faustctf.net/information/attackdefense-for-beginners/>
- Statistiche degli ultimi anni: <https://cyberchallenge.it/attack-defense/2024>

### Tool di difesa

- <https://github.com/UlisseLab/tulip>
- <https://github.com/UlisseLab/caronte>
- <https://github.com/UlisseLab/pcap-broker>

### Tool di attacco

- <https://github.com/UlisseLab/DestructiveFarm>
- <https://github.com/OpenAttackDefenseTools/ataka>
- <https://docs.pwntools.com/>
