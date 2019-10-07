#include "fm.h"
int file;
page_t header;
// Initializers
int openDB(char * pathname) {
    // There are 2 cases when trying to open a file
    if (access(pathname, F_OK) != -1) {
        // Case 1: file exists
        file = open(pathname, O_RDWR);
        return 1;
    }
    else {
        // Case 2: file does not exist
        file = open(pathname, O_RDWR | O_CREAT, 0777);
        return 0;
    }    
}

page_t createHeaderPage() {
    /* If file is empty, create a header page and allocate a free page
     * Since header is always at 0x00, the first free page will be located
     * exactly 4096 Bytes after the first header page
     * 
     * However, we do not have a free page yet, so this will be
     * implemented later on. */ 
    setFreePageOffset(&header, 1);
    // Number of pages = 1 (only the header)
    setNumPages(&header, 1);
    // We don't know the root yet, so set it to header page
    setRootPageOffset(&header, 0);

    //Allocate two new free pages (just in case)
    createFreePage(1, 2);   //Creates a free page just after header
    createFreePage(2, 0);   //Creates a free page after the one before

    WRITE(header, 0);
    return header;
}

// @param[in] takes the page offset as input, and nextFreePage = 0
// if it is the last free page
page_t createFreePage(pagenum_t offset, pagenum_t nextFreePage) {
    // Allocates memory for a new free page at offset num * sizeof(page)
    page_t free;
    setNextFreePage(&free, offset);
    // Write the free page into the file
    WRITE(free, PAGEOFFSET(offset));
    return free;
}


// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum) {
    page_t tempFree;

    // Read and clean the page
    file_read_page(pagenum, &tempFree);
    memset(&tempFree, 0, PAGE_SIZE);

    // Sets the free page offset of the header to this free page
    // The next free page after this will be the originally next free page
    setNextFreePage(&tempFree, getNextFreePage(&header));
    setFreePageOffset(&header, pagenum);

    // Write to memory and sync
    file_write_page(pagenum, &tempFree);
    file_write_page(0, &header);
}
// Allocate an on-disk page from the free page list
pagenum_t file_alloc_page() {
    // When there are no more free pages on-disk, allocate 2 new free pages
    // First, update the cached header

    file_read_page(0, &header);

    page_t tempFree;
    if(getFreePageOffset(&header) == 1) {
        setFreePageOffset(&header, getNumPages(&header));

        // Add two new free pages to the header page
        // TODO: add implementation for dynamic free page allocation
        setNextFreePage(&tempFree, 2);
        file_write_page(1, &tempFree);
        setNextFreePage(&tempFree, 0);
        file_write_page(2, &tempFree);

        // Synchronize the header page
        setNumPages(&header, getNumPages(&header) + 2);
        file_write_page(0, &header);
    }

    //TODO: fix logic of <else> free file allocation
    offset_t nextFreePageOffset = getFreePageOffset(&header);

    file_read_page(nextFreePageOffset, &tempFree);
    setNextFreePage(&header, getNextFreePage(&tempFree));

    // Synchronize
    file_write_page(0, &header);

}
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest) {
    if (pread(file, dest, PAGE_SIZE, PAGEOFFSET(pagenum)) < PAGE_SIZE) {
        printf("Failed to read the file\n");
        exit(EXIT_FAILURE);
    }
}

// Writer function

// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src) {
    if (pwrite(file, src, PAGE_SIZE, PAGEOFFSET(pagenum))< PAGE_SIZE) {
        printf("Failed to write the file to the disk\n");
        exit(EXIT_FAILURE);
    }
    fflush(stdout); // synchronize the file to on-disk memory
}
//TODO: create file_write_PageHeader
void file_write_entry(page_t * page, pagenum_t key, pagenum_t value) {
    Entry entry;
    entry.key = key;
    entry.page = value;
}
void file_write_record(page_t * page, pagenum_t key, char * value) {

}

// Header Page
    // Getters

page_t getHeaderPage() {
    page_t head;
    READ(head, 0);
    return head;
}

