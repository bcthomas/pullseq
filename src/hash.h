#ifndef HASH_H
#define HASH_H

#include "uthash.h"

typedef struct lookup {
	char *name;
	UT_hash_handle hh;
} lookup_t;


void add_name(char *name);
lookup_t *find_name(char *name);
void delete_name(lookup_t *s);
void delete_hash(void);
void print_hash(void);

#endif
