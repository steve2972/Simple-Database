/*
 *  bpt.c  
 */
#define Version "1.14"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010-2016  Amittai Aviram  http://www.amittai.com
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, 
 *  this list of conditions and the following disclaimer in the documentation 
 *  and/or other materials provided with the distribution.
 
 *  3. Neither the name of the copyright holder nor the names of its 
 *  contributors may be used to endorse or promote products derived from this 
 *  software without specific prior written permission.
 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE.
 
 *  Author:  Amittai Aviram 
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *  Original Date:  26 June 2010
 *  Last modified: 17 June 2016
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *  
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

#include "bpt.h"

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
int order = DEFAULT_ORDER;

/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
node * queue = NULL;

/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
bool verbose_output = false;


// FUNCTION DEFINITIONS.

// OUTPUT AND UTILITIES


/* Helper function for printing the
 * tree out.  See print_tree.
 */
void enqueue( node * new_node ) {
    node * c;
    if (queue == NULL) {
        queue = new_node;
        queue->next = NULL;
    }
    else {
        c = queue;
        while(c->next != NULL) {
            c = c->next;
        }
        c->next = new_node;
        new_node->next = NULL;
    }
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
node * dequeue( void ) {
    node * n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}


/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
void print_leaves( node * root ) {
    int i;
    node * c = root;
    if (root == NULL) {
        printf("Empty tree.\n");
        return;
    }
    while (!c->is_leaf)
        c = c->pointers[0];
    while (true) {
        for (i = 0; i < c->num_keys; i++) {
            if (verbose_output)
                printf("%lx ", (unsigned long)c->pointers[i]);
            printf("%d ", c->keys[i]);
        }
        if (verbose_output)
            printf("%lx ", (unsigned long)c->pointers[order - 1]);
        if (c->pointers[order - 1] != NULL) {
            printf(" | ");
            c = c->pointers[order - 1];
        }
        else
            break;
    }
    printf("\n");
}


/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
int height( node * root ) {
    int h = 0;
    node * c = root;
    while (!c->is_leaf) {
        c = c->pointers[0];
        h++;
    }
    return h;
}


/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
int path_to_root( node * root, node * child ) {
    int length = 0;
    node * c = child;
    while (c != root) {
        c = c->parent;
        length++;
    }
    return length;
}


/* Prints the B+ tree in the command
 * line in level (rank) order, with the 
 * keys in each node and the '|' symbol
 * to separate nodes.
 * With the verbose_output flag set.
 * the values of the pointers corresponding
 * to the keys also appear next to their respective
 * keys, in hexadecimal notation.
 */
void print_tree( node * root ) {

    node * n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL) {
        printf("Empty tree.\n");
        return;
    }
    queue = NULL;
    enqueue(root);
    while( queue != NULL ) {
        n = dequeue();
        if (n->parent != NULL && n == n->parent->pointers[0]) {
            new_rank = path_to_root( root, n );
            if (new_rank != rank) {
                rank = new_rank;
                printf("\n");
            }
        }
        if (verbose_output) 
            printf("(%lx)", (unsigned long)n);
        for (i = 0; i < n->num_keys; i++) {
            if (verbose_output)
                printf("%lx ", (unsigned long)n->pointers[i]);
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->num_keys; i++)
                enqueue(n->pointers[i]);
        if (verbose_output) {
            if (n->is_leaf) 
                printf("%lx ", (unsigned long)n->pointers[order - 1]);
            else
                printf("%lx ", (unsigned long)n->pointers[n->num_keys]);
        }
        printf("| ");
    }
    printf("\n");
}


/* Finds the record under a given key and prints an
 * appropriate message to stdout.
 */
void find_and_print(node * root, int key, bool verbose) {
    record * r = find(root, key, verbose);
    if (r == NULL)
        printf("Record not found under key %d.\n", key);
    else 
        printf("Record at %lx -- key %d, value %s.\n",
                (unsigned long)r, key, r->value);
}


/* Finds and prints the keys, pointers, and values within a range
 * of keys between key_start and key_end, including both bounds.
 */
void find_and_print_range( node * root, int key_start, int key_end,
        bool verbose ) {
    int i;
    int array_size = key_end - key_start + 1;
    int returned_keys[array_size];
    void * returned_pointers[array_size];
    int num_found = find_range( root, key_start, key_end, verbose,
            returned_keys, returned_pointers );
    if (!num_found)
        printf("None found.\n");
    else {
        for (i = 0; i < num_found; i++)
            printf("Key: %d   Location: %lx  Value: %s\n",
                    returned_keys[i],
                    (unsigned long)returned_pointers[i],
                    ((record *)
                     returned_pointers[i])->value);
    }
}


/* Finds keys and their pointers, if present, in the range specified
 * by key_start and key_end, inclusive.  Places these in the arrays
 * returned_keys and returned_pointers, and returns the number of
 * entries found.
 */
