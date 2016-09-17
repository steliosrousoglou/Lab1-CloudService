#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "headers.h"


/*
	Hashtable API
*/

// global hashtable for vertices
vertex_map map;

// Returns hash value
int hash_vertex(uint64_t id) {
	return id % SIZE;
}

// Returns true if vertex exists
bool vertex_exists(uint64_t id) {
	int hash = hash_vertex(id);
	vertex** table = map.table;
	vertex* index = table[hash];

	if(index != NULL) {
		if(index->id == id) return true;
		while(index->next) {
		    if(index->id == id) return true;
		    index = index->next;
		}
	} return false;
}

vertex * get_vertex(uint64_t id){
	if (!vertex_exists(id)){
		return NULL;
	}
	int hash = hash_vertex(id);
	vertex** table = map.table;
	vertex* index = table[hash];
	if(index != NULL) {
		if(index->id == id) return index;
		while(index->next) {
			if(index->id== id) return index;
			index = index->next;
		}
	}
	return NULL;
	//the above situation should never be returned
}

// Adds vertex, returns false is vertex existed
bool add_vertex(uint64_t id) {
	int hash = hash_vertex(id);
	vertex** table = map.table;

	if(vertex_exists(id)) return false;

	vertex* new = malloc(sizeof(vertex));
	if(!new) DIE();
	new->id = id;
	new->next = table[hash];
	new->head = NULL;
	new->path = -1;
	table[hash] = new;
	map.size += 1;
	return true;
}

//helper code 
void fix_edges(vertex *out){
	edge *head = out->head;
	uint64_t id = out->id;
	vertex *tmp;
	while(head){
		LL_delete(&((get_vertex(head->b))->head), id);
		head=head->next;
	}
}

// Helper, returns false if vertex does not exist
// TODO: delete all edges adjacent to vertex
bool delete_vertex(vertex** head, uint64_t id)
{
    if(*head == NULL) return false;
    else {
        vertex* tmp = *head;
        if(tmp->id == id) {
            fix_edges(tmp);
            tmp = tmp->next;
            free(*head);
            *head = tmp;
            return true;
        }

        while(tmp->next)
        {
            if(tmp->next->id == id) {
                vertex* tmp2 = tmp->next->next;
                fix_edges(tmp->next);
                free(tmp->next);
                tmp->next = tmp2;
                return true;
            }
            tmp = tmp->next;
        }
    }
    return false;
}

// Removes vertex, returns false is vertex does not exist
bool remove_vertex(uint64_t id) {
	int hash = hash_vertex(id);
	vertex** table = map.table;

	if(delete_vertex(&(table[hash]), id)) {
		map.size -= 1;
		return true;
	} else return false;
}

// For testing, print all nodes
void all_nodes() {
	vertex** table = map.table;

	for(int i=0; i<SIZE; i++) {
		vertex* ptr = table[i];
		while(ptr) {
			printf("At index %d, value (%llu)\n", i, table[i]->id);
			ptr = ptr->next;
		}
	}
}


/*
	Linked-list API
*/

// Inserts n into linked list
void LL_insert(edge** head, uint64_t n){
    edge* newnode = malloc(sizeof(edge));
    newnode->b = n;
    newnode->next = NULL;

    if(*head==NULL)
    {
        *head = newnode;
    }
    else
    {
        edge* temp; 
        temp = *head;
        *head = newnode;
        newnode->next = temp;
    }
}

// Returns true if n is in the linked list
bool LL_contains(edge** head, uint64_t n) {
    edge* ptr = *head; 
    while(ptr) {
        if(ptr->b == n) return true;
        ptr = ptr->next;
    }
    return false;
}

// Removes n from linked list
bool LL_delete(edge** head, uint64_t n)
{
    if(*head == NULL) return false;
    else {
        edge* tmp = *head;
        if(tmp->b == n) {
            tmp = tmp->next;
            free(*head);
            *head = tmp;
            return true;
        }

        while(tmp->next)
        {
            if(tmp->next->b == n) {
                edge* tmp2 = tmp->next->next;
                free(tmp->next);
                tmp->next = tmp2;
                return true;
            }
            tmp = tmp->next;
        }
    }
    return false;
}

// Adds edge, returns 
int add_edge(uint64_t a, uint64_t b) {
	vertex** table = map.table;

	vertex* v1 = table[hash_vertex(a)];
	vertex* v2 = table[hash_vertex(b)];

	// find vertex a
	while(v1) {
		if (v1->id == a) break;
		v1 = v1->next;
	}
	// find vertex b
	while(v2) {
		if (v2->id == b) break;
		v2 = v2->next;
	}

	// code 400
	if(!v1 || !v2 || a == b) return 400;

	if(LL_contains(&(v1->head), b)) return 204;

	LL_insert(&(v1->head), b);
	LL_insert(&(v2->head), a);
	return 200;
}

// Removes edge, returns false if it didn't exist
bool remove_edge(uint64_t a, uint64_t b) {
	vertex** table = map.table;
	
	vertex* v1 = table[hash_vertex(a)];
	vertex* v2 = table[hash_vertex(b)];

	// find vertex a
	while(v1) {
		if (v1->id == a) break;
		v1 = v1->next;
	}
	// find vertex b
	while(v2) {
		if (v2->id == b) break;
		v2 = v2->next;
	}

	// can't remove edge
	if(!v1 || !v2) return false;

	return (LL_delete(&(v1->head), b) && LL_delete(&(v2->head), a));
}
// assumes both nodes exist; check if they dont in the other thing;
// TODO: figure out if assingning a parent the value of -1 is a problem
// because of the uint_64 thing, and generally if the int/uint_64 
// thing is a problem 
int shortest_path(uint64_t id1, uint64_t id2){
	
	queue *resetqueue = queueCreate();
	queue *bfsQ = queueCreate();
	vertex *root=get_vertex(id1);

	root->path=0;
	
	vertex *current;
	vertex *n;
	edge *runner;


	enqueue(&bfsQ, root->id);
	enqueue(&resetqueue, root->id);
	int shortest_path = -1;
	while (bfsQ->head != 0){
		current=get_vertex(dequeue(&bfsQ));
		for (runner = current->head; 
			runner != NULL; 
			runner = runner->next){
			n=get_vertex(runner->b);
			if (n->path == -1){
				n->path = current->path + 1;
				enqueue(&resetqueue, n->id);
				enqueue(&bfsQ, n->id);
			}
		}
	}
	shortest_path=(get_vertex(id2))->path;
	vertex *reset;
	while(resetqueue->head !=0){
		reset = get_vertex(dequeue(&resetqueue));
		reset->path = -1;
	}
	queue_destroy(&resetqueue);
	queue_destroy(&bfsQ);
	return shortest_path;

}


queue * queueCreate(void){
	queue *q;
	q = malloc(sizeof(queue));
	q->head = q->tail = 0;
	return q;

}

void enqueue(queue **q, uint64_t value){
	struct elt *e;
	e = malloc(sizeof(struct elt));
	assert(e);
	e->value = value;
	e->next = 0;
	if((*q)->head == 0) {
		(*q)->head = e;
	}
	else {
		(*q)->tail->next = e;
	}
	(*q)->tail = e;
}
uint64_t dequeue(queue **q){
	uint64_t ret;
	struct elt *e;
	assert(!((*q)->head == 0));

	ret= (*q)->head->value;

	e = (*q)->head;
	(*q)->head = e->next;
	free(e);
	return ret;

}

void queue_destroy(queue **q){
	while(!(*q)->head == 0){
		dequeue(q);

	}
	free(*q);
}