// size of hashtable
#define SIZE (100000)

// prototype for exit function
void DIE();

/*
	Hashtable API prototypes
*/

//Queue for doing BFS and tracking nodes
struct elt {
    struct elt *next;
    uint64_t value;
};

typedef struct queue{
    struct elt *head;
    struct elt *tail;
} queue;


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
	int path;
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
bool vertex_exists(uint64_t id);
// retrieve a vertex
vertex * get_vertex(uint64_t id);
// adds vertex, returns false is vertex existed
bool add_vertex(uint64_t id);
// helper, returns false if vertex does not exist
bool delete_vertex(vertex** head, uint64_t id);
// removes vertex, returns false is vertex does not exist
bool remove_vertex(uint64_t id);
// finds shortest path between two nodes
int shortest_path(uint64_t id1, uint64_t id2);
// For testing, print all nodes
void all_nodes();

/*
	Linked-list (edges) API prototypes
*/

// Inserts node in given LL
void LL_insert(edge** head, uint64_t n);
// Returns true if n is in given linked list
bool LL_contains(edge** head, uint64_t n);
// Removes n from linked list
bool LL_delete(edge** head, uint64_t n);
// Adds edge, returns 400, 204 or 200
int add_edge(uint64_t a, uint64_t b);
// Removes edge, returns false if it didn't exist
bool remove_edge(uint64_t a, uint64_t b);

/* Methods for Queue used for BFS */

queue * queueCreate(void);
void enqueue(queue **q, uint64_t value);
uint64_t dequeue(queue **q);
void queue_destroy(queue **q);
