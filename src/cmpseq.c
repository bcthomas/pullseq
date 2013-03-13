#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "global.h"
#include "cmpseq.h"
#include "output.h"
#include "seqdiff_results.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

sd_lookup_t *sd_lookup = NULL;

/**
 * cmpseq
 *
 * Compare two files of sequences and determine which sequences are
 * uniq or common to each file.
 *
 **/
void cmpseq(seqdiff_results_t *results) {
	gzFile fp;
	int l;
	kseq_t *seq;
	sd_lookup_t *s,*temp;

	/* open first sequence file */
	fp = gzopen(results->first_file,"r");
	if (!fp) {
		fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,results->first_file);
		exit(EXIT_FAILURE);
	}

	seq = kseq_init(fp); /* initialize kseq */
	
	while((l = kseq_read(seq)) >= 0) {
		results->first_file_total++; /* increment first_file_total */
		sd_add_seq(seq,1,results->use_header);
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
		sd_add_seq(seq,2,results->use_header);
	}

	kseq_destroy(seq); /* free kseq struct */
	gzclose(fp); /* done reading file */

	HASH_ITER(hh, sd_lookup, s, temp) {
		if (s->in_a == 1 && s->in_b == 0) {
			results->first_file_uniq++;
			if (!results->only_summarize)
				fprintf(stdout,"%s\t\n",s->name);
			/* printing out fasta or fastq????? */
			if (results->a_output_fp != NULL)
				print_fasta(results->a_output_fp, s->name, s->comment, s->seq);
		} else if (s->in_a == 0 && s->in_b == 1) {
			results->second_file_uniq++;
			if (!results->only_summarize)
				fprintf(stdout,"\t%s\n",s->name);
			if (results->b_output_fp != NULL)
				print_fasta(results->b_output_fp, s->name, s->comment, s->seq);
		} else if (s->in_a == 1 && s->in_b == 1) {
			results->common++;
			if (!results->only_summarize)
				fprintf(stdout,"%s\t%s\n",s->name,s->name);
			if (results->c_output_fp != NULL) {
				print_fasta(results->c_output_fp, s->name, s->comment, s->seq);
			}
		}
	}

	sd_delete_hash(); /* free the hash nodes */
}

/* hash functions */
/**
 * sd_add_seq
 * checks if the key is in the hash. If yes, just increment count; if
 * no, add the new entry to the hash.
 */
void sd_add_seq(kseq_t *seq, int file, int use_header)
{
	sd_lookup_t *s;

	if (use_header)
		HASH_FIND_STR(sd_lookup,seq->name.s,s);
	else
		HASH_FIND_STR(sd_lookup,seq->seq.s,s);

	if (s==NULL) { /* key is not in hash */
		s = (sd_lookup_t *)malloc(sizeof(sd_lookup_t));
		if (s == NULL) {
			fprintf(stderr,"couldn't get memory for sd_lookup_t\n");
			exit(EXIT_FAILURE);
		} else {
			/* initialize struct */
			s->seq = NULL;
			s->name = NULL;
			s->count = 1;
			s->in_a = 0;
			s->in_b = 0;
		}

		s->seq = (char *)malloc((sizeof(char*) * (strlen(seq->seq.s)+1)));
		if (s->seq == NULL) {
			fprintf(stderr,"couldn't get memory for seq string\n");
			exit(EXIT_FAILURE);
		} else
			strncpy(s->seq,seq->seq.s,strlen(seq->seq.s)+1);

		s->name = (char *)malloc((sizeof(char*) * (strlen(seq->name.s)+1)));
		if (s->name == NULL) {
			fprintf(stderr,"couldn't get memory for name string\n");
			exit(EXIT_FAILURE);
		} else
			strncpy(s->name,seq->name.s,strlen(seq->name.s)+1);

		if (seq->comment.s != NULL) {
			s->comment = (char *)malloc((sizeof(char*) * (strlen(seq->comment.s)+1)));
			if (s->comment == NULL) {
				fprintf(stderr,"couldn't get memory for comment string\n");
				exit(EXIT_FAILURE);
			} else
				strncpy(s->comment,seq->comment.s,strlen(seq->comment.s)+1);
		} else
			s->comment = NULL;

		if (file == 1)
			s->in_a = 1;
		else if (file == 2)
			s->in_b = 1;
		if (use_header)
			HASH_ADD_KEYPTR( hh, sd_lookup, s->name, strlen(s->name), s );
		else
			HASH_ADD_KEYPTR( hh, sd_lookup, s->seq, strlen(s->seq), s );
	} else {
		s->count++; /* key is already in the hash, just incr count */
		if (file == 1)
			s->in_a = 1;
		else if (file == 2)
			s->in_b = 1;
	}
}

sd_lookup_t *sd_find_seq(char *str, int use_header)
{
	sd_lookup_t *s;
	HASH_FIND_STR(sd_lookup, str, s);
	if (s)
		return s;
	else
		return (sd_lookup_t *)NULL;
}

void sd_delete_seq(sd_lookup_t *s)
{
	HASH_DEL(sd_lookup, s);
	free(s->seq);
	free(s->name);
	free(s);
}

void sd_delete_hash()
{
	sd_lookup_t *current_seq, *tmp;
	HASH_ITER(hh,sd_lookup,current_seq,tmp) {
		sd_delete_seq(current_seq);
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

