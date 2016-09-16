/*
	Hashtable API
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct vertex {
	uint64_t id;
	struct vertex* next;
} vertex;

typedef struct vertex_map {
	vertex** table;
	size_t size;
} vertex_map;

// Returns hash value
int hash_vertex(uint64_t id);
// return true if vertices the same 
bool same_vertex(uint64_t a, uint64_t b);
// return true if vertex exists
bool vertex_exists(vertex* index, uint64_t id);
// adds vertex, returns false is vertex existed
bool add_vertex(vertex_map* map, uint64_t id);
// helper, returns false if vertex does not exist
bool delete_vertex(vertex** head, uint64_t id);
// removes vertex, returns false is vertex does not exist
bool remove_vertex(vertex_map* map, uint64_t id);
// For testing, print all nodes
void all_nodes(vertex_map* map);

// global hashtable for vertices
vertex_map vertexmap;

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
	vertex** table = vertexmap.table;

	if(vertex_exists(table[hash], id)) return false;

	vertex* new = malloc(sizeof(vertex));
	if(!new) DIE();
	new->id = id;
	new->next = table[hash];
	table[hash] = new;
	vertexmap.size += 1;
	return true;
}

// Helper, returns false if vertex does not exist
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
	vertex** table = vertexmap.table;

	if(delete_vertex(&(table[hash]), id)) {
		vertexmap.size -= 1;
		return true;
	} else return false;
}

// For testing, print all nodes
void all_nodes() {
	vertex** table = vertexmap.table;

	for(int i=0; i<SIZE; i++) {
		vertex* ptr = table[i];
		while(ptr) {
			printf("At index %d, value (%llu)\n", i, table[i]->id);
			ptr = ptr->next;
		}
	}
}
