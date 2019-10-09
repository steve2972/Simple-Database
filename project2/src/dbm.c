#include "dbm.h"

pagenum_t root;

// Database IO

int open_table(char* pathname) {
    /* Opens/creates file with name pathname
     * @param[out]: 1 if new file, 0 if existing file
     * 
     * Note: The following command automatically sets the external int file
     * to point at the existing database; no other calls to the file is
     * necessary.
     */
    openDB(pathname);
    
}
int db_insert(int64_t key, char* value) {
    // MASTER INSERTION
    Record * record = makeRecord(key, value);
    Insert(getRootPageOffset(&header), record);
    return 0;
}
int db_find(int64_t key) {
    pagenum_t pagenum = findLeaf(getRootPageOffset(&header), key);
    return pagenum;
}
int db_delete(int64_t key) {
    return 0;
}

// INSERTION
