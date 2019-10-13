#ifndef __BPT_H__
#define __BPT_H__
// Uncomment the line below if you are compiling on Windows.
#define WINDOWS
#include <fm.h>
#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif
// Default order is 4.
#define DEFAULT_ORDER 4
// Minimum order is necessarily 3.  We set the maximum
// order arbitrarily.  You may change the maximum order.
#define MIN_ORDER 3
#define MAX_ORDER 20

// TYPES.
/* Type representing the record
 * to which a given key refers.
 * In a real B+ tree system, the
 * record would hold data (in a database)
 * or a file (in an operating system)
 * or some other information.
 * Users can rewrite this part of the code
 * to change the type and content
 * of the value field.
 */
typedef struct record {
    keyNum key;
    char value[120];
} record;
/* Type representing a node in the B+ tree.
 * This type is general enough to serve for both
 * the leaf and the internal node.
 * The heart of the node is the array
 * of keys and the array of corresponding
 * pointers.  The relation between keys
 * and pointers differs between leaves and
 * internal nodes.  In a leaf, the index
 * of each key equals the index of its corresponding
 * pointer, with a maximum of order - 1 key-pointer
 * pairs.  The last pointer points to the
 * leaf to the right (or NULL in the case
 * of the rightmost leaf).
 * In an internal node, the first pointer
 * refers to lower nodes with keys less than
 * the smallest key in the keys array.  Then,
 * with indices i starting at 0, the pointer
 * at i + 1 points to the subtree with keys
 * greater than or equal to the key in this
 * node at index i.
 * The num_keys field is used to keep
 * track of the number of valid keys.
 * In an internal node, the number of valid
 * pointers is always num_keys + 1.
 * In a leaf, the number of valid pointers
 * to data is always num_keys.  The
 * last leaf pointer points to the next leaf.
 */
typedef struct node {
    void ** pointers;
    int * keys;
    struct node * parent;
    bool is_leaf;
    int num_keys;
    struct node * next; // Used for queue.
} node;

typedef struct pageNode {
    pagenum_t page;
    int height;
    struct pageNode * nextNode;
} pageNode;
// GLOBALS.
/* The order determines the maximum and minimum
 * number of entries (keys and pointers) in any
 * node.  Every node has at most order - 1 keys and
 * at least (roughly speaking) half that number.
 * Every leaf has as many pointers to data as keys,
 * and every internal node has one more pointer
 * to a subtree than the number of keys.
 * This global variable is initialized to the
 * default value.
 */
extern int order;
/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
extern pageNode * queue;
/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
extern bool verbose_output;
// FUNCTION PROTOTYPES.
// Output and utility.

void usage(void);

void enqueue(offset_t new_node, int rank);

node * _dequeue( void );
offset_t dequeue(int * rank);

int _height( node * root );
int height(offset_t root);

int path_to_root( node * root, node * child );
void print_leaves( node * root );
void printLeaves(offset_t root);
void printTree(offset_t root);

void find_and_print(node * root, int key, bool verbose);
int findAndPrint(offset_t root, keyNum key, char * ret_val);

void find_and_print_range(node * root, int range1, int range2, bool verbose); 
int find_range( node * root, int key_start, int key_end, bool verbose,
        int returned_keys[], void * returned_pointers[]); 


node * find_leaf( node * root, int key, bool verbose );
pagenum_t findLeaf(pagenum_t root, keyNum key);


record * find( node * root, int key, bool verbose );
Record * findRecord(pagenum_t root, keyNum key);

int cut( int length );

/* INSERTION*/

record * make_record(keyNum key, char * value);
node * make_node( void );
node * make_leaf( void );


int get_left_index(node * parent, node * left);
int getLeftIndex(pagenum_t parent, pagenum_t left);


node * insert_into_leaf( node * leaf, int key, record * pointer );
offset_t insertIntoLeaf(offset_t leaf, Record * record);


node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer);
offset_t insertIntoLeafAfterSplitting(offset_t root, offset_t leaf, Record * record);

node * insert_into_node(node * root, node * parent, int left_index, int key, node * right);
offset_t insertIntoNode(offset_t root, offset_t parent, int left, keyNum key, offset_t right);

node * insert_into_node_after_splitting(node * root, node * parent, int left_index, int key, node * right);
offset_t insertIntoNodeAfterSplitting(offset_t root, offset_t parent, int left_index, keyNum key, offset_t right);

node * insert_into_parent(node * root, node * left, int key, node * right);
offset_t insertIntoParent(offset_t root, offset_t left, keyNum key, offset_t right);

node * insert_into_new_root(node * left, int key, node * right);
offset_t insertIntoNewRoot(offset_t left, keyNum key, offset_t right);

node * start_new_tree(int key, record * pointer);
offset_t startNewTree(Record * record);

node * insert( node * root, keyNum key, char * value );
offset_t Insert(offset_t root, Record * record);



/*  DELETION*/

int get_neighbor_index( node * n );
keyNum getNeighborIndex(offset_t n);

node * remove_entry_from_node(node * n, int key, node * pointer);
offset_t removeEntryFromNode(offset_t leaf, Record * record);

node * adjust_root(node * root);
offset_t adjustRoot(offset_t root);

node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime);
offset_t coalesceNodes(offset_t root, offset_t n);

node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime_index, int k_prime);
// Do not need redistributeNodes!!!
offset_t redistributeNodes(offset_t root, offset_t n, offset_t neighbor, keyNum neighbor_index, keyNum k_prime_index, keyNum k_prime);

node * delete_entry( node * root, node * n, int key, void * pointer );
offset_t deleteEntry(offset_t root, offset_t n, Record * record);

offset_t deleteRecord(offset_t root, keyNum key);

node * _delete( node * root, int key );
offset_t delete(keyNum key);

void destroy_tree_nodes(node * root);
node * destroy_tree(node * root);


#endif /* __BPT_H__*/