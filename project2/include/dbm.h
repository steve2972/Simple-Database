#ifndef __DBM_H__
#define __DBM_H__

/*
 * dbm.h is short for Database Management Header File.
 * This part of my code is similar to the Files and Index
 * Management layer in the usual DBMS architecture.
 * 
 * Furthermore, dbm.h is reliant upon bpt.h for its various
 * methods on managing a b+ tree. 
 * 
 * Thus, dbm.h takes two dependencies:
 * fm.h and bpt.h
 */

#define Windows
#include "fm.h"
#include "bpt.h"

extern page_t head;
extern page_t root_page;

// Database IO

int open_table(char* pathname);
int db_insert(int64_t key, char* value);
int db_find(int64_t key);
int db_delete(int64_t key);


#endif /*__DBM_H__*/