#ifndef __FM_H__
#define __FM_H__

#define Windows

// standard header files
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// system call header files
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define INTERNAL_ORDER 249
#define LEAF_ORDER 31
#define PAGE_SIZE 4096

extern int file;

/*
* Type definitions of the most commonly used Bytes
*/
typedef uint64_t pagenum_t;
typedef uint64_t offset_t;
typedef uint64_t keyNum;
typedef int8_t Byte;


/*
* The structures that make up the core of the 
* types of Pages stored in files
*/
typedef struct HeaderPage {
    offset_t freePage;
    offset_t rootPage;
    pagenum_t numPages;
    Byte reserved[4072];    // unused space for header page
} HeaderPage;

typedef struct FreePage {
    offset_t NextFreePage;
    Byte reserved[4088];    // only 8 Bytes used for the next free page pointer
} FreePage;

typedef struct PageHeader {
    offset_t ParentPageNum;
    int isLeaf; // 1 if leaf, 0 if not
    int NumKeys; 
    Byte reserved[104];
    offset_t sibling;  // right sibling for leaf, one more page for internal

} PageHeader;

typedef struct Record {
    pagenum_t key; //data[0] = key, rest is value
    char value[120];
    //31 records go in leaf page
} Record;

typedef struct entry {
    keyNum key;
    offset_t page;
    //248 entries go into internal page
} entry;

typedef struct LeafPage {
    PageHeader pageheader;
    Record records[LEAF_ORDER];
} LeafPage;

typedef struct InternalPage {
    PageHeader pageheader;
    entry entries[INTERNAL_ORDER - 1];  // maximum of 248 entries
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
    };
} page_t;


/*
* The following are the API's used to access, read and modify files
*/

#define READ(buf, offset) (pread(file, &(buf), PAGE_SIZE, offset))
#define WRITE(buf, offset) (pwrite(file, &(buf), PAGE_SIZE, offset))
#define PAGEOFFSET(offset) (offset * sizeof(page_t))

// Initializers
page_t createHeaderPage();
page_t createFreePage(pagenum_t offset, pagenum_t nextFreePage);

// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum);
// Allocate an on-disk page from the free page list
pagenum_t file_alloc_page();
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest);

// Writer function
// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src);
    // Write new page header (for leaf and internal pages)
void file_write_PageHeader(page_t * page, pagenum_t freepage, pagenum_t reserved, int isLeaf, int num);
    // Write new page entry (for internal pages)
void file_write_entry(page_t * page, pagenum_t key, pagenum_t value);
    // Write new page record (for leaf pages)
void file_write_record(page_t * page, pagenum_t key, char * value);

// Header Page
    // Getters
offset_t getFreePageOffset(page_t * page);
offset_t getRootPageOffset(page_t * page);
pagenum_t getNumPages(page_t * page);
    // Setters
int setFreePageOffset(page_t * page, offset_t offset);
int setRootPageOffset(page_t * page, offset_t offset);
int setNumPages(page_t * page, pagenum_t pages);

// Free Page
    // Getters
offset_t getNextFreePage(page_t * page);
    // Setters
int setNextFreePage(page_t * page, offset_t offset);

// Internal Page
    // Getters -> Page Header code getters shared with Leaf Page
offset_t getParentPageNum(page_t * page);
int getNumKeys(page_t * page);
int isLeaf(page_t * page);
int getOneMorePage(page_t * page);
keyNum getKey(page_t * page, int index);
    // Setters
int setParentPageNum(page_t * page, offset_t offset);
int setLeaf(page_t * page); // if 1 -> 0. if 0 -> 1

#endif /*__FM_H__*/