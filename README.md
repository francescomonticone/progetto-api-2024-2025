# 🚛 MovHex – Ottimizzatore di percorsi su griglia esagonale

> Progetto finale del corso di **Algoritmi e Strutture Dati** (A.A. 2024–2025).

>Voto conseguito: 30 e lode 

## Descrizione

MovHex è un simulatore di una rete di trasporto progettato per calcolare percorsi a costo minimo su una mappa composta da celle esagonali.

La mappa rappresenta una superficie suddivisa in esagoni regolari, all'interno della quale i veicoli possono muoversi sia tramite collegamenti terrestri tra esagoni adiacenti sia attraverso rotte aeree dirette create dinamicamente.

Il progetto è stato sviluppato come prova finale del corso di **Algoritmi e Strutture Dati** e pone particolare attenzione alla progettazione di strutture dati efficienti e all'implementazione di algoritmi per la ricerca del percorso minimo in presenza di modifiche dinamiche della rete.



## Funzionalità

* Inizializzazione di una mappa esagonale di dimensioni arbitrarie.
* Gestione del costo di attraversamento di ciascun esagono.
* Modifica dinamica dei costi di una regione della mappa.
* Inserimento e rimozione di rotte aeree orientate.
* Calcolo del percorso a costo minimo tra due esagoni.
* Interfaccia a riga di comando conforme alle specifiche del progetto.

## Modello della mappa

Ogni esagono è identificato da una coppia di coordinate `(colonna, riga)` e contiene:

* un costo di uscita via terra;
* fino a cinque rotte aeree uscenti;
* collegamenti terrestri con gli esagoni adiacenti.

Le rotte aeree sono direzionate e possono essere aggiunte o rimosse durante l'esecuzione del programma.

## Comandi supportati

Il programma implementa i seguenti comandi:

* `init` – inizializza la mappa;
* `change_cost` – modifica i costi di una regione della mappa;
* `toggle_air_route` – aggiunge o rimuove una rotta aerea;
* `travel_cost` – calcola il costo minimo di percorrenza tra due esagoni.

## Aspetti algoritmici

L'intera rete di trasporto viene modellata come un grafo pesato.

L'implementazione affronta i seguenti problemi algoritmici:

* rappresentazione efficiente del grafo;
* gestione di aggiornamenti dinamici dei pesi;
* ricerca del percorso minimo;
* calcolo della distanza tra celle di una griglia esagonale;
* gestione efficiente delle liste di adiacenza.

## Ottimizzazioni

Le specifiche del progetto prevedono che:

* le interrogazioni sul percorso minimo siano molto più frequenti delle modifiche alla rete;
* le richieste di percorso siano concentrate in specifiche aree della mappa.

L'implementazione è stata progettata tenendo conto di queste caratteristiche, privilegiando le prestazioni delle operazioni di ricerca rispetto agli aggiornamenti.

## Tecnologie utilizzate

* Linguaggio C
* Grafi pesati
* Liste di adiacenza
* Code di priorità (ove utilizzate)
* Gestione dinamica della memoria

## Obiettivi didattici

Il progetto ha permesso di approfondire:

* progettazione di strutture dati;
* algoritmi su grafi;
* ricerca del cammino minimo;
* analisi della complessità;
* gestione della memoria in C;
* ottimizzazione delle prestazioni.

## Nota

Questo repository contiene la mia implementazione personale del progetto finale del corso di **Algoritmi e Strutture Dati** (A.A. 2024–2025).

La descrizione presente in questo README è una sintesi realizzata da me e non riproduce il testo originale della prova d'esame.
