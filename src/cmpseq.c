#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "global.h"
#include "cmpseq.h"
#include "uthash.h"
#include "kseq.h"

/*
 * read sequences in first_file into hash
 *   update results with first_file counts
 * read sequences from second_file
 * lookup sequence in hash
 * if found
 *  update common
 *  update hash value to acknowledge overlap
 * else
 *  update second_file_uniq
 * walk through hash and check which entries are not common
 *  update first_file_uniq
 *
 */

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

sd_lookup_t *sd_lookup = NULL;

/* cmpseq without file-based output */
void cmpseq(seqdiff_results_t *results) {
	gzFile fp;
	int l;
	kseq_t *seq;
	sd_lookup_t *s,*temp;


	/* open sequence file */
	fp = gzopen(results->first_file,"r");
	if (!fp) {
		fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,results->first_file);
		exit(EXIT_FAILURE);
	}

	seq = kseq_init(fp); /* initialize kseq */
	
	while((l = kseq_read(seq)) >= 0) {
		results->first_file_total++; /* increment first_file_total */
		sd_add_name(seq->seq.s,1);
	}

	kseq_destroy(seq);
	gzclose(fp); /* done reading file */

	/* process second_file */
	fp = gzopen(results->second_file,"r");
	if (!fp) {
		fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,results->second_file);
		exit(EXIT_FAILURE);
	}

	seq = kseq_init(fp); /* initialize kseq */
	
	while((l = kseq_read(seq)) >= 0) {
		results->second_file_total++; /* increment second_file_total */
		sd_add_name(seq->seq.s,2);
#ifdef XYZ
		temp = sd_find_name(seq->seq.s);
		if (temp == NULL) {
			results->second_file_uniq++; /* increment second_file_uniq */
		} else {
			results->common++; /* increment common */
		}
#endif
	}

	kseq_destroy(seq); /* free kseq struct */
	gzclose(fp); /* done reading file */

	/*
	for(temp=sd_lookup;temp!=NULL;temp=temp->hh.next) {
		if (temp->in_a == 1 && temp->in_b == 0)
			results->first_file_uniq++;
		else if (temp->in_a == 0 && temp->in_b == 1)
			results->second_file_uniq++;
		else if (temp->in_a == 1 && temp->in_b == 1)
			results->common++;
	}
	*/

	HASH_ITER(hh, sd_lookup, s, temp) {
		if (s->in_a == 1 && s->in_b == 0)
			results->first_file_uniq++;
		else if (s->in_a == 0 && s->in_b == 1)
			results->second_file_uniq++;
		else if (s->in_a == 1 && s->in_b == 1)
			results->common++;
	}

	sd_delete_hash(); /* free the hash nodes */
}

/* cmpseq with fasta- or fastq-based output */
void cmpseq_with_output(seqdiff_results_t *results) {
}

/* hash functions */
/**
 * sd_add_name
 * checks if the key is in the hash. If yes, just increment count; if
 * no, add the new entry to the hash.
 */
void sd_add_name(char *name, int file)
{
	sd_lookup_t *s;

	HASH_FIND_STR(sd_lookup,name,s);

	if (s==NULL) { /* key is not in hash */
		s = (sd_lookup_t *)malloc(sizeof(sd_lookup_t));
		if (s == NULL) {
			fprintf(stderr,"couldn't get memory for sd_lookup_t\n");
			exit(EXIT_FAILURE);
		} else {
			s->name = NULL;
			s->count = 1;
			s->in_a = 0;
			s->in_b = 0;
		}
		s->name = (char *)malloc((sizeof(char*) * (strlen(name)+1)));
		if (s->name == NULL) {
			fprintf(stderr,"couldn't get memory for name string\n");
			exit(EXIT_FAILURE);
		}
		strncpy(s->name,name,strlen(name)+1);
		if (file == 1)
			s->in_a = 1;
		else if (file == 2)
			s->in_b = 1;
		HASH_ADD_KEYPTR( hh, sd_lookup, s->name, strlen(s->name), s );
	} else {
		s->count++; /* key is already in the hash, just incr count */
		if (file == 1)
			s->in_a = 1;
		else if (file == 2)
			s->in_b = 1;
	}
}

sd_lookup_t *sd_find_name(char *name)
{
	sd_lookup_t *s;
	HASH_FIND_STR(sd_lookup, name, s);
	if (s)
		return s;
	else
		return (sd_lookup_t *)NULL;
}

void sd_delete_name(sd_lookup_t *s)
{
	HASH_DEL(sd_lookup, s);
	free(s->name);
	free(s);
}

void sd_delete_hash()
{
	sd_lookup_t *current_name, *tmp;
	HASH_ITER(hh,sd_lookup,current_name,tmp) {
		HASH_DEL(sd_lookup, current_name);
		free(current_name->name);
		free(current_name);
	}
}

int sd_hash_key_count(void)
{
	sd_lookup_t *s;
	int count = 0;
	for(s=sd_lookup;s!=NULL;s=s->hh.next)
		count++;
	return(count);
}

void sd_print_hash(void)
{
	sd_lookup_t *s;
	for(s=sd_lookup;s!=NULL;s=s->hh.next)
		fprintf(stderr,"name %s\n",s->name);
}