page_t getRootPage(page_t * header) {
    page_t root;
    READ(root, PAGEOFFSET(header->hp.rootPage));
    return root;
}

offset_t getFreePageOffset(page_t * header) {
    return header->hp.freePage;
}
offset_t getRootPageOffset(page_t * page) {
    return page->hp.rootPage;
}
offset_t getNumPages(page_t * page) {
    return page->hp.numPages;
}

    // Setters => returns 1 if successful, 0 if not
int setFreePageOffset(page_t * page, offset_t offset) {
    page->hp.freePage = offset;
    return page->hp.freePage == offset ? 1 : 0;
}
int setRootPageOffset(page_t * page, offset_t offset) {
    page->hp.rootPage = offset;
    return page->hp.rootPage == offset ? 1 : 0;
}
int setNumPages(page_t * page, pagenum_t pages) {
    page->hp.numPages = pages;
    return page->hp.numPages == pages ? 1 : 0;
}
void increment(page_t * page) {
    page->hp.numPages++;
}

// Free Page
    // Getters
offset_t getNextFreePage(page_t * page) {
    return page->fp.NextFreePage;
}
    // Setters
int setNextFreePage(page_t * page, offset_t offset) {
    page->fp.NextFreePage = offset;
    return page->fp.NextFreePage == offset ? 1 : 0;
}

// Node Page
    // Getters
        // Page Header Getters
offset_t getParentPageNum(page_t * page) {
    return ((const NodePage *) page)->header.ParentPageNum;
}
int getNumKeys(page_t * page) {
    return ((const NodePage *)page)->header.NumKeys;
}
int isLeaf(page_t * page) {
    return ((const NodePage *)page)->header.isLeaf;
}
offset_t getOneMorePage(page_t * page) {
    // Returns the right sibling for leaf. Returns one more page for internal.
    return ((const NodePage *)page)->header.sibling;   // Can be leaf or internal page
}

        // Leaf Page Getters
int copyRecord(page_t * page, keyNum key, char * dest) {
    // Copies contents of the record into the destination
    // On-disk => in memory
    char * src = ((const NodePage *)page)->records[key].value;
    strcpy(dest, src);
    // Return 1 if successfully copied
    return strcmp(dest, src) == 0 ? 1 : 0;
}
keyNum getKey(page_t * page, int index) {
    // Important function that returns the key associated with the index
    // Returns -1 if failed to find key, or if list out-of-bounds

    // Case 1: Internal Page
    if(!isLeaf(page)) {
        if (index >= INTERNAL_ORDER - 2) {
            return -1;
        }
        else {
            return ((const NodePage *)page)->entries[index].key;
        }
    }
    else {
    // Case 2: Leaf Page
        if (index >= LEAF_ORDER -1) {
            return -1;
        }
        else {
            return ((const NodePage *)page)->records[index].key;
        }

    }
}
int getIndex(page_t * page, keyNum key) {
    // Function that returns the index associated with the key

    // Case 1: Internal Page
    if (!isLeaf(page)) {
        for(int i = 0; i < INTERNAL_ORDER-2; i++) {
            if (((const NodePage *)page)->entries[i].key == key) {
                return i;
            }
        }
        return -1;      // Failed to find associated key in Internal Page
    }

    // Case 2: Leaf Page
    if (isLeaf(page)) {
        for (int i = 0; i < LEAF_ORDER-1; i++) {
            if (((const NodePage *)page)->records[i].key == key) {
                return i;
            }
        }
        return -1;      // Failed to find associated key in Leaf Page
    }
}
    // Setters


// Utility Functions

// @param[out]: 0 if header, 1 if free, 2 if node, 3 if unknown
int PageType(page_t page) {
    // Check which parameters are initialized
    if (page.hp.numPages != NULL) {
        return 0;
    }
    else if (page.fp.NextFreePage != NULL) {
        return 1;
    }
    else if (page.node.header.isLeaf != NULL) {
        return 2;
    }
    return 3;
}
