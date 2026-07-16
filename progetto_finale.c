#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAXINPUT 100
#define INF INT_MAX
#define HT_SIZE 60
#define A 7 //costante A per HT
#define B 23 //costante B per HT

//strutture dati utilizzate
typedef struct node{
	struct air_route* air_h;
	int v[4]; //salvo solo i vicini difficili da calcolare in base alla riga non quelli banali
	short int cost;
	unsigned char count; //max 6 vicini basta un char
	unsigned char change_colour; //colore nodi
	unsigned char air_count;
} node;

typedef struct air_route{
	struct node* d; //nodo di destinazione che collega la rotta
	struct air_route* next;
} air_route; //lista per i collegamenti aerei

//lista per implementare la coda
typedef struct queuenode{
	struct queuenode* next; //puntatore a elemento successivo coda
	node* hex; //puntatore ad esagono corrente
	int dist; //distanza dal nodo di partenza
} queuenode;

typedef struct queue{ //coda
	queuenode* head;
	queuenode* tail;
}queue;

typedef struct heapnode{
	int pos;
	int dist;
}heapnode;

typedef struct minheap{
	heapnode* a_heap; //array dinamico di heapnode
	int* pos_heap; //puntatore all'indice heap
	int heapsize; //elementi nel min heap
}minheap;

typedef struct cache_ht{ //hash table per cache risultati
	struct cache_ht* next;
	int s_pos;
	int d_pos;
	int dist;   // Distanza minima calcolata
} cache_ht;

typedef struct ht{
	cache_ht** buckets;  // tabella hash a chaining : Array di liste
} ht;

//funzione per calcolare i vicini
int pos(node* hex, node* map, int col) {
	return (hex - map); // indice come differenza tra puntatori
}

int adj(node* map, int pos, int rig, int col, int num){
	int x = pos%col;
	int y = pos/col;
	if(num == 3 || num == 0){ //questi li calcolo sempre gli altri li salvo nel vettore dei vicini
		if(x<col-1 && num == 0) return y * col + (x+1); //convenzione dei vicini oraria a partire da quello a dx (non dipende dalla riga)
		if(x>0) return y * col + (x-1); //vicino sx
	}
	else{
		if((y%2)==0){ //se riga pari
			if(y>0 && num == 1) return (y-1) * col + x; //vicino sotto  (y-1) * col + (x+1)
			if(y<rig-1 && num == 5) return (y+1) * col + x; //vicino sopra
			if(x>0 && y<rig-1 && num == 4) return (y+1) * col + (x-1); //vicino sx-sopra
			if(x>0 && y>0 && num == 2) return (y-1) * col + (x-1); //vicino sx-sotto
		}
		else{
			if(y>0 && num == 2) return (y-1) * col + (x); //vicino sotto
			if(y<rig-1 && num == 4) return (y+1) * col + (x); //vicino sopra
			if(x<col-1 && y<rig-1 && num == 5) return (y+1) * col + (x+1); //vicino dx-sopra
			if(x<col-1 && y>0 && num == 1) return (y-1) * col + (x+1); //vicino dx-sotto
		}
	}
	return -1;
}

void init_adj_cache(node* map, int rig, int col) {
	for(int y = 0; y < rig; y++) {
		for(int x = 0; x < col; x++) {
			int pos = y * col + x;
			map[pos].count = 0;
			for(int i = 1; i <= 5; i++) {
				if(i!=3){ //vicino 0 e 3 li calcolo ogni volta per risparmiare memoria
					int v_pos = adj(map, pos ,rig, col, i);
					if(v_pos != -1) {
						map[pos].v[map[pos].count] = v_pos;  // Assegnazione separata
						map[pos].count++;
					}
				}
			}
		}
	}
}

//funzioni per tabella hash
int hash(int s_pos, int d_pos) {
	return (A*s_pos + B*d_pos) % HT_SIZE; //faccio combinazione di posizioni con due coefficienti
}

ht* cache_init() {
	ht* hash_table = malloc(sizeof(ht)); //alloco array di buckets
	hash_table->buckets = calloc(HT_SIZE, sizeof(cache_ht*)); //HT di size con tutto inizializzato a NULL per comodità
	return hash_table;
}

