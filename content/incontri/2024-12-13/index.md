+++
title = "13 Dicembre 2024"
description = "SQLi"
author= "Renny"
date = "2024-12-11"
tags = ["web", "sql"]
+++

[Slide PDF](./web-sqli.pdf)

## Preambolo

cosa ci serve:

- basi di web security (`requests`)
- conoscenza di base di **SQL** (ma anche no)
- creatività

Questa lezione si può seguire da Windows o Linux (ma sempre meglio installare Linux 🙏).

## a cosa mi serve SQL?

Alcuni di voi avranno visto **SQL** in sè per sè, ma come lo integro in un applicazione e per cosa lo uso?

Use cases ovvi:

- conservare dati strutturati ->
  - credenziali applicazioni web 🔥?
  - dati personali di utenti?

Chiaramente per un attaccante un database è sempre una superficie d'attacco notevole.

## come interagisce un applicazione con il DB?

In qualche modo la mia applicazione farà **query** al database sottostante.

Possiamo immaginarci qualcosa del genere:

```python
user_input = ... #prendiamo input dall'utente
query = "SELECT * FROM users WHERE id = " + user_input
cursor.execute(query)
```

Come al solito, il primo approccio che prendiamo è spesso sbagliato. Come posso rompere un codice del genere?

Se avete voglia di testare un po' (e dovreste) provate questo [sito](https://sqlfiddle.com/mysql/online-compiler?id=5bd5008e-3f1d-4386-9c52-6fb4c9363925).

## cosa non va?

Guardando qualsiasi applicativo conviene sempre ragionare partendo da dove abbiamo **controllo**, ovvero il nostro input. Dove andrà a finire ciò che mandiamo? Che tipo di restrizioni sono imposte su ciò che mandiamo?

Ragionando su questi binari, ci rendiamo presto conto che

- Possiamo mandare quello che ci pare
- andrà a finire direttamente nella query

Nulla a questo punto ci vieta di mandare **comandi**, ovvero parole che SQL interpreta come codice : possiamo iniettare (injection) **SQL** a nostro piacimento!

Abbiamo una SQL injection, e questo vuol dire che siamo liberi di leggere e scrivere nel DB, in modo (spesso) arbitrario.

## un esempio più complesso (e utile)

Spesso abbiamo meno **controllo** sul nostro input : o è filtrato o è piazzato in modo sconveniente se vogliamo rompere l'applicazione.

Provate a risolvere questa [challenge](http://web-17.challs.olicyber.it/logic) di OliCyber (hint hint : **control characters**).

### definizione: injection

Quando un programma interpreta il nostro input, che dovrebbe essere letterale ed inerte, come codice. Spesso richiede un qualche **escape character** o **sequence**.

## tangente : come si fa un app sicura?

Ci sono modi e modi di fare query:

- I modi giusti e sicuri 🥱 :
  - query parametrizzate : uso dei placeholder per rappresentari i miei valori nella query :
  ```
  cursor.execute("SELECT * FROM users WHERE id = ?", (user_id,))
  ```
- ORM (Object Relational Mapping) : Uso librerie per astrarre le query (SQLAlchemy)

Ricordatevi che in questo modo vi state semplicemente affidando al codice di altre persone, che spesso è ugualmente [rotto](https://nvd.nist.gov/vuln/detail/cve-2024-1597), però meglio di niente

## e se non vedo tutto l'output?

é molto raro che i server connettano direttamente gli utenti al DB senza fare qualche altro calcolo in mezzo.

Forse l'**injection** è in un endpoint che restituisce solo un valore numerico, oppure booleano per la logica dell'applicazione.

O ancora, immaginiamoci di avere un **injection** che ci permette di sovvertire un endpoint per vedere i dettagli di un utente, in modo da poter vedere anche la sua password : magari la query che viene eseguita è giusta, ma il server prova a interpretarla secondo il formato che si aspetta (senza la password) e crasha, senza darci altre informazioni.

Ci troviamo davanti a un esempio di **oracolo**

### definizione : oracolo

~L'oracolo (dal latino oraculum) è un essere o un ente considerato fonte di saggi consigli o di profezie, un'autorità infallibile, solitamente di natura spirituale.~

In cybersecurity, una funzione che ci permette di interrogare un programma ricevendo una risposta booleana (si o no).

Spesso nel mondo reale la risposta non è diretta, ma deriva dalla presenza o meno di un errore : **error-based oracle**

## come si chiede un nome ad un oracolo?

Noi vogliamo una stringa, ma l'oracolo ci dà solo si e no! -> `divide et impera`

Chiediamo un carattere alla volta, testando con tutto l'alfabeto -> `la prima lettera è [A-Z]?`

Dopo qualche richiesta avremo il primo carattere, e procediamo così fino alla fine.

### si può fare di meglio (meno richeste)?

Lasciato come esercizio al lettore

### una challenge ad oracolo

Familiarizzate con le challenge ad oracolo risolvendo [questa](http://web-17.challs.olicyber.it/blind)!

## E se l'output non c'è?

A volte il server non ha bisogno di mandarci nulla indietro nel suo uso inteso (ex. aggiornare il proprio username). Come facciamo?

Forti del fatto che la giusta combinazione di 0 ed 1 possono rappresentare tutto il rappresentabile, non ci rimane che trovare un modo nuovo di ricavarli.

In diversi dialetti di **SQL** abbiamo delle simpatiche funzioni, che possiamo usare in congiunzione ad `IF` per ottenere oracoli :

- `SLEEP` : blocca il DB per x secondi -> se la risposta del server è lenta, abbiamo un 1 altrimenti 0
- Funzioni di read di risorse : alcuni server **SQL** permettono la lettura di risorse, sia locali che remote (protocolli HTTP o FTP) -> secondo voi come derivo un oracolo?

Queste sono alcuni casi comuni, ma la parte divertente è adattarsi alle circostanze in modi nuovi.

Non usate tool già fatti come `sqlmap`, non impararete molto e di sicuro non furbi quanto voi (**aka** non funzionano)
