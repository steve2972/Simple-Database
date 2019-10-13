#include "dbm.h"

int main( int argc, char ** argv ) {
    open_table("test.db");
    char instruction;
    keyNum key;
    char input[120];
    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd':
            scanf("%ld", &key);
            db_delete(key);
            db_print_leaves();
            break;
        case 'i':
            scanf("%ld %s", &key, input);
            db_insert(key, input);
            break;
        case 'f':
            char * ret_val = malloc(sizeof(char) * 120);
            scanf("%ld", &key);
            db_find(key, ret_val);
            break;
        case 'l':
            db_print_leaves();
            break;
        case 'q':
            while (getchar() != (int)'\n');
            return 1;
            break;
        case 't':
            db_print();
            break;
        case '?':
            usage();
            break;
        default:
            usage();
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");
}