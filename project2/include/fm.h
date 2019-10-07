#ifndef __FM_H__
#define __FM_H__

/*
 * fm.h is short for File Management Header File.
 * This layer of my database system manages the 
 * disk space and is congruent to the Disk Space Management
 * layer in a usual DBMS architecture
 */

#define Windows

// standard header files
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// system call header files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define INTERNAL_ORDER 249
#define LEAF_ORDER 31
#define PAGE_SIZE 4096

extern int file;
extern page_t header;   // file and header are necessary when reading,
                        // so, cache them!

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
    keyNum key; //data[0] = key, rest is value
    char value[120];
    //31 records go in leaf page
} Record;

typedef struct Entry {
    keyNum key;
    offset_t page;
    //248 entries go into internal page
} Entry;

typedef struct nodePage {
    PageHeader header;
    union {
        Record records[LEAF_ORDER];     //31 records can go in leaf
        Entry entries[INTERNAL_ORDER-1];//248 entries can go in internal
    };
} NodePage;

typedef struct page_t {
    /*
    *in-memory page structure
    *4 types of pages (Header, Free, Leaf, Internal)
    *Page specifications: PageSize = 4096Bytes, RecordSize = 128 (8 + 120)Bytes
    */
    union  {
        HeaderPage hp;
        FreePage fp;
        NodePage node;
    };
} page_t;


/*
* The following are the API's used to access, read and modify files
*/

#define READ(buf, offset) (pread(file, &(buf), PAGE_SIZE, offset))
#define WRITE(buf, offset) (pwrite(file, &(buf), PAGE_SIZE, offset))
#define PAGEOFFSET(offset) (offset * sizeof(page_t))

// Initializers
int openDB(char * pathname);

page_t createHeaderPage();
page_t createFreePage(pagenum_t offset, pagenum_t nextFreePage);
page_t createInternalPage();
page_t createLeafPage();

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
PageHeader file_write_PageHeader(page_t * page, pagenum_t parent, offset_t sibling, int isLeaf, int num);
    // Write new page entry (for internal pages)
void file_write_entry(page_t * page, pagenum_t key, pagenum_t value);
    // Write new page record (for leaf pages)
void file_write_record(page_t * page, pagenum_t key, char * value);

// Header Page
    // Getters
page_t getHeaderPage();
page_t getRootPage(page_t * header);

offset_t getFreePageOffset(page_t * page);
offset_t getRootPageOffset(page_t * page);
pagenum_t getNumPages(page_t * page);
    // Setters
int setFreePageOffset(page_t * page, offset_t offset);
int setRootPageOffset(page_t * page, offset_t offset);
int setNumPages(page_t * page, pagenum_t pages);
void increment(page_t * page);  //adds 1 to numPages

// Free Page
    // Getters
offset_t getNextFreePage(page_t * page);
    // Setters
int setNextFreePage(page_t * page, offset_t offset);

// Node Page
    // Getters -> Page Header code getters shared with Leaf Page

        // Page Header Getters
offset_t getParentPageNum(page_t * page);
int getNumKeys(page_t * page);
int isLeaf(page_t * page);
offset_t getOneMorePage(page_t * page);
        // Leaf Page Getters
int copyRecord(page_t * page, keyNum key, char * dest);
keyNum getKey(page_t * page, int index);
int getIndex(page_t * page, keyNum key);
        // Internal Page Getters

    // Setters
int setParentPageNum(page_t * page, offset_t offset);
int LeafToggle(page_t * page); // if 1 -> 0. if 0 -> 1
int setNumkeys(page_t * page, int keys);
int setEntryOffset(page_t * page, offset_t offset, int index);
int setRecordValue(page_t * page, char * value, int index);
int setKey(page_t * page, keyNum key, int index);

// Utility Functions
int PageType(page_t page);    // returns the type of page
int findEmptyEntryIndex(page_t * page);
int findEmptyRecordIndex(page_t * page);
int findEntryByKey(page_t * page, keyNum key);
int findRecordByKey(page_t * page, keyNum key);

#endif /*__FM_H__*/