void cache_insert(ht* hasht, int s_pos, int d_pos, int dist){
	int num = hash(s_pos, d_pos); //calcolo hash
	cache_ht* entry = malloc(sizeof(cache_ht));
	entry->s_pos = s_pos;
	entry->d_pos = d_pos;
	entry->dist = dist;
	entry->next = hasht->buckets[num]; //inserisco elemento in testa alla ht
	hasht->buckets[num] = entry;
}

int cache_find(ht* hasht, int s_pos, int d_pos){
	int num = hash(s_pos, d_pos);
	cache_ht* ptr = hasht->buckets[num];
	while(ptr!=NULL){
		if(ptr->s_pos == s_pos && ptr->d_pos == d_pos) return ptr->dist;
		ptr = ptr->next;
	}
	return -1; //non trovato
}

void cache_deallocate(ht* hasht){
	int i;
	if(hasht != NULL){
		for(i=0;i<HT_SIZE;i++){
			cache_ht* prec = hasht->buckets[i];
			while(prec!=NULL){
				cache_ht* ptr = prec->next;
				free(prec);
				prec = ptr;
			}
		}
		free(hasht->buckets);
		free(hasht);
	}
}

//funzioni per coda
void init_queue(queue* Q){
	Q->head = Q->tail = NULL;
}
int empty(queue* Q){
	if(Q->head == NULL){
		return 1;
	}
	else{
		return 0;
	}
}
void enqueue(queue* Q, node* hex, int dist) {
	queuenode* n_node = malloc(sizeof(queuenode));
	n_node->hex = hex;
	n_node->dist = dist;
	n_node->next = NULL;

	if (empty(Q)) {
		Q->head = Q->tail = n_node;
	} else {
		Q->tail->next = n_node;
		Q->tail = n_node;
	}
}
queuenode* dequeue(queue* Q){
	if(!empty(Q)){
		queuenode* ptr = Q->head;
		Q->head = Q->head->next;
		if(Q->head == NULL) Q->tail = NULL; //lista è diventata vuota
		ptr->next = NULL;
		return ptr;
	}
	else{
		printf("Errore: dequeue su coda vuota\n");
		return NULL;
	}
}
void min_heapify(minheap* h, int n){
	int l, r;
	int posmin;
	l = 2*n+1; //figlio sx con convenzione a partire da 0
	r = 2*n+2; //figlio dx con convenzione a partire da 0
	if(l<h->heapsize && h->a_heap[l].dist < h->a_heap[n].dist){
		posmin = l;
	}
	else{
		posmin = n;
	}
	if (r < h->heapsize && h->a_heap[r].dist < h->a_heap[posmin].dist) {
		posmin = r;
	}
	if(posmin != n){ //scambio i nodi nel min heap
		heapnode node = h->a_heap[n];
		h->a_heap[n] = h->a_heap[posmin]; //metto il nodo da posmin a n
		h->a_heap[posmin] = node; //metto il nodo da n a posmin salvato temporaneamente

		h->pos_heap[h->a_heap[n].pos] = n; //aggiorno puntatore alla posizione nell'heap
		h->pos_heap[h->a_heap[posmin].pos] = posmin; //stessa cosa con posmin

		min_heapify(h, posmin); //continuo con la procedura se necessario
	}
}

minheap* min_heap_init(int l){
	minheap* h = malloc(sizeof(minheap)); //creo min heap dinamico
	h->a_heap = malloc(l*sizeof(heapnode)); //array di heapnode, min heap vero e proprio
	h->pos_heap = malloc(l*sizeof(int));
	h->heapsize = 0; //0 elementi inseriti

	for(int i = 0; i < l; i++) {
		h->pos_heap[i] = -1;
	}
	//inizializzo a -1 tutte le posizioni nell'heap, non c'è nessun elemento
	return h;
}

