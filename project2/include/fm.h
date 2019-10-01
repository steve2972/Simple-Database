#ifndef __FM_H__
#define __FM_H__

#define Windows
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

extern FILE * file;

typedef uint64_t pagenum_t; //pagenum_t size of 8 bytes

typedef struct HeaderPage {
    pagenum_t FreePageNum, RootPageNum, NumOfPages;
} HeaderPage;

typedef struct FreePage {
    pagenum_t NextFreePage;
} FreePage;

typedef struct PageHeader {
    pagenum_t FreePageNum;
    int isLeaf; // 1 if leaf, 0 if not
    int NumKeys; 
    pagenum_t Reserved[14]; // Reserved[14] = right sibling page number OR one more page number
} PageHeader;

typedef struct Record {
    pagenum_t key; //data[0] = key, rest is value
    char value[120];
    //31 records go in leaf page
} Record;

typedef struct entry {
    pagenum_t key;
    pagenum_t page;
    //248 entries go into internal page
} entry;

typedef struct LeafPage {
    PageHeader pageheader;
    Record records[31];
} LeafPage;

typedef struct InternalPage {
    PageHeader pageheader;
    entry entries[248];
} InternalPage;

typedef struct page_t {
    /*
    *in-memory page structure
    *4 types of pages (Header, Free, Leaf, Internal)
    *Page specifications: PageSize = 4096Bytes, RecordSize = 128 (8 + 120)Bytes
    */
    union  {
        HeaderPage hp;
        FreePage fp;
        LeafPage lp;
        InternalPage ip;
    } Page;
} page_t;



// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum);

// Allocator functions:
    // Allocate an on-disk page from the free page list
pagenum_t file_alloc_page();

// Reader functions:
    // Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest);

// Writer function
    // Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src);

#endif /*__FM_H__*/