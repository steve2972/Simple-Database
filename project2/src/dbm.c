#include "dbm.h"

page_t root_page;

// Helper Functions



// Database IO

int open_table(char* pathname) {
    /* Opens/creates file with name pathname
     * @param[out]: 1 if new file, 0 if existing file
     * 
     * Note: The following command automatically sets the external int file
     * to point at the existing database; no other calls to the file is
     * necessary.
     */
    int _existingFile = openDB(pathname);
    if (_existingFile) {
        offset_t _root = getRootPageOffset(&header);
        // Case 1: There is no root => This is an empty dataabse
        if (_root == 0) {
            root_page = header;
        }
        // Case 2: There is a root => find the root page
        else if (_root > 0) {
            root_page = getRootPage(&header);
        }
        page_t test;
        READ(test, 4096);
        printf("Hello again: %ld\n", test.fp.NextFreePage);

        return 1;
    }
    else {
        // Initialize header and write to file
        page_t test;
        READ(test, 0);
        printf("Greetings! %ld %ld\n", test.hp.numPages, test.hp.freePage);

        return 1;
    }
    // Return -1 if the file could not be opened, or if the header page
    // could not be found
    return -1;
}
int db_insert(int64_t key, char* value) {
    // MASTER INSERTION
    record * pointer;
    page_t * leaf;
    return 0;
}
int db_find(int64_t key) {
    return 0;
}
int db_delete(int64_t key) {
    return 0;
}

// INSERTION
