/*
	Hashtable API
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
bool vertex_exists(vertex* index, uint64_t id) {
	if(index != NULL) {
		if(index->id == id) return true;
		while(index->next) {
		    if(index->id == id) return true;
		    index = index->next;
		}
	} return false;
}

// Adds vertex, returns false is vertex existed
bool add_vertex(uint64_t id) {
	int hash = hash_vertex(id);
	vertex** table = map.table;

	if(vertex_exists(table[hash], id)) return false;

	vertex* new = malloc(sizeof(vertex));
	if(!new) DIE();
	new->id = id;
	new->next = table[hash];
	new->head = NULL;
	table[hash] = new;
	map.size += 1;
	return true;
}

// Helper, returns false if vertex does not exist
// TODO: delete all edges adjacent to vertex
bool delete_vertex(vertex** head, uint64_t id)
{
    if(*head == NULL) return false;
    else {
        vertex* tmp = *head;
        if(tmp->id == id) {
            tmp = tmp->next;
            free(*head);
            *head = tmp;
            return true;
        }

        while(tmp->next)
        {
            if(tmp->id == id) {
                vertex* tmp2 = tmp->next->next;
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