int find_range( node * root, int key_start, int key_end, bool verbose,
        int returned_keys[], void * returned_pointers[]) {
    int i, num_found;
    num_found = 0;
    node * n = find_leaf( root, key_start, verbose );
    if (n == NULL) return 0;
    for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++) ;
    if (i == n->num_keys) return 0;
    while (n != NULL) {
        for ( ; i < n->num_keys && n->keys[i] <= key_end; i++) {
            returned_keys[num_found] = n->keys[i];
            returned_pointers[num_found] = n->pointers[i];
            num_found++;
        }
        n = n->pointers[order - 1];
        i = 0;
    }
    return num_found;
}


node * find_leaf( node * root, int key, bool verbose ) {
    int i = 0;
    node * c = root;
    if (c == NULL) {
        if (verbose) 
            printf("Empty tree.\n");
        return c;
    }
    while (!c->is_leaf) {
        if (verbose) {
            printf("[");
            for (i = 0; i < c->num_keys - 1; i++)
                printf("%d ", c->keys[i]);
            printf("%d] ", c->keys[i]);
        }
        i = 0;
        while (i < c->num_keys) {
            if (key >= c->keys[i]) i++;
            else break;
        }
        if (verbose)
            printf("%d ->\n", i);
        c = (node *)c->pointers[i];
    }
    if (verbose) {
        printf("Leaf [");
        for (i = 0; i < c->num_keys - 1; i++)
            printf("%d ", c->keys[i]);
        printf("%d] ->\n", c->keys[i]);
    }
    return c;
}
pagenum_t findLeaf(pagenum_t root, keyNum key) {
    pagenum_t c = root;
    page_t page;
    file_read_page(c, &page);

    while (!isLeaf(&page)) {
        // Continue looping until leaf page is found
        if (key < getKey(&page, 0)) {
            // Get the page number of the key -> leftmost sibling of page
            c = getOneMorePage(&page);
        }
        else {
            int index = getIndex(&page, key);
            if (index == -1) {
                // Index not in the internal page
                return -1;
            } 
            c = getEntryOffset(&page, index+1);
        }

        // Sync the page with the current page number
        file_read_page(c, &page);
    }

    return c;
}


/* Finds and returns the record to which
 * a key refers.
 */
record * find( node * root, int key, bool verbose ) {
    int i = 0;
    node * c = find_leaf( root, key, verbose );
    if (c == NULL) return NULL;
    for (i = 0; i < c->num_keys; i++)
        if (c->keys[i] == key) break;
    if (i == c->num_keys) 
        return NULL;
    else
        return (record *)c->pointers[i];
}
Record * findRecord(pagenum_t root, keyNum key) {
    pagenum_t leaf = findLeaf(root, key);

    if (leaf == -1) {
        return NULL;
    }

    page_t leafPage;
    file_read_page(leaf, &leafPage);
    int index = getIndex(&leafPage, key);

    if (index == -1) {
        return NULL;
    }

    Record * record = (Record *)malloc(sizeof(Record));
    record->key = getKey(&leafPage, index);
    copyRecord(&leafPage, index, record->value);
    return record;
}

/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}


// INSERTION
record * make_record(keyNum key, char * value) {
    record * new_record = (record *)malloc(sizeof(record));
    if (new_record == NULL) {
        perror("Record creation.");
        exit(EXIT_FAILURE);
    }
    else {
        new_record->key = key;
        strcpy(new_record->value, value);
    }
    return new_record;
}
node * make_node( void ) {
    node * new_node;
    new_node = malloc(sizeof(node));
    if (new_node == NULL) {
        perror("Node creation.");
        exit(EXIT_FAILURE);
    }
    new_node->keys = malloc( (order - 1) * sizeof(int) );
    if (new_node->keys == NULL) {
        perror("New node keys array.");
        exit(EXIT_FAILURE);
    }
    new_node->pointers = malloc( order * sizeof(void *) );
    if (new_node->pointers == NULL) {
        perror("New node pointers array.");
        exit(EXIT_FAILURE);
    }
    new_node->is_leaf = false;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}
node * make_leaf( void ) {
    node * leaf = make_node();
    leaf->is_leaf = true;
    return leaf;
}
int get_left_index(node * parent, node * left) {

    int left_index = 0;
    while (left_index <= parent->num_keys && 
            parent->pointers[left_index] != left)
        left_index++;
    return left_index;
}
int getLeftIndex(pagenum_t parent, pagenum_t left) {
    int left_index = 0;

    page_t parentPage;
    file_read_page(parent, &parentPage);
    while (left_index <= getNumKeys(&parentPage) && 
            getEntryOffset(&parentPage, left_index) != left_index) {
            
            left_index++;
    }

    return left_index;
}