void min_heap_insert(minheap* h, int pos, int dist){
	if(h->pos_heap[pos] != -1) return; //controllo se c'è già
	int n = h->heapsize;  //aggiungo coppia nodo distanza
	h->heapsize = h->heapsize + 1;
	h->a_heap[n].dist = dist;
	h->a_heap[n].pos = pos;
	h->pos_heap[pos] = n;

	while (n > 0 && h->a_heap[(n-1)/2].dist > h->a_heap[n].dist) { //distanza padre > distanza figlio
		heapnode node = h->a_heap[n];
		h->a_heap[n] = h->a_heap[(n-1)/2]; //faccio scendere il padre
		h->a_heap[(n-1)/2] = node; //faccio salire il figlio perchè è minore

		h->pos_heap[h->a_heap[n].pos] = n;
		h->pos_heap[h->a_heap[(n-1)/2].pos] = (n-1)/2;

		n = (n-1)/2; //vado avanti
	}

}

void heap_swap(minheap* h, int i, int j) {
	heapnode temp = h->a_heap[i];
	h->a_heap[i] = h->a_heap[j];
	h->a_heap[j] = temp;

	h->pos_heap[h->a_heap[i].pos] = i;
	h->pos_heap[h->a_heap[j].pos] = j;
}

void heap_free(minheap* h){

	if(h->a_heap != NULL){
		free(h->a_heap); //libero minheap
	}

	if(h->pos_heap != NULL) {
		free(h->pos_heap);
	}

	free(h); //libero struttura h per heap e ht
}

heapnode heap_min(minheap* h) {
	heapnode min = h->a_heap[0]; //estraggo minimo (primo elemento)
	h->pos_heap[min.pos] = -1; //aggiorno il puntatore a -1 perchè non c'è piu nell' heap
	h->heapsize--;
	h->a_heap[0] = h->a_heap[h->heapsize];
	if(h->heapsize > 0) {
		h->pos_heap[h->a_heap[0].pos] = 0;
		min_heapify(h, 0);
	}
	return min;
}


void decrease_key(minheap* h, int pos, int n_dist) {
	int ind = h->pos_heap[pos]; //mi salvo l'indice nell'heap del nodo alla posizione pos
	if (ind == -1 || n_dist >= h->a_heap[ind].dist) return; //se non esiste oppure la distanza nuova è peggiore non faccio niente
	// salvo il nodo da muovere
	heapnode target = h->a_heap[ind];
	target.dist = n_dist;
	int flag = 0;

	// risalgo l'heap
	while (ind > 0 && !flag) {
		int parent = (ind - 1) / 2;
		if (h->a_heap[parent].dist > n_dist) { //se distanza del padre è maggiore vado avanti
			h->a_heap[ind] = h->a_heap[parent]; //sposto il nodo tanto il nodo originale salvato in target
			h->pos_heap[h->a_heap[ind].pos] = ind; //aggiorno valore puntato da pos_heap
			ind = parent; //sto risalendo
		} else {
			flag = 1;
		}
	} // posiziono il nodo nella giusta posizione
	h->a_heap[ind] = target;
	h->pos_heap[target.pos] = ind; //aggiorno valore puntato da pos_heap come indice effettivo
}


void air_pop(air_route** air_h, air_route* ptr){
	if(ptr == NULL || *air_h == NULL) return; //se lista è vuota
	air_route* prec = *air_h;
	if(prec == ptr){ //ptr è il primo elemento
		(*air_h) = (*air_h)->next;
		free(ptr);
	}
	else{
		while(prec->next!=ptr){
			prec = prec->next;
		}
		prec->next = ptr->next;
		free(ptr);
	}
}

void format(char *str,int *p1, int *p2, int *p3, int *p4,int *type){ //formattazione input per separare parametri
	char str_dup[MAXINPUT];
	if(strstr(str,"init")!=NULL){
		sscanf(str, "%s %d %d", str_dup, p1, p2);
		*(type) = 1;
		*(p3) = 0;
		*(p4) = 0;
	}
	else if(strstr(str,"change_cost")!=NULL){
		sscanf(str, "%s %d %d %d %d", str_dup, p1, p2, p3, p4);
		*(type) = 2;
	}
	else if(strstr(str,"toggle_air_route")!=NULL){
		sscanf(str, "%s %d %d %d %d", str_dup, p1, p2, p3, p4);
		*(type) = 3;
	}
	else if(strstr(str,"travel_cost")!=NULL){
		sscanf(str, "%s %d %d %d %d", str_dup, p1, p2, p3, p4);
		*(type) = 4;
	}
	else{
		printf("Comando non disponibile\n");
	}
}

