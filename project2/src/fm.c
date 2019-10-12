#include "fm.h"
int file;
page_t header;
// Initializers
int openDB(char * pathname) {
    // There are 2 cases when trying to open a file
    if (access(pathname, F_OK) != -1) {
        // Case 1: file exists
        file = open(pathname, O_RDWR);
        READ(header, 0);
        return 1;
    }
    else {
        // Case 2: file does not exist
        file = open(pathname, O_RDWR | O_CREAT, 0777);
        header = createHeaderPage();
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
    setFreePageOffset(&header, 0);
    // Number of pages = 1 (only the header)
    setNumPages(&header, 1);
    // We don't know the root yet, so set it to header page
    setRootPageOffset(&header, 0);

    // Create two new free pages
    file_alloc_page();

    WRITE(header, 0);
    return header;
}

page_t createFreePage(pagenum_t offset, pagenum_t nextFreePage) {
    // Allocates memory for a new free page at offset num * sizeof(page)
    page_t free;
    setNextFreePage(&free, offset);
    // Write the free page into the file
    WRITE(free, PAGEOFFSET(offset));
    return free;
}
offset_t createInternalPage() {
    page_t internal;

    // Allocate the required memory space from the free page list
    offset_t offset = file_alloc_page();

    file_read_page(offset, &internal);

    setFreePageOffset(&header, getNextFreePage(&internal));
    file_write_page(0, &header);

    memset(&internal, 0, PAGE_SIZE);

    PageHeader header;
    header.isLeaf = 0;
    header.NumKeys = 0;
    header.ParentPageNum = 0;
    header.sibling = 0;

    internal.node.header = header;

    for (int i = 0; i < INTERNAL_ORDER-1; i++) {
        setKey(&internal, 0, i);
        setEntryOffset(&internal, 0, i);
    }

    file_write_page(offset, &internal);

    return offset;
}
offset_t createLeafPage() {
    page_t leaf;

    // Same proccess as createInternalPage()
    offset_t offset = file_alloc_page();

    file_read_page(offset, &leaf);

    memset(&leaf, 0, PAGE_SIZE);

    PageHeader header;
    header.isLeaf = 1;
    header.NumKeys = 0;
    header.ParentPageNum = 0;
    header.sibling = 0;

    leaf.node.header = header;

    for (int i = 0; i < LEAF_ORDER-1; i++) {
        setKey(&leaf, 0, i);
        setRecordValue(&leaf, "", 0);
    }

    file_write_page(offset, &leaf);

    return offset;
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
    if(getFreePageOffset(&header) == 0) {
        pagenum_t nextFreePage = getNumPages(&header);
        setFreePageOffset(&header, nextFreePage);

        // Add two new free pages to the header page
        for (int i = 1; i <= 2; i++) {
            nextFreePage++;
            // set the next free page to either nextfreepage or 0 (beginning)
            setNextFreePage(&tempFree, i < 2 ? nextFreePage:0);
            file_write_page(nextFreePage-1, &tempFree);
        }

        // Synchronize the header page
        setNumPages(&header, getNumPages(&header) + 2);

        file_write_page(0, &header);

        return nextFreePage - 2;
    }

    /* Use alloc page whenever creating a new leaf page or internal page
     * Thus, we clear the next free page, and set the next free page offset
     * of the header page to the next free page offset of the original 
     * free page.
     */
    offset_t nextFreePageOffset = getFreePageOffset(&header);

    // suppose the header is A:
    file_read_page(nextFreePageOffset, &tempFree);
    // A -> F1 -> F2 turns into A -> F2
    setNextFreePage(&header, getNextFreePage(&tempFree));

    // Synchronize
    file_write_page(0, &header);

    // Return the address of F1
    return nextFreePageOffset;

}
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest) {
    pread(file, dest, PAGE_SIZE, PAGEOFFSET(pagenum));
}

// Writer function

// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src) {
    pwrite(file, src, PAGE_SIZE, PAGEOFFSET(pagenum));
    fflush(stdout); // synchronize the file to on-disk memory
}
//TODO: create file_write_PageHeader
PageHeader file_write_PageHeader(page_t * page, pagenum_t parent,\
                            offset_t sibling, int isLeaf, int num) {

    PageHeader pageheader;
    pageheader.isLeaf = isLeaf;
    pageheader.ParentPageNum = parent;
    pageheader.sibling = sibling;
    pageheader.NumKeys = num;
    
    return pageheader;
}
void file_write_entry(page_t * page, pagenum_t key, pagenum_t value) {
    Entry entry;
    entry.key = key;
    entry.page = value;
    
    // Find the position where 
    int index = findEmptyEntryIndex(page);
    // Set the entry at the index equal to the above entry
    page->node.entries[index] = entry;
}
void file_write_record(page_t * page, pagenum_t key, char * value) {
    Record record;
    record.key = key;
    strcpy(record.value, value);
    
    int index = findEmptyRecordIndex(page);
    page->node.records[index] = record;
}

// Header Page
    // Getters

page_t getHeaderPage() {
    return header;
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
    return ((const NodePage *)page)->header.sibling;
}

        // Node Page Getters
