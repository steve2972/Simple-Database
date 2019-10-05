#include "fm.h"

// Initializers
page_t createHeaderPage() {
    // If file is empty, create a header page and allocate a free page
    page_t header;
    /* Since header is always at 0x00, the first free page will be located
       exactly 4096 Bytes after the first header page */ 
    header.hp.freePage = 1; 
    // Number of pages = 1 header page + 2 free pages
    header.hp.numPages = 3;
    // We don't know the root yet, so set it to header page
    header.hp.rootPage = 0; 

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
    free.fp.NextFreePage = nextFreePage;

    // Write the free page into the file
    WRITE(free, PAGEOFFSET(offset));
    return free;
}


pagenum_t file_alloc_page() {
    // Allocate an on-disk page from the free page list

}

// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum) {

}
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest) {

}

// Writer function

// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src) {
    
}
