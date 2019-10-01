#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>


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
   union {
        HeaderPage hp;
        FreePage fp;
        LeafPage lp;
        InternalPage ip;
   };

} page_t;

Record createEmptyRecord() {
    Record record = {0, ' '};
    return record;
}
entry createEntry(pagenum_t key, pagenum_t page) {
    entry temp = {key, page};
    return temp;
}

entry createEmptyEntry() {
    entry temp = {0,0};
    return temp;
}

PageHeader createPageHeader(pagenum_t fp, int isLeaf, int n) {
    PageHeader pageheader = {fp, isLeaf, n};
    return pageheader;
}

page_t createHeaderPage() {
    page_t page;
    page.hp.FreePageNum = 1; //The next page is the free page when initialized
    page.hp.NumOfPages = 1;
    page.hp.RootPageNum = 0; // Not initialized yet -> sets root page after first insert

}

page_t createInternalPage(pagenum_t parentNode) {
    page_t page;
    page.ip.pageheader = createPageHeader(parentNode, 0, 0);
    for (int i = 0; i < 248; i++) {
        page.ip.entries[i] = createEmptyEntry();
    }

    return page;
}

page_t createLeafPage(page_t internalPage) {
    page_t leaf;
    leaf.lp.pageheader = internalPage.ip.pageheader;
    leaf.lp.pageheader.isLeaf = 1;
    
    return leaf;
}


int main(void) {
    FILE *fpt;
    page_t header = createHeaderPage();

    fpt = fopen("test.bin", "wb");
    if (!fpt) {
        printf("Unable to open file\n");
        return 1;
    }

    fseek(fpt, 0x00, SEEK_SET); //goto beginning byte of file located at 0x00
    fwrite(&header, sizeof(page_t), 1, fpt);

    page_t test;
    fseek(fpt, 0x00, SEEK_SET);
    fread(&test, sizeof(page_t), 1, fpt);
    fclose(fpt);

    
}