int copyRecord(page_t * page, int index, char * dest) {
    // Copies contents of the record into the destination
    // On-disk => in memory
    char * src = page->node.records[index].value;
    strcpy(dest, src);
    // Return 1 if successfully copied
    return strcmp(dest, src) == 0 ? 1 : 0;
}
keyNum getKey(page_t * page, int index) {
    // Helper function that returns the key associated with the index
    // Returns -1 if failed to find key, or if list out-of-bounds

    // Case 1: Internal Page
    if(!isLeaf(page)) {
        if (index > INTERNAL_ORDER - 2) {
            return -1;
        }
        else {
            return ((const NodePage *)page)->entries[index].key;
        }
    }
    else {
    // Case 2: Leaf Page
        if (index > LEAF_ORDER -1) {
            return -1;
        }
        else {
            return ((const NodePage *)page)->records[index].key;
        }

    }
}
int getIndex(page_t * page, keyNum key) {
    // Helper function that returns the index associated with the key

    // Case 1: Internal Page
    if (!isLeaf(page)) {
        if (key > INTERNAL_ORDER - 1 || key < 0)
            return -1;
        
        int max = 0, maxIndex = -1;
        for(int i = 0; i < INTERNAL_ORDER-1; i++) {
            if (((const NodePage *)page)->entries[i].key <= key && ((const NodePage *)page)->entries[i].key > max) {
                max = ((const NodePage *)page)->entries[i].key;
                maxIndex = i;
            }
        }
        if (maxIndex >= 0)
            return maxIndex;
        return -1;      // Failed to find associated key in Internal Page
    }

    // Case 2: Leaf Page
    if (isLeaf(page)) {
        for (int i = 0; i < LEAF_ORDER; i++) {
            if (((const NodePage *)page)->records[i].key == key) {
                return i;
            }
        }
        return -1;      // Failed to find associated key in Leaf Page
    }
}
offset_t getEntryOffset(page_t * page, int index) {
    if (!isLeaf(page)) {
        // Internal Page => [0, 247]
        if (index >= INTERNAL_ORDER -1) {
            return -1;
        }
        else if (index == 0) {
            // returns One more Page Number -> used where leftmost key deleted
            return getOneMorePage(page);
        }
        else {
            return ((const NodePage *)page)->entries[index].page;
        }
    }
    else {
        // Leaf Page: helper function to get the right sibling
        if (index == LEAF_ORDER) {
            return ((const NodePage *)page)->header.sibling;
        }
        return -1;
    }
}
    // Setters
int setParentPageNum(page_t * page, offset_t offset) {
    page->node.header.ParentPageNum = offset;

    return page->node.header.ParentPageNum == offset ? 1:0;
}
int LeafToggle(page_t * page) {
    if (page->node.header.isLeaf == 0) {
        // This is not a leaf -> set to leaf
        page->node.header.isLeaf = 1;
        return 1;
    }
    else {
        page->node.header.isLeaf = 0;
        return 1;
    }

    return -1;
}
int setNumKeys(page_t * page, int keys) {
    page->node.header.NumKeys = keys;

    return page->node.header.NumKeys == keys ? 1:0;
}
int setEntryOffset(page_t * page, offset_t offset, int index) {
    page->node.entries[index].page = offset;

    return page->node.entries[index].page == offset ? 1 : 0;
}
int setSiblingOffset(page_t * page, offset_t offset, int index) {
    if (!isLeaf(page)) {
        if (index > INTERNAL_ORDER - 1) {
            return -1;
        }
        else if (index == 0) {
            page->node.header.sibling = offset;
        }
        else {
            page->node.entries[index].page = offset;
        }

        return 1;
    }
    else {
        if (index == LEAF_ORDER-1) {
            page->node.header.sibling = offset;
            return 1;
        }
        return -1;
    }
}
int setRecordValue(page_t * page, char * value, int index) {
    strcpy(page->node.records[index].value, value);

    return strcmp(page->node.records[index].value, value) == 0 ? 1 : 0;
}
int setKey(page_t * page, keyNum key, int index) {
    if (isLeaf(page)) {
        page->node.records[index].key = key;
        return 1;
    }
    else if (!isLeaf(page)) {
        page->node.entries[index].key = key;
        return 1;
    }
    return -1;
}


// Utility Functions

int findEmptyEntryIndex(page_t * page) {
    for (int i = 0; i < INTERNAL_ORDER -1; i++) {
        //248 entries means that the last entry index = 247 = 249-2
        if (((const NodePage *)page)->entries[i].key == 0) {
            return i;
        }
    }
    // Returns -1 if full
    return -1;
}
int findEmptyRecordIndex(page_t * page) {
    for (int i = 0; i < LEAF_ORDER; i++) {
        if (((const NodePage *)page)->records[i].key == 0) {
            return i;
        }
    }
    // Returns -1 if full
    return -1;
}
//TODO: implement a faster search method
int findEntryByKey(page_t * page, keyNum key) {
    for (int i = 0 ; i < INTERNAL_ORDER - 2; i++) {
        if (((const NodePage *)page)->entries[i].key == key) {
            return i;
        }
    }
    return -1;
}
int findRecordByKey(page_t * page, keyNum key) {
    for (int i = 0; i < LEAF_ORDER; i++) {
        if (((const NodePage *)page)->records[i].key == key) {
            return i;
        }
    }
    return -1;
}
int search(page_t * page, keyNum key) {
    /*
     * DISCLAIMER: the following algorithm is adapted from the code uploaded to
     * gitHub under the alias hgs3896
     */
    int left = 0, right = getNumKeys(page), middle, middle_key;
    while (left < right) {
        middle = (left + right) / 2;

        if ( left == middle )
            return middle;

        middle_key = getKey(page, middle);
        if (middle_key <= key)
            left = middle;
        else
            right = middle;
    }
    return -1;
}
Record * makeRecord(keyNum keynum, char * value) {
    Record * record = (Record *)malloc(sizeof(Record));
    record->key = keynum;
    strcpy(record->value, value);
    return record;
}