node * insert_into_leaf( node * leaf, int key, record * pointer ) {

    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }
    leaf->keys[insertion_point] = key;
    leaf->pointers[insertion_point] = pointer;
    leaf->num_keys++;
    return leaf;
}
offset_t insertIntoLeaf(offset_t leaf, Record * record) {
    int insertion_point, numKeys;
    page_t leafPage;
    file_read_page(leaf, &leafPage);
    numKeys = getNumKeys(&leafPage);

    // Find the insertion poing
    if (record->key < getKey(&leafPage, 0)) {
        insertion_point = 0;
    }
    else {
        insertion_point = search(&leafPage, record->key) + 1;
        if (insertion_point == 0) {
            return -1;
        }
    }

    // Rearrange the leaf records so that the keys are in numerical order

    for (int i = numKeys; i > insertion_point; i--) {
        // Go from back to front because we need to set the values 
        // Instead of using a temporary memory space
        // [1,2,3]=> [,1,2]
        char temp[120];
        setKey(&leafPage, getKey(&leafPage, i-1), i);
        copyRecord(&leafPage, i-1, temp);
        setRecordValue(&leafPage, temp, i);
    }
    
    // Insert into insertion_point
    setKey(&leafPage, record->key, insertion_point);
    setRecordValue(&leafPage, record->value, insertion_point);
    keyNum keys = leafPage.node.header.NumKeys + 1;
    setNumKeys(&leafPage, keys);

    file_write_page(leaf, &leafPage);
    return leaf;
}

/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer) {

    node * new_leaf;
    int * temp_keys;
    void ** temp_pointers;
    int insertion_index, split, new_key, i, j;

    new_leaf = make_leaf();

    temp_keys = malloc( order * sizeof(int) );
    if (temp_keys == NULL) {
        perror("Temporary keys array.");
        exit(EXIT_FAILURE);
    }

    temp_pointers = malloc( order * sizeof(void *) );
    if (temp_pointers == NULL) {
        perror("Temporary pointers array.");
        exit(EXIT_FAILURE);
    }

    insertion_index = 0;
    while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->keys[i];
        temp_pointers[j] = leaf->pointers[i];
    }

    temp_keys[insertion_index] = key;
    temp_pointers[insertion_index] = pointer;

    leaf->num_keys = 0;

    split = cut(order - 1);

    for (i = 0; i < split; i++) {
        leaf->pointers[i] = temp_pointers[i];
        leaf->keys[i] = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < order; i++, j++) {
        new_leaf->pointers[j] = temp_pointers[i];
        new_leaf->keys[j] = temp_keys[i];
        new_leaf->num_keys++;
    }

    free(temp_pointers);
    free(temp_keys);

    new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
    leaf->pointers[order - 1] = new_leaf;

    for (i = leaf->num_keys; i < order - 1; i++)
        leaf->pointers[i] = NULL;
    for (i = new_leaf->num_keys; i < order - 1; i++)
        new_leaf->pointers[i] = NULL;

    new_leaf->parent = leaf->parent;
    new_key = new_leaf->keys[0];

    return insert_into_parent(root, leaf, new_key, new_leaf);
}
offset_t insertIntoLeafAfterSplitting(offset_t root, offset_t leaf, Record * record) {
    // Same logic as insert_into_leaf_after_splitting() from the bpt code
    page_t leafPage, newLeafPage;
    offset_t newLeafOffset;
    keyNum temp_keys[LEAF_ORDER];
    char temp_records[LEAF_ORDER][120];      //temp values = 31 records of 120 chars each
    int insertion_index, split, new_key, i, j;

    newLeafOffset = createLeafPage();

    file_read_page(leaf, &leafPage);
    file_read_page(newLeafOffset, &newLeafPage);

    // First find where to insert the record
    if (record->key < getKey(&leafPage, 0))
        insertion_index = 0;
    else
        insertion_index = search(&leafPage, record->key) + 1;

    // Adapted from bpt code
    for (i = 0, j = 0; i < getNumKeys(&leafPage); i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = getKey(&leafPage, i);
        copyRecord(&leafPage, i, temp_records[j]);
    }

    temp_keys[insertion_index] = record->key;
    strcpy(temp_records[insertion_index], record->value);

    split = cut(LEAF_ORDER -1);

    int temp = 0;   // Use temp to dynamically get the number of keys;
    for (i = 0; i < split; i++) {
        setRecordValue(&leafPage, temp_records[i], i);
        setKey(&leafPage, temp_keys[i], i);
        temp++;
        setNumKeys(&leafPage, temp);
    }
    temp = 0;
    for (i = split, j = 0; i < order; i++, j++) {
        setRecordValue(&newLeafPage, temp_records[i], j);
        setKey(&newLeafPage, temp_keys[i], j);
        temp++;
        setNumKeys(&newLeafPage, temp);
    }


    setSiblingOffset(&newLeafPage, getEntryOffset(&leafPage, LEAF_ORDER-1), LEAF_ORDER-1);
    setSiblingOffset(&leafPage, newLeafOffset, LEAF_ORDER-1);

    for (i = getNumKeys(&leafPage); i < LEAF_ORDER-1; i++) {
        setKey(&leafPage, 0, i);
        setRecordValue(&leafPage, "", 0);
    }
    for (i = getNumKeys(&newLeafPage); i < LEAF_ORDER-1; i++) {
        setKey(&newLeafPage, 0, i);
        setRecordValue(&newLeafPage, "", 0);
    }

    new_key = getKey(&newLeafPage, 0);
    // parent node of split new page is the same as the original page
    setParentPageNum(&newLeafPage, getParentPageNum(&leafPage));

    file_write_page(leaf, &leafPage);
    file_write_page(newLeafOffset, &newLeafPage);

    return insertIntoParent(root, leaf, new_key, newLeafOffset);
}

