// size of hashtable
#define SIZE (100000)

// prototype for exit function
void DIE();

/*
	Hashtable API prototypes
*/

// edge node definition
typedef struct edge {
	uint64_t b;				// adjacent vertex
	struct edge* next;		// for chaining
} edge;

// vertex node definition
typedef struct vertex {
	uint64_t id;			// unique id of vertex
	edge* head; 			// linked list of edges
	struct vertex* next;	// for chaining
} vertex;

// vertex hashtable definition
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
bool add_vertex(uint64_t id);
// helper, returns false if vertex does not exist
bool delete_vertex(vertex** head, uint64_t id);
// removes vertex, returns false is vertex does not exist
bool remove_vertex(uint64_t id);
// For testing, print all nodes
void all_nodes();

/*
	Linked-list (edges) API prototypes
*/

// inserts node in given LL
void LL_insert(edge** head, uint64_t n);
// returns true if n is in given linked list
bool LL_contains(edge** head, uint64_t n);
// Adds edge, returns 400, 204 or 200
int add_edge(uint64_t a, uint64_t b);