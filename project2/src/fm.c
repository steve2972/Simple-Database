#include "fm.h"

pagenum_t file_alloc_page() {
    // Allocate an on-disk page from the free page list

}

// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum) {
    fseek(file, sizeof(page_t) * pagenum, SEEK_SET);
    FreePage page;
    page.NextFreePage = 0;  //TODO: change this integer into page number

    fwrite(&page, sizeof(page_t), 1, file);
}
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest) {
    fseek(file, sizeof(page_t) * pagenum, SEEK_SET);
    fread(dest, sizeof(page_t), 1, file);
}

// Writer function

// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src) {
    //File comes from external constant
    fseek(file, sizeof(page_t) * pagenum, SEEK_SET);
    fwrite(src, sizeof(page_t), 1, file);
}