node * insert_into_node(node * root, node * n, 
        int left_index, int key, node * right) {
    int i;

    for (i = n->num_keys; i > left_index; i--) {
        n->pointers[i + 1] = n->pointers[i];
        n->keys[i] = n->keys[i - 1];
    }
    n->pointers[left_index + 1] = right;
    n->keys[left_index] = key;
    n->num_keys++;
    return root;
}

offset_t insertIntoNode(offset_t root, offset_t parent, int left_index, keyNum key, offset_t right) {
    int i;

    page_t parentPage;
    file_read_page(parent, &parentPage);
    for (i = getNumKeys(&parentPage); i > left_index; i--) {
        setSiblingOffset(&parentPage, getEntryOffset(&parentPage, i), i+1);
        setKey(&parentPage, getKey(&parentPage, i-1), i);
    }
    setSiblingOffset(&parentPage, right, left_index + 1);
    setKey(&parentPage, key, left_index);
    keyNum numKeys = getNumKeys(&parentPage) + 1;
    setNumKeys(&parentPage, numKeys);
    
    file_write_page(parent, &parentPage);

    return root;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
node * insert_into_node_after_splitting(node * root, node * old_node, int left_index, 
        int key, node * right) {

    int i, j, split, k_prime;
    node * new_node, * child;
    int * temp_keys;
    node ** temp_pointers;

    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places. 
     * Then create a new node and copy half of the 
     * keys and pointers to the old node and
     * the other half to the new.
     */

    temp_pointers = malloc( (order + 1) * sizeof(node *) );
    if (temp_pointers == NULL) {
        perror("Temporary pointers array for splitting nodes.");
        exit(EXIT_FAILURE);
    }
    temp_keys = malloc( order * sizeof(int) );
    if (temp_keys == NULL) {
        perror("Temporary keys array for splitting nodes.");
        exit(EXIT_FAILURE);
    }

    for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pointers[j] = old_node->pointers[i];
    }

    for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = old_node->keys[i];
    }

    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
    split = cut(order);
    new_node = make_node();
    old_node->num_keys = 0;
    for (i = 0; i < split - 1; i++) {
        old_node->pointers[i] = temp_pointers[i];
        old_node->keys[i] = temp_keys[i];
        old_node->num_keys++;
    }
    old_node->pointers[i] = temp_pointers[i];
    k_prime = temp_keys[split - 1];
    for (++i, j = 0; i < order; i++, j++) {
        new_node->pointers[j] = temp_pointers[i];
        new_node->keys[j] = temp_keys[i];
        new_node->num_keys++;
    }
    new_node->pointers[j] = temp_pointers[i];
    free(temp_pointers);
    free(temp_keys);
    new_node->parent = old_node->parent;
    for (i = 0; i <= new_node->num_keys; i++) {
        child = new_node->pointers[i];
        child->parent = new_node;
    }

    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */

    return insert_into_parent(root, old_node, k_prime, new_node);
}
offset_t insertIntoNodeAfterSplitting(offset_t root, offset_t parent, int left_index, keyNum key, offset_t right) {
    int i, j, split, k_prime, keys;
    offset_t new_node, child_offset;
    keyNum temp_keys[INTERNAL_ORDER];
    offset_t temp_pointers[INTERNAL_ORDER+1];

    page_t parentPage, newNode, child;
    file_read_page(parent, &parentPage);

    keys = getNumKeys(&parentPage);

    for (i = 0, j = 0; i < keys + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pointers[j] = getEntryOffset(&parentPage, i);
    }

    for (i = 0, j = 0; i < keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = getKey(&parentPage, i);
    }

    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;

    split = cut(INTERNAL_ORDER);

    new_node = createInternalPage();
    file_read_page(new_node, &newNode);

    keys = 0;
    for (i = 0; i < split - 1; i++) {
        setSiblingOffset(&parentPage, temp_pointers[i], i);
        setKey(&parentPage, temp_keys[i], i);
        keys++;
        setNumKeys(&parentPage, keys);
    }

    setEntryOffset(&parentPage, split-1, temp_pointers[split-1]);
    k_prime = temp_keys[split - 1];
    keys = 0;

    for (++i, j = 0; i < order; i++, j++) {
        setSiblingOffset(&newNode, j, temp_pointers[i]);
        setKey(&newNode, j, temp_keys[i]);
        keys++;
        setNumKeys(&newNode, keys);
    }
    setSiblingOffset(&newNode, j, temp_pointers[i]);

    setParentPageNum(&newNode, getParentPageNum(&parentPage));

    for (i = 0; i <= getNumKeys(&newNode); i++) {
        child_offset = getEntryOffset(&newNode, i);
        file_read_page(child_offset, &child);
        setParentPageNum(&child, new_node);
        file_write_page(child_offset, &child);
    }
    
    // Write to memory
    file_write_page(new_node, &newNode);
    file_write_page(parent, &parentPage);

    return insertIntoParent(root, parent, k_prime, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node * insert_into_parent(node * root, node * left, int key, node * right) {

    int left_index;
    node * parent;

    parent = left->parent;

    /* Case: new root. */

    if (parent == NULL)
        return insert_into_new_root(left, key, right);

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */

    left_index = get_left_index(parent, left);


    /* Simple case: the new key fits into the node. 
     */

    if (parent->num_keys < order - 1)
        return insert_into_node(root, parent, left_index, key, right);

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */

    return insert_into_node_after_splitting(root, parent, left_index, key, right);
}
offset_t insertIntoParent(offset_t root, offset_t left, keyNum key, offset_t right) {
    int left_index;
    offset_t parent;
    page_t parentPage, leftPage;

    // parent = left->parent
    file_read_page(left, &leftPage);

    parent = getParentPageNum(&leftPage);

    /* Case: new root. */

    if (parent == 0)// Parent is equal to the header page => empty tree
        return insertIntoNewRoot(left, key, right);

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */

    left_index = getLeftIndex(parent, left);
    file_read_page(parent, &parentPage);

    /* Simple case: the new key fits into the node. 
     */

    if (getNumKeys(&parentPage) < INTERNAL_ORDER - 1)
        return insertIntoNode(root, parent, left_index, key, right);

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */

    return insertIntoNodeAfterSplitting(root, parent, left_index, key, right);
}



/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
node * insert_into_new_root(node * left, int key, node * right) {

    node * root = make_node();
    root->keys[0] = key;
    root->pointers[0] = left;
    root->pointers[1] = right;
    root->num_keys++;
    root->parent = NULL;
    left->parent = root;
    right->parent = root;
    return root;
}
offset_t insertIntoNewRoot(offset_t left, keyNum key, offset_t right) {
    page_t rootPage;
    offset_t root = createInternalPage();
    // Need the pages of the left and right offsets
    page_t leftPage, rightPage;

    file_read_page(left, &leftPage);
    file_read_page(right, &rightPage);
    file_read_page(root, &rootPage);

    setKey(&rootPage, key, 0);
    setSiblingOffset(&rootPage, left, 0);
    setSiblingOffset(&rootPage, right, 1);    
    setNumKeys(&rootPage, 1);
    setParentPageNum(&rootPage, 0);
    setParentPageNum(&leftPage, root);
    setParentPageNum(&rightPage, root);

    // Remember to reset the root into the new root
    setRootPageOffset(&header, root);
    file_write_page(0, &header);

    file_write_page(root, &rootPage);
    file_write_page(left, &leftPage);
    file_write_page(right, &rightPage);

    return root;
}
/* First insertion:
 * start a new tree.
 */
node * start_new_tree(int key, record * pointer) {

    node * root = make_leaf();
    root->keys[0] = key;
    root->pointers[0] = pointer;
    root->pointers[order - 1] = NULL;
    root->parent = NULL;
    root->num_keys++;
    return root;
}
offset_t startNewTree(Record * record) {
    offset_t root = createLeafPage();
    page_t temp;

    file_read_page(root, &temp);

    setKey(&temp, record->key, 0);
    setRecordValue(&temp, record->value, 0);
    setNumKeys(&temp, 1);
    
    setRootPageOffset(&header, root);
    
    file_write_page(0, &header);
    file_write_page(root, &temp);

    return root;
}


/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
node * insert( node * root, keyNum key, char * value) {

    record * pointer;
    node * leaf;

    /* The current implementation ignores
     * duplicates.
     */

    if (find(root, key, false) != NULL)
        return root;

    /* Create a new record for the
     * value.
     */
    pointer = make_record(key, value);


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root == NULL) 
        return start_new_tree(key, pointer);


    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    leaf = find_leaf(root, key, false);

    /* Case: leaf has room for key and pointer.
     */

    if (leaf->num_keys < order - 1) {
        leaf = insert_into_leaf(leaf, key, pointer);
        return root;
    }


    /* Case:  leaf must be split.
     */

    return insert_into_leaf_after_splitting(root, leaf, key, pointer);
}
offset_t Insert(offset_t root, Record * record) {
    Record * pointer = (Record *)malloc(sizeof(Record));
    page_t leaf;

    /* The current implementation ignores
     * duplicates.
     */

    if (findRecord(root, record->key) != NULL)
        return root;

    /* Create a new record for the
     * value.
     */
    pointer->key = record->key;
    strcpy(pointer->value, record->value);



    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root == 0) 
        return startNewTree(pointer);


    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    offset_t leaf_offset = findLeaf(root, record->key);
    if (leaf_offset == 0) {
        return root;        // final check to see if leaf exists
    }
    file_read_page(leaf_offset, &leaf);

    /* Case: leaf has room for key and pointer.
     */

    if (getNumKeys(&leaf) < LEAF_ORDER -1) {
        insertIntoLeaf(leaf_offset, record);
        return root;
    }


    /* Case:  leaf must be split.
     */

    return insertIntoLeafAfterSplitting(root, leaf_offset, record);


}




// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_neighbor_index( node * n ) {

    int i;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
    for (i = 0; i <= n->parent->num_keys; i++)
        if (n->parent->pointers[i] == n)
            return i - 1;

    // Error state.
    printf("Search for nonexistent pointer to node in parent.\n");
    printf("Node:  %#lx\n", (unsigned long)n);
    exit(EXIT_FAILURE);
}
keyNum getNeighborIndex(offset_t n) {
    int i;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */

    //TODO: fix and check logic of this code

    int num_keys;
    page_t neighbor;
    offset_t c = n;
    file_read_page(c, &neighbor);

    c = getParentPageNum(&neighbor);
    file_read_page(c, &neighbor);
    i = 0, num_keys = getNumKeys(&neighbor);
    while(i<=num_keys && n != getEntryOffset(&neighbor, i)){
        i++;
    }
    if(i!=0)
        return getOffset(&neighbor, i-1);
    else{
        c = getParentPageNum(&neighbor);
        while(c != 0){
            file_read_page(c, &neighbor);
            if(isLeaf(&neighbor))
                return c;
            num_keys = getNumKeys(&neighbor);
            c = getEntryOffset(&neighbor, num_keys);
        }
    }

    return 0; 

}


node * remove_entry_from_node(node * n, int key, node * pointer) {

    int i, num_pointers;

    // Remove the key and shift other keys accordingly.
    i = 0;
    while (n->keys[i] != key)
        i++;
    for (++i; i < n->num_keys; i++)
        n->keys[i - 1] = n->keys[i];

    // Remove the pointer and shift other pointers accordingly.
    // First determine number of pointers.
    num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
    i = 0;
    while (n->pointers[i] != pointer)
        i++;
    for (++i; i < num_pointers; i++)
        n->pointers[i - 1] = n->pointers[i];


    // One key fewer.
    n->num_keys--;

    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
    if (n->is_leaf)
        for (i = n->num_keys; i < order - 1; i++)
            n->pointers[i] = NULL;
    else
        for (i = n->num_keys + 1; i < order; i++)
            n->pointers[i] = NULL;

    return n;
}
offset_t removeEntryFromNode(offset_t leaf, Record * record) {
    int deletion_point;
    page_t leafPage;
    file_read_page(leaf, &leafPage);


    // Remove the key and shift other keys accordingly.
    deletion_point = search(&leafPage, record->key);
    char temp[120];
    for(int i = deletion_point; i < getNumKeys(&leafPage); i++) {
        setKey(&leafPage, getKey(&leafPage, i+1), i);
        copyRecord(&leafPage, i+1, temp);
        setRecordValue(&leafPage, temp, i);

        if (i == getNumKeys(&leafPage) - 1) {
            setKey(&leafPage, 0, i);
            setRecordValue(&leafPage, "", i);
        }
    }

    // One key fewer.
    int numkeys = getNumKeys(&leafPage) - 1;
    setNumKeys(&leafPage, numkeys);

    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.

    // --> implemented in the above for loop

    file_write_page(leaf, &leafPage);
    return leaf;
}


