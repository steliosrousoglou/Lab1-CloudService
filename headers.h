// size of hashtable
#define SIZE (100000)

// prototype for exit function
void DIE();

/*
	Hashtable API prototypes
*/

// vertex node definition
typedef struct vertex {
	uint64_t id;
	struct vertex* next;
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