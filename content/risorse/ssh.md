+++
title = "SSH"
+++

Il protocollo **SSH** (Secure Shell) è un protocollo di rete che permette di stabilire una connessione sicura tra due macchine.

L'utilizzo più comune di SSH è quello di **accedere in remoto alla shell di un'altra macchina**, ma viene utilizzato senza che l'utente se ne accorga in molti altri contesti (rsync, git, ...).

L'implementazione più comune di SSH è **OpenSSH**, che è disponibile di default su quasi tutte le distribuzioni Linux.

## Connessione

Per connettersi ad una macchina remota via SSH è sufficiente utilizzare il comando `ssh` seguito dall'indirizzo IP o dal nome della macchina remota.

```shell
ssh user@remote
```

Dove:

- `user` è l'utente con cui si vuole effettuare il login
- `remote` è l'indirizzo IP o il nome della macchina remota

Se l'utente è lo stesso sia sulla macchina locale che sulla macchina remota, è possibile omettere l'utente.

```shell
ssh remote
```

## Configurazione

E' possibile configurare il comportamento di SSH tramite il file `~/.ssh/config`. Normalmente questo file è utilizzato per definire opzioni e nomi di host.

Per esempio, al posto di eseguire

```shell
ssh -p 2222 -J user@jump -i ~/.ssh/id_rsa user@186.x.x.x
```

è possibile definire un host nel file `~/.ssh/config`:

```shell
Host remote
    HostName 186.x.x.x
    User user
    Port 2222
    IdentityFile ~/.ssh/id_rsa
    ProxyJump user@jump
```

e poi connettersi con

```shell
ssh remote
```

## SSH tunneling

Spesso è desiderabile esporre dei servizi che non siano direttamente accessibili dall'esterno.

Per fare ciò è possibile utilizzare il **tunneling SSH**.

Il tunneling SSH permette di creare un **tunnel sicuro tra due porte di due macchine in rete**, in modo che il traffico tra le due macchine sia crittografato e autenticato.

### Local forwarding

```shell
ssh -L 8080:localhost:80 user@remote
```

Dove:

- `8080` è la porta locale
- `localhost:80` è l'enpoint su cui è in ascolto il servizio remoto
- `user@remote` è l'utente e l'indirizzo IP della macchina remota

Questo comando crea un tunnel SSH tra la porta `8080` della macchina locale e la porta `80` della macchina remota. Tutti i pacchetti che arrivano sulla porta `8080` della macchina locale vengono inoltrati sulla porta `80` della macchina remota.

> [!TIP] Quando usarlo?
> Per utilizzare in locale un servizio remoto che non è direttamente accessibile dall'esterno.
>
> **Esempio:** un database che binda su `localhost:3306`

### Remote forwarding

> [!WARNING] Attenzione
> In questo modo si espone un servizio locale sulla macchina remota.
>
> **Assicurarsi che il servizio sia sicuro e che non possa essere sfruttato da terzi!**

```shell
ssh -R 8080:localhost:80 user@remote
```

Dove:

- `8080` è la porta remota
- `localhost:80` è l'enpoint su cui è in ascolto il servizio locale
- `user@remote` è l'utente e l'indirizzo IP della macchina remota

Questo comando crea un tunnel SSH tra la porta `8080` della macchina remota e la porta `80` della macchina locale. Tutti i pacchetti che arrivano sulla porta `8080` della macchina remota vengono inoltrati sulla porta `80` della macchina locale.

> [!TIP] Quando usarlo?
> Per far si che dalla macchina remota si possa accedere ad un servizio avviato sulla macchina locale.

## Ulteriori informazioni

Per maggiori informazioni, la fonte più affidabile è come sempre la pagina di manuale di `ssh`:

```
man ssh
```