node * adjust_root(node * root) {

    node * new_root;

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

    if (root->num_keys > 0)
        return root;

    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!root->is_leaf) {
        new_root = root->pointers[0];
        new_root->parent = NULL;
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else
        new_root = NULL;

    free(root->keys);
    free(root->pointers);
    free(root);

    return new_root;
}
offset_t adjustRoot(offset_t root) {
    offset_t new_root;
    page_t old_root, new_root_page;
    file_read_page(root, &old_root);

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */
    
    if(getNumKeys(&old_root) > 0) {
        return root;
    }

    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!isLeaf(&old_root)) {
        new_root = getEntryOffset(&old_root, 0);
        file_read_page(new_root, &new_root_page);
        setParentPageNum(&new_root_page, 0);
        file_write_page(new_root, &new_root_page);

        // Synchronize the header page
        setRootPageOffset(&header, new_root);
        file_write_page(0, &header);
    }
    // If it is a leaf (has no children),
    // then the whole tree is empty.
    else
        new_root = 0;   // set the new root to the header page

    file_free_page(root);

    return new_root;
}

/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime) {

    int i, j, neighbor_insertion_index, n_end;
    node * tmp;

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */

    if (neighbor_index == -1) {
        tmp = n;
        n = neighbor;
        neighbor = tmp;
    }

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    neighbor_insertion_index = neighbor->num_keys;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!n->is_leaf) {

        /* Append k_prime.
         */

        neighbor->keys[neighbor_insertion_index] = k_prime;
        neighbor->num_keys++;


        n_end = n->num_keys;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->pointers[i] = n->pointers[j];
            neighbor->num_keys++;
            n->num_keys--;
        }

        /* The number of pointers is always
         * one more than the number of keys.
         */

        neighbor->pointers[i] = n->pointers[j];

        /* All children must now point up to the same parent.
         */

        for (i = 0; i < neighbor->num_keys + 1; i++) {
            tmp = (node *)neighbor->pointers[i];
            tmp->parent = neighbor;
        }
    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->pointers[i] = n->pointers[j];
            neighbor->num_keys++;
        }
        neighbor->pointers[order - 1] = n->pointers[order - 1];
    }

    root = delete_entry(root, n->parent, k_prime, n);
    free(n->keys);
    free(n->pointers);
    free(n); 
    return root;
}
offset_t coalesceNodes(offset_t root, offset_t n, offset_t neighbor, int neighbor_index, int k_prime) {
    // Note: n= node we are deleting
    int i, j, neighbor_insertion_index, n_end;
    keyNum numKeys;
    page_t freePage, parentPage, tmp, neighborPage;
    offset_t parentOffset;
    
    // Initialize pages
    file_read_page(n, &freePage);
    parentOffset = getParentPageNum(&freePage);
    file_read_page(parentOffset, &parentPage);
    file_read_page(neighbor, &neighborPage);

    //TODO: add when parent offset = 0

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */

    if (neighbor_index == -1) {
        // Switch neighbor and n
        file_write_page(n, &neighborPage);
        file_write_page(neighbor, &freePage);
    }

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    file_read_page(n, &freePage);
    file_read_page(neighbor, &neighborPage);
    neighbor_insertion_index = getNumKeys(&neighborPage);

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!isLeaf(&freePage)) {

        /* Append k_prime.
         */
        setKey(&neighborPage, k_prime, neighbor_insertion_index);
        setNumKeys(&neighborPage, getNumKeys(&neighborPage) + 1);

        n_end = getNumKeys(&freePage);

        keyNum numKeysNeighbor = getNumKeys(&neighborPage);
        keyNum numKeysFreePage = n_end;
        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            setKey(&neighborPage, getKey(&freePage, j), i);
            char tempVal[120];
            copyRecord(&freePage, j, tempVal);
            setRecordValue(&neighborPage, tempVal, i);
            setNumKeys(&neighborPage, ++numKeysNeighbor);
            
            setNumKeys(&freePage, --numKeysFreePage);
        }

        /* The number of pointers is always
         * one more than the number of keys.
         */
        
        char tempVal[120];
        copyRecord(&freePage, j, tempVal);
        setRecordValue(&neighborPage, tempVal, i);
        /* All children must now point up to the same parent.
         */

        for (i = 0; i < getNumKeys(&neighborPage) + 1; i++) {
            file_read_page(neighborPage.node.entries[i].page, &tmp);
            setParentPageNum(&tmp, neighbor);
            file_write_page(neighborPage.node.entries[i].page, &tmp);
        }
    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        int numKeysNeighbor = getNumKeys(&neighborPage);
        for (i = neighbor_insertion_index, j = 0; j < getNumKeys(&freePage); i++, j++) {
            setKey(&neighborPage, getKey(&freePage, j), i);
            char tempVal[120];
            copyRecord(&freePage, j, tempVal);
            setRecordValue(&neighborPage, tempVal, i);
            setNumKeys(&neighborPage, ++numKeysNeighbor);

        }
        char tempVal[120];
        copyRecord(&freePage, INTERNAL_ORDER - 1, tempVal);
        setRecordValue(&neighborPage, tempVal, INTERNAL_ORDER-1);
    }

    //TODO: check logic of this code
    Record * record = makeRecord(k_prime, n);
    root = deleteEntry(root, getParentPageNum(&freePage), record);
 
    return root;
}