//funzione init
node* init(int rig, int col){
	int i,j;
	node* map = malloc(col * rig * sizeof(node));

	for(i=0; i<rig; i++){        // Prima le righe
		for(j=0; j<col; j++){     // Poi le colonne
			node* current_node = &map[i * col + j];  // Riga * num_colonne + colonna
			current_node->cost = 1;
			current_node->air_count = 0;
			current_node->air_h = NULL;
			current_node->change_colour = 'w';
		}
	}
	return map;
}

//funzione per deallocare mappa
void deallocate(node* map, int rig, int col){
	int i,j;
	if(map == NULL) return; //se mappa è vuota non faccio niente

	for(i=0;i<rig;i++){
		for(j=0;j<col;j++){
			node* current_node = &map[i * col + j];

			air_route* ptr = current_node->air_h;

			while(ptr != NULL){ //libero rotte aeree
				air_route* next_ptr = ptr->next;
				free(ptr);
				ptr = next_ptr;
			}
		}
	}
	free(map);
}

void reset_colours(int pos, node* map, int rig, int col) {
	if (pos<0  || map[pos].change_colour == 'w') {
		return;
	}
	// Resetta il colore corrente
	map[pos].change_colour = 'w';

	// Resetta i vicini in modo ricorsivo
	for (int h = 0; h < 6; h++) {
		int x = adj(map, pos, rig, col, h);
		if (x != -1 ) {
			reset_colours(x, map, rig, col);
		}
	}
}

int inf(float x){ //trovare inferiore anche se il numero è negativo
	if(x>=0) return (int) x;
	else if(x == (int) x) return x;
	else{
		return (int) x - 1;
	}
}

//funzione change_cost
void change_cost(node* map, int x, int y, int v, int raggio, int rig, int col) {
	node* target_node = &map[y * col + x];
	if(target_node == NULL || raggio <= 0) {
		return;
	}
	printf("OK\n");
	//Precalcolo dei delta in un array in modo da calcolarli una sola volta
	int* delta_cost = malloc(raggio * sizeof(int));
	int i, h;

	if(delta_cost == NULL) {
		printf("Errore allocazione delta_cost\n");
	}

	for(i = 0; i < raggio; i++) {
		delta_cost[i] =  inf(v * (float)(raggio - i)/raggio); //uso floor per arrotondare all'intero inferiore anche per i negativi
	}

	queue Q; // Coda per visita in ampiezza liv per liv
	init_queue(&Q);
	target_node->change_colour = 'g'; //nodo sorgente grigio
	enqueue(&Q, target_node, 0); //metto in coda il nodo sorgente con distanza nulla

	while(!empty(&Q)) {
		queuenode* ptr;
		ptr = dequeue(&Q); //prendo puntatore
		if(ptr != NULL){
			node* hex = ptr->hex; //esagono puntato
			int dist = ptr->dist; //distanza
			int hex_pos = pos(hex, map, col);

			if(dist < raggio) {

				int delta = delta_cost[dist]; //trovo valore giusto nell'array di distanze
				hex->cost += delta; //sommo delta al costo esagono (ciò si applica anche per le rotte aeree perchè il costo rimane quello)

				if(hex->cost < 0) hex->cost = 0; //sistemo i costi per limiti
				if(hex->cost > 100) hex->cost = 100;

				if(dist < raggio - 1) { //per il penultimo livello basta cambiare i vicini
					for(h = 0; h < 6; h++) {
						int adj_pos = adj(map, hex_pos, rig, col, h);
						if(adj_pos != -1 && map[adj_pos].change_colour == 'w') {
							map[adj_pos].change_colour = 'g';
							enqueue(&Q, &map[adj_pos], dist + 1);
						}
					}
				}
			}
			free(ptr); // Libera solo dopo aver finito di usare i dati
			hex->change_colour = 'b';
		}
	}
	free(delta_cost);
	reset_colours(pos(target_node, map, col), map, rig, col); //resetto i colori come tutto bianco
}

