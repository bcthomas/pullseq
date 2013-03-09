#ifndef CMPSEQ_H
#define CMPSEQ_H

#include "global.h"
#include "uthash.h"

typedef struct _sd_lookup {
	char *name;
	int count;
	int in_a;
	int in_b;
	UT_hash_handle hh;
} sd_lookup_t;

/* hash-related methods */
void sd_add_name(char *name, int file);
sd_lookup_t *sd_find_name(char *name);
void sd_delete_name(sd_lookup_t *s);
void sd_delete_hash(void);
void sd_print_hash(void);
int sd_hash_key_count(void);

void cmpseq(seqdiff_results_t *results);
void cmpseq_with_output(seqdiff_results_t *results);

#endif
