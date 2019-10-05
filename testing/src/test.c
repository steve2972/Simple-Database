#include "/mnt/c/Users/steve/OneDrive/Desktop/Programming/DBMS_Systems/2019_ITE2038_2016054148/testing/src/fm.c"
int main(int argc, char ** argv) {
    file = open(argv[1], O_RDWR);
    if (file < 0) {
        // File does not exist => create a new file with same name
        file = creat(argv[1], O_RDWR);
        // Initialize the file with a new header page
        createHeaderPage();
    }

    page_t page;
    READ(page, 0);

    printf("%ld %ld %ld\n", page.hp.numPages, page.hp.freePage, page.hp.rootPage);
    
}