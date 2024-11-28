+++
title = "29 Novembre 2024"
description = "Basi di Network Security"
date = 2024-11-28
+++

## Parte 0: Accesso alla piattaforma

- Vai alla pagina <https://training.olicyber.it/>
- Crea un account con il tuo indirizzo email e accedi alla piattaforma

## Parte 1: Network Basics

- [NS_1.1 - Fondamenti di reti di calcolatori](../2024-11-22/NS_1.1%20-%20Fondamenti%20di%20reti%20di%20calcolatori.pdf)
- [NS_1.2 - Analisi del traffico di rete con Wireshark](NS_1.2%20-%20Analisi%20del%20traffico%20di%20rete%20con%20Wireshark.pdf)

### Wireshark

- [Wireshark](https://www.wireshark.org/)
- [Wireshark Docs](https://www.wireshark.org/docs/)

Se utilizzi Kali Linux, Wireshark è già preinstallato. Per altre distribuzioni, puoi installarlo con uno dei seguenti comandi:

```bash
sudo apt install wireshark         # Per Ubuntu/Debian/Kali
sudo dnf install wireshark         # Per Fedora
sudo pacman -S wireshark           # Per Arch Linux
sudo zypper install wireshark      # Per openSUSE

```

> [!TIP] Challenge
>
> 1. Installa Wireshark sul tuo computer e prova a catturare il traffico di rete mentre navighi su internet. Riesci a trovare il tuo traffico HTTP?
> 2. Prova a fare `ping` ad un altro computer e cattura il traffico di rete con Wireshark. Cosa vedi?

## Parte 2: Firewall su Linux

Il kernel Linux utilizza il sottosistema netfilter per filtrare il traffico di rete. Per configurare netfilter, i due strumenti principali sono:

- **iptables**: uno strumento classico e ampiamente utilizzato.
- **nftables**: una soluzione più recente e flessibile.

Durante questa lezione, ci concentreremo su iptables.

### iptables

### firewalld

[FirewallD][firewalld] è un software di gestione del firewall per Linux che fornisce un interfaccia D-Bus per impostare regole di firewall.

E' il firewall di default su distribuzioni di derivazione Red Hat come Fedora, CentOS e RHEL. In aggiunta a quanto già offerto da `iptables`, `firewalld` offre il concetto di zone, la cui gestione permette di semplificare la configurazione del firewall.

[firewalld]: https://firewalld.org/

La sua interfaccia a riga di comando è `firewall-cmd`.

```bash
# Visualizza la zona attiva
firewall-cmd --get-active-zones

# Mostra le regole della zona corrente
firewall-cmd --list-all

# Visualizza le regole della zona 'public'
firewall-cmd --zone=public --list-all

# Aggiungi il servizio HTTP alla zona 'public'
firewall-cmd --zone=public --add-service=http

# Aggiungi il servizio HTTP in modo permanente
firewall-cmd --zone=public --add-service=http --permanent

# Riavvia il servizio firewalld
systemctl restart firewalld

# Blocca tutto il traffico in ingresso eccetto la porta 22
firewall-cmd --zone=public --set-target=DROP
firewall-cmd --zone=public --add-port=22/tcp

# Salva la configurazione attuale
firewall-cmd --runtime-to-permanent

```

#### Permanent vs Runtime

Le modifiche fatte con firewall-cmd sono, di default, temporanee (runtime). Per renderle permanenti, usa il comando:

```bash
firewall-cmd --runtime-to-permanent
```

## Parte 3: VPN

Una [VPN] (Virtual Private Network) è un metodo per creare una connessione sicura tra due dispositivi su una rete pubblica.

[VPN]: https://en.wikipedia.org/wiki/Virtual_private_network

### OpenVPN

[OpenVPN] è un software open-source che opera a livello user-space, offrendo grande flessibilità, ma con prestazioni leggermente inferiori rispetto ad altre soluzioni.

[OpenVPN]: https://openvpn.net/

### WireGuard

[WireGuard] è una VPN open-source integrata direttamente nel kernel Linux, offrendo maggiore velocità e sicurezza grazie a impostazioni predefinite robuste.

[WireGuard]: https://www.wireguard.com/

#### WireGuard su Linux

WireGuard è supportato nativamente dai kernel moderni. Per configurarlo, è necessario installare `wireguard-tools`:

```bash
sudo apt install wireguard-tools     # Per Ubuntu/Debian/Kali
sudo dnf install wireguard-tools     # Per Fedora
sudo pacman -S wireguard-tools       # Per Arch Linux
sudo zypper install wireguard-tools  # Per openSUSE

```

> [!TIP]- Esempio di file di configurazione (.conf):
>
> ```conf
> [Interface]
> PrivateKey=<chiave_privata>
> Address=<ip_locale>
> ListenPort=<porta>  # opzionale
>
> [Peer]
> PublicKey=<chiave_pubblica>
> Endpoint=<ip_pubblico>:<porta>
> AllowedIPs=<ip_remoti>
>
> ```

Comandi utili per WireGuard:

```bash
sudo wg-quick up <config_file>.conf     # Avvia la VPN
sudo wg-quick down <config_file>.conf   # Arresta la VPN
```
