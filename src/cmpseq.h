#ifndef CMPSEQ_H
#define CMPSEQ_H

#include "global.h"
#include "seqdiff_results.h"

typedef struct _sd_lookup {
	char *name;    /* header_name */
	char *comment; /* header_description */
	char *seq;
	int count;
	int in_a;
	int in_b;
	UT_hash_handle hh;
} sd_lookup_t;

/* hash-related methods */
void sd_add_seq(kseq_t *seq, int file, int use_header);
sd_lookup_t *sd_find_seq(char *str, int use_header);
void sd_delete_seq(sd_lookup_t *s);
void sd_delete_hash(void);
void sd_print_hash(void);
int sd_hash_key_count(void);

void cmpseq(seqdiff_results_t *results);

#endif
