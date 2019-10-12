#include "dbm.h"

// Database IO

int open_table(char* pathname) {
    openDB(pathname);
    
}
int db_insert(int64_t key, char* value) {
    // MASTER INSERTION
    Record * record = makeRecord(key, value);
    Insert(getRootPageOffset(&header), record);
    return 0;
}
int db_find(int64_t key) {
    findAndPrint(getRootPageOffset(&header), key);
    return 1;
}
int db_delete(int64_t key) {
    delete(key);
    return 1;
}
int db_print() {
    printTree(getRootPageOffset(&header));
    return 1;
}
int db_print_leaves() {
    printLeaves(getRootPageOffset(&header));
    return 1;
}


// INSERTION
