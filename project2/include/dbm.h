#ifndef __DBM_H__
#define __DBM_H__

#define Windows
#include "fm.h"
#include "bpt.h"


int open_table(char* pathname);
int db_insert(int64_t key, char* value);
int db_find(int64_t key);
int db_delete(int64_t key);


#endif /*__DBM_H__*/