/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, 
        int k_prime_index, int k_prime) {  

    int i;
    node * tmp;

    /* Case: n has a neighbor to the left. 
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

    if (neighbor_index != -1) {
        if (!n->is_leaf)
            n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
        for (i = n->num_keys; i > 0; i--) {
            n->keys[i] = n->keys[i - 1];
            n->pointers[i] = n->pointers[i - 1];
        }
        if (!n->is_leaf) {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys];
            tmp = (node *)n->pointers[0];
            tmp->parent = n;
            neighbor->pointers[neighbor->num_keys] = NULL;
            n->keys[0] = k_prime;
            n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
        }
        else {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
            neighbor->pointers[neighbor->num_keys - 1] = NULL;
            n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
            n->parent->keys[k_prime_index] = n->keys[0];
        }
    }

    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {  
        if (n->is_leaf) {
            n->keys[n->num_keys] = neighbor->keys[0];
            n->pointers[n->num_keys] = neighbor->pointers[0];
            n->parent->keys[k_prime_index] = neighbor->keys[1];
        }
        else {
            n->keys[n->num_keys] = k_prime;
            n->pointers[n->num_keys + 1] = neighbor->pointers[0];
            tmp = (node *)n->pointers[n->num_keys + 1];
            tmp->parent = n;
            n->parent->keys[k_prime_index] = neighbor->keys[0];
        }
        for (i = 0; i < neighbor->num_keys - 1; i++) {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->pointers[i] = neighbor->pointers[i + 1];
        }
        if (!n->is_leaf)
            neighbor->pointers[i] = neighbor->pointers[i + 1];
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */

    n->num_keys++;
    neighbor->num_keys--;

    return root;
}

