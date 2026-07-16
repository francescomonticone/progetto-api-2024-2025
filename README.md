# 🚛 MovHex – Ottimizzatore di percorsi su griglia esagonale

> Progetto finale del corso di **Algoritmi e Strutture Dati** – Politecnico di Milano (A.A. 2024–2025)

**Valutazione conseguita:** **30 e Lode**

---

## Descrizione

MovHex è un simulatore di una rete di trasporto progettato per calcolare percorsi a costo minimo su una mappa composta da celle esagonali.

La rete è modellata come un **grafo pesato dinamico** in cui i veicoli possono spostarsi:

- tramite collegamenti terrestri tra esagoni adiacenti;
- tramite rotte aeree direzionate aggiungibili e rimovibili durante l'esecuzione.

Il progetto è stato sviluppato come prova finale del corso di **Algoritmi e Strutture Dati** con l'obiettivo di progettare strutture dati efficienti e algoritmi ottimizzati per la gestione di interrogazioni sul percorso minimo in presenza di aggiornamenti dinamici della rete.

---

# Funzionalità

- Inizializzazione di mappe esagonali di dimensioni arbitrarie.
- Gestione dinamica del costo di attraversamento degli esagoni.
- Aggiornamento locale dei costi mediante propagazione su raggio.
- Inserimento e rimozione di rotte aeree orientate.
- Calcolo del percorso minimo tra due esagoni.
- Interfaccia a riga di comando conforme alle specifiche del progetto.

---

# Modello della mappa

La mappa è rappresentata come una griglia rettangolare di esagoni.

Ogni esagono contiene:

- costo di uscita via terra;
- collegamenti terrestri verso gli esagoni adiacenti;
- fino a cinque rotte aeree uscenti;
- informazioni ausiliarie utilizzate dagli algoritmi di ricerca.

Le rotte aeree sono direzionate e possono essere create o rimosse dinamicamente durante l'esecuzione.

---

# Comandi implementati

| Comando | Descrizione |
|----------|-------------|
| `init` | Inizializza la mappa |
| `change_cost` | Aggiorna il costo degli esagoni in una regione |
| `toggle_air_route` | Inserisce o rimuove una rotta aerea |
| `travel_cost` | Calcola il percorso a costo minimo |

---

# Strutture dati utilizzate

## Array di nodi

L'intera mappa è memorizzata in un **array monodimensionale** di strutture `node`.

Questa rappresentazione permette:

- accesso diretto agli esagoni in tempo costante;
- ottima località di memoria;
- riduzione dell'overhead rispetto ad una rappresentazione completamente dinamica.

Ogni nodo memorizza:

- costo di uscita;
- vicini della griglia;
- lista delle rotte aeree;
- informazioni di supporto agli algoritmi.

---

## Liste di adiacenza

Le rotte aeree sono implementate mediante **liste concatenate**.

Questa scelta consente di utilizzare memoria proporzionale al numero effettivo di collegamenti e rende semplici le operazioni di inserimento e rimozione.

---

## Min Heap

Per il calcolo del percorso minimo è stato implementato un **Min Heap binario** realizzato tramite array.

Il Min Heap gestisce efficientemente le operazioni di:

- inserimento;
- estrazione del minimo;
- aggiornamento della priorità (`decrease-key`).

Per velocizzare quest'ultima operazione viene mantenuto un vettore che associa ogni nodo alla sua posizione corrente nell'heap.

---

## Cache dei percorsi

Poiché il testo della prova specifica che il comando `travel_cost` viene eseguito molto più frequentemente rispetto agli aggiornamenti della rete, è stata implementata una **cache dei percorsi**.

La cache utilizza una **Hash Table con Separate Chaining** che memorizza:

```
(sorgente, destinazione) → costo minimo
```

Quando viene richiesta una coppia già calcolata, il risultato viene restituito immediatamente senza rieseguire la ricerca del percorso minimo.

Ogni modifica della rete (`change_cost` o `toggle_air_route`) invalida automaticamente la cache.

---

## Cache dei vicini

Durante l'inizializzazione vengono precalcolati i vicini della griglia esagonale la cui determinazione dipende dalla parità della riga.

In questo modo le visite del grafo non devono ricalcolare continuamente le coordinate dei vicini.

---

# Algoritmi implementati

L'implementazione combina diversi algoritmi classici:

- rappresentazione di grafi pesati mediante liste di adiacenza;
- ricerca del percorso minimo tramite algoritmo di Dijkstra;
- gestione dinamica delle priorità mediante Min Heap;
- ricerca in Hash Table;
- calcolo della distanza tra celle di una griglia esagonale.

---

# Ottimizzazioni

L'implementazione è stata progettata considerando il carico di lavoro previsto dalla consegna:

- moltissime interrogazioni `travel_cost`;
- pochi aggiornamenti della rete;
- richieste concentrate nelle stesse aree della mappa.

Per questo motivo sono state introdotte varie ottimizzazioni:

- rappresentazione contigua della mappa;
- precalcolo dei vicini;
- Min Heap personalizzato;
- cache dei percorsi;
- Hash Table per accesso rapido ai risultati già calcolati.

---

# Testing

Per verificare correttezza e robustezza dell'implementazione sono state realizzate diverse suite di test.

### Edge cases

Sono stati verificati:

- percorsi con sorgente uguale alla destinazione;
- nodi non raggiungibili;
- inserimento/rimozione di rotte aeree;
- aggiornamenti dei costi;
- esagoni intransitabili;
- gestione degli input limite.

### Stress test

Sono stati inoltre eseguiti test su mappe di grandi dimensioni contenenti migliaia di operazioni, comprendenti:

- `travel_cost`
- `change_cost`
- `toggle_air_route`

Lo scopo era verificare:

- correttezza dopo numerosi aggiornamenti;
- stabilità della gestione dinamica della memoria;
- comportamento delle strutture dati su grandi istanze;
- efficacia delle ottimizzazioni introdotte.

---

# Tecnologie utilizzate

- Linguaggio **C**
- Grafi pesati
- Liste di adiacenza
- Min Heap
- Hash Table
- Gestione dinamica della memoria

---

# Competenze acquisite

Durante lo sviluppo del progetto sono stati approfonditi:

- progettazione di strutture dati efficienti;
- algoritmi su grafi;
- algoritmo di Dijkstra;
- progettazione di Hash Table;
- implementazione di Heap;
- analisi della complessità;
- ottimizzazione delle prestazioni;
- gestione della memoria in C.

---

# Nota

Questo repository contiene **la mia implementazione personale** del progetto finale del corso di **Algoritmi e Strutture Dati** (Politecnico di Milano, A.A. 2024–2025).
