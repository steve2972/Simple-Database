#include "dbm.h"

page_t head;
page_t root_page;

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
        head = getHeaderPage();
        offset_t _root = getFreePageOffset(&head);
        // Case 1: There is no root => This is an empty dataabse
        if (_root == 0) {
            root_page = head;
        }
        // Case 2: There is a root => find the root page
        else if (_root > 0) {
            root_page = getRootPage(&head);
        }
        return 1;
    }
    else {
        // Initialize header and write to file
        head = createHeaderPage();
        return 1;
    }

    // Return -1 if the file could not be opened, or if the header page
    // could not be found
    return -1;
}
int db_insert(int64_t key, char* value) {
    return 0;
}
int db_find(int64_t key) {
    return 0;
}
int db_delete(int64_t key) {
    return 0;
}