/*
 * I will not implement redistribute nodes in my B+ tree because
 * I will be using a delayed merge, where the tree does not merge the leaves
 * unless there are no longer any entries in the node.
 * 
 * In this case, we only need to coalesce nodes.
 */

/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
node * delete_entry( node * root, node * n, int key, void * pointer ) {

    int min_keys;
    node * neighbor;
    int neighbor_index;
    int k_prime_index, k_prime;
    int capacity;

    // Remove key and pointer from node.

    n = remove_entry_from_node(n, key, pointer);

    /* Case:  deletion from the root. 
     */

    if (n == root) 
        return adjust_root(root);


    /* Case:  deletion from a node below the root.
     * (Rest of function body.)
     */

    /* Determine minimum allowable size of node,
     * to be preserved after deletion.
     */

    min_keys = n->is_leaf ? cut(order - 1) : cut(order) - 1;

    /* Case:  node stays at or above minimum.
     * (The simple case.)
     */

    if (n->num_keys >= min_keys)
        return root;

    /* Case:  node falls below minimum.
     * Either coalescence or redistribution
     * is needed.
     */

    /* Find the appropriate neighbor node with which
     * to coalesce.
     * Also find the key (k_prime) in the parent
     * between the pointer to node n and the pointer
     * to the neighbor.
     */

    neighbor_index = get_neighbor_index( n );
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = n->parent->keys[k_prime_index];
    neighbor = neighbor_index == -1 ? n->parent->pointers[1] : 
        n->parent->pointers[neighbor_index];

    capacity = n->is_leaf ? order : order - 1;

    /* Coalescence. */

    if (neighbor->num_keys + n->num_keys < capacity)
        return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);

    /* Redistribution. */

    else
        return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}
offset_t deleteEntry(offset_t root, offset_t n, Record * record) {
    keyNum numKeys = removeEntryFromNode(n, record);

    if (n == root) {
        return adjustRoot(root);
    }

    if (numKeys < 1) {
        return coalesceNodes(root, n);
    }
}

offset_t deleteRecord(offset_t root, keyNum key) {
    offset_t deletionKey;
    Record * deletionRecord;

    deletionRecord = findRecord(root, key);
    deletionKey = findLeaf(root, key);

    if (deletionRecord != NULL && deletionKey != 0) {
        root = deleteEntry(root, deletionKey, deletionRecord);
    }

    return root;
}



/* Master deletion function.
 */
node * _delete(node * root, int key) {

    node * key_leaf;
    record * key_record;

    key_record = find(root, key, false);
    key_leaf = find_leaf(root, key, false);
    if (key_record != NULL && key_leaf != NULL) {
        root = delete_entry(root, key_leaf, key, key_record);
        free(key_record);
    }
    return root;
}
offset_t delete(keyNum key) {
    file_read_page(0, &header);

    offset_t root = deleteEntry(getRootPageOffset(&header), key);
}


void destroy_tree_nodes(node * root) {
    int i;
    if (root->is_leaf)
        for (i = 0; i < root->num_keys; i++)
            free(root->pointers[i]);
    else
        for (i = 0; i < root->num_keys + 1; i++)
            destroy_tree_nodes(root->pointers[i]);
    free(root->pointers);
    free(root->keys);
    free(root);
}


node * destroy_tree(node * root) {
    destroy_tree_nodes(root);
    return NULL;
}

