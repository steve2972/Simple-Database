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
            db_print();
            break;
        case 'i':
            scanf("%ld %s", &key, input);
            break;
        case 'f':
            scanf("%ld", &key);
            db_find(key);
        case 'r':
            scanf("%d %d", &input, &range2);
            if (input > range2) {
                int tmp = range2;
                range2 = input;
                input = tmp;
            }
            find_and_print_range(root, input, range2, instruction == 'p');
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