void toggle_air_route(node* map, int xp, int yp, int xd, int yd, int col){
	node* source_node = &map[yp * col + xp];
	node* dest_node = &map[yd * col + xd];

	if(source_node->air_count < 5){ //si può inserire o eliminare
		air_route* ptr = source_node->air_h;
		while(ptr!=NULL && ptr->d != dest_node){
			ptr = ptr->next;
		}
		if(ptr!=NULL) { //se trova una rotta già aperta
			air_pop(&source_node->air_h, ptr); //elimina elemento
			source_node->air_count -= 1;
			printf("OK\n");
		}
		else{
			air_route* n_node = malloc(sizeof(air_route));
			n_node->next = source_node->air_h; //inizzializzo node
			n_node->d = dest_node;
			source_node->air_h = n_node;
			source_node->air_count += 1;
			printf("OK\n");
		}
	}
	else{
		air_route* ptr = source_node->air_h;
		while(ptr!=NULL && ptr->d!=dest_node){
			ptr = ptr->next;
		}
		if(ptr!=NULL) { //se trova una rotta già aperta
			air_pop(&source_node->air_h, ptr); //elimina elemento
			source_node->air_count -= 1;
			printf("OK\n");
		}
		else{
			printf("KO\n");
		}
	}
}

int check_coord(int p1, int p2, int rig, int col){ //controllo se sono nei limiti della mappa
	return (p1>=0 && p1<col && p2>=0 && p2<rig);
}

int travel_cost(node* map, int p_pos, int d_pos, int rig, int col, ht* hasht){ //costo minimo tra l'esagono di partenza e quello di destinaznione
	if(p_pos==d_pos){
		return 0; //caso banale
	}
	if(map[p_pos].cost == 0) { //caso in cui il costo dell'esagono di partenza è nullo
		return -1;
	}
	int i;
	//provo a vedere se è gia nella cache
	int find = cache_find(hasht, p_pos, d_pos);
	if(find != -1) return find; //utilizzo cache altrimenti vado avanti

	int* dist = malloc(rig*col*sizeof(int)); //array dove salvo ogni distanza
	for(i=0;i<rig*col;i++) dist[i] = INF; //array delle distanze ad infinito
	dist[p_pos] = 0;

	minheap* h = min_heap_init(rig*col);  //inizializzo heap con l nodi al massimo e la dimensione della tabella hash ridotta
	min_heap_insert(h, p_pos, 0); //nodo sorgente distanza 0

	while(h->heapsize>0){ //finchè c'è qualcosa all' interno del min heap
		heapnode h_node = heap_min(h);//estraggo nodo con distanza minima (il primo)
		int u_pos = h_node.pos; //salvo puntatore al nodo
		int e_dist_node = h_node.dist; //salvo distanza nodo
		h->pos_heap[u_pos] = -1; // aggiorno come non presente nel heap perchè rimosso

		if(u_pos == d_pos){ //se nodo estratto è la destinazione ho finito
			heap_free(h); //libero spazio heap
			free(dist);
			cache_insert(hasht, p_pos, d_pos, e_dist_node);
			return e_dist_node;
		}

		if(e_dist_node > dist[u_pos]){ //se distanza nodo e' maggiore del vettore delle distanze ho gia trovato di meglio e non lo considero
			continue;  // salta il ciclo e prendi il prossimo nodo
		}
		//gestione vicini
		int v_pos;
		if (map[u_pos].cost != 0){
			int x = u_pos % col;
			int temp_dist = map[u_pos].cost + e_dist_node; //costo temporaneo per passare da quel nodo
			// vicino 0 calcolato per ottimizzare memoria
			if (x < col - 1) {
				v_pos = u_pos + 1;
				if (temp_dist < dist[v_pos]) {
					dist[v_pos] = temp_dist; //aggiorno distanza < di quella salvata in precedenza
					if (h->pos_heap[v_pos] == -1) { //se nodo non presente lo inserisco
						min_heap_insert(h, v_pos, temp_dist);
					} else { //se gia presente decremento la key
						decrease_key(h, v_pos, temp_dist);
					}
				}
			}

			// vicino 3 calcolato
			if (x > 0){
				v_pos = u_pos - 1;
				if (temp_dist < dist[v_pos]){
					dist[v_pos] = temp_dist; //aggiorno distanza < di quella salvata in precedenza
					if (h->pos_heap[v_pos] == -1){ //se nodo non presente lo inserisco
						min_heap_insert(h, v_pos, temp_dist);
					} else { //se gia presente decremento la key
						decrease_key(h, v_pos, temp_dist);
					}
				}
			}

			// vicini precalcolati in v (1,2,4,5)
			for (i = 0; i < map[u_pos].count; i++){
				v_pos = map[u_pos].v[i]; //estraggo nodi vicini precalcolati
				if (v_pos != -1){ //se il vicino esiste e posso uscire dal nodo
					if (temp_dist < dist[v_pos]){
						dist[v_pos] = temp_dist; //aggiorno distanza < di quella salvata in precedenza
						if (h->pos_heap[v_pos] == -1){ //se nodo non presente lo inserisco
							min_heap_insert(h, v_pos, temp_dist);
						} else { //se gia presente decremento la key
							decrease_key(h, v_pos, temp_dist);
						}
					}
				}
			}
		}
		//gestione rotte aeree
		air_route* ptr = map[u_pos].air_h; //estraggo nodi aerei vicini
		while(ptr != NULL){ //se posso uscire dal nodo
			int temp_dist = map[u_pos].cost + e_dist_node; //costo temporaneo per passare da quel nodo
			int air_v_pos = pos(ptr->d, map, col); //calcolo posizione del vicino nel vettore delle distanze
			if (temp_dist < dist[air_v_pos]){
				dist[air_v_pos] = temp_dist; //aggiorno distanza < di quella salvata in precedenza
				if (h->pos_heap[air_v_pos] == -1){
					min_heap_insert(h, air_v_pos, temp_dist);
				} else {
					decrease_key(h, air_v_pos, temp_dist);
				}
			}
			ptr = ptr -> next;
		}
	}
	heap_free(h);
	free(dist);
	return -1;
}
void cache_clear(ht* hasht){ //imposto tutti i buckets a NULL invece di deallocare tutto
	int i;
	if(hasht != NULL){
		for(i=0;i<HT_SIZE;i++){
			cache_ht* prec = hasht->buckets[i];
			while(prec!=NULL){
				cache_ht* ptr = prec->next;
				free(prec);
				prec = ptr;
			}
			hasht->buckets[i] = NULL;
		}
	}
}
int main(int argc, char* argv[]){
	char input[MAXINPUT];
	int type;
	int p1,p2,p3,p4;
	int col = 0,rig = 0;
	int flag_map = 0;
	node* map = NULL; //inizializzazione mappa a NULL
	ht* cache = NULL;

	while(fgets(input, sizeof(input), stdin)!= NULL){
		format(input,&p1,&p2,&p3,&p4,&type); //formatto stringa in input e separo in parametri e tipo di comando

		if(type == 1){  //funzione init con due parametri
			flag_map = 1; // mappa inizializzata
			deallocate(map, rig, col); //se non serve non fa niente, righe e colonne salvate
			cache_deallocate(cache);
			col = p1;
			rig = p2;
			map = init(rig,col);
			cache = cache_init();
			init_adj_cache(map, rig, col);
			printf("OK\n");
		}
		else if(type == 2){ //funzione change cost con quattro parametri
			if(flag_map && p4>0 && check_coord(p1, p2, rig, col) && abs(p3)<=10){ //controlli casi
				change_cost(map,p1,p2,p3,p4, rig, col);
				cache_clear(cache);
			}
			else{
				printf("KO\n");
			}
		}
		else if(type == 3){ //funzione toggle air route con quattro parametri
			if(flag_map && check_coord(p1, p2, rig, col) && check_coord(p3, p4, rig, col)){
				toggle_air_route(map, p1, p2, p3, p4, col);
				cache_clear(cache);
			}
			else{
				printf("KO\n");
			}
		}
		else if(type == 4){ //funzione travel cost con quattro parametri
			if(flag_map && check_coord(p1, p2, rig, col) && check_coord(p3, p4, rig, col)){
				printf("%d\n", travel_cost(map, p2 * col + p1, p4 * col + p3, rig, col, cache));
			}
			else{
				printf("-1\n");
			}
		}
	}
	deallocate(map,rig,col); //dealloco la mappa
	cache_deallocate(cache); //dealloco completamente la cache
	return 0;
}
