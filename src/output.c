#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

extern char const *progname;
extern int verbose_flag;
extern int QUALITY_SCORE;

void print_fastq_seq(kseq_t *seq)
{
	char *qual_str = NULL;
	int i=0;
	int l=strlen(seq->seq.s) + 1;     /* sequence length */
	if (seq->qual.s == NULL) {        /* just use a default value for the quality code */
		qual_str = (char *)malloc(sizeof(char) * l);
		for (i=0;i<l;i++)
			qual_str[i] = QUALITY_SCORE;
		qual_str[l - 1] = '\0';
		if (seq->comment.s == NULL)
			printf("@%s\n%s\n+\n%s\n",seq->name.s,seq->seq.s,qual_str);
		else
			printf("@%s %s\n%s\n+\n%s\n",seq->name.s,seq->comment.s,seq->seq.s,qual_str);
		free(qual_str);
	} else {
		if (seq->comment.s == NULL)
			printf("@%s\n%s\n+\n%s\n",seq->name.s,seq->seq.s,seq->qual.s);
		else
			printf("@%s %s\n%s\n+\n%s\n",seq->name.s,seq->comment.s,seq->seq.s,seq->qual.s);
	}
}

void print_fasta_seq(kseq_t *seq, int n)
{
	int l = seq->seq.l;   /* sequence length */
	int x,i=0;
	char *seqbuf = NULL;
	seqbuf = (char *)malloc(sizeof(char) * (n + 1));
	if (seqbuf == NULL) {
		fprintf(stderr,"print_seq: out of memory for seqbuf!\n");
		exit(EXIT_FAILURE);
	}

	if (n <= 0)
		n = 50;

	if (l > n) {                  /* seqlength is > column length - split sequence */
		if (seq->comment.s == NULL) {
			printf(">%s\n",seq->name.s);
		} else {
			printf(">%s %s\n",seq->name.s,seq->comment.s);
		}

		for (x=0; x<l;x++) {
			if (i < n) {                     /* there's less sequence than the column width */
				seqbuf[i] = seq->seq.s[x];
				i++;
			} else {                         /* i is >= column width, so print this line */
				seqbuf[i] = '\0';            /* set last position in string to null */
				printf("%s\n",seqbuf);       /* print this line */
				i = 0;                       /* reset i */
				seqbuf[0] = '\0';            /* reset buffer */
				seqbuf[i] = seq->seq.s[x];   /* set this buffer line to current sequence char */
				i++;
			}
		}
		if (i<n)
			seqbuf[i] = '\0';
		if (strlen(seqbuf) > 0)
			printf("%s\n",seqbuf);
	} else {                     /* seqlength < column length, so just print the full sequence */
		if (seq->comment.s == NULL)
			printf(">%s\n%s\n",seq->name.s,seq->seq.s);
		else
			printf(">%s %s\n%s\n",seq->name.s,seq->comment.s,seq->seq.s);
	}
	free(seqbuf);
}

void print_fasta(FILE *fp, char *name, char *comment, char *seq)
{
	int l = strlen(seq);   /* sequence length */
	int x,i=0;
	int n = 50;
	char *seqbuf = NULL;
	seqbuf = (char *)malloc(sizeof(char) * (n + 1));
	if (seqbuf == NULL) {
		fprintf(stderr,"print_seq: out of memory for seqbuf!\n");
		exit(EXIT_FAILURE);
	}

	if (l > n) {                  /* seqlength is > column length - split sequence */
		if (comment == NULL)
			fprintf(fp, ">%s\n",name);
		else
			fprintf(fp, ">%s %s\n",name,comment);

		for (x=0; x<l;x++) {
			if (i < n) {                     /* there's less sequence than the column width */
				seqbuf[i] = seq[x];
				i++;
			} else {                         /* i is >= column width, so print this line */
				seqbuf[i] = '\0';            /* set last position in string to null */
				fprintf(fp, "%s\n",seqbuf);       /* print this line */
				i = 0;                       /* reset i */
				seqbuf[0] = '\0';            /* reset buffer */
				seqbuf[i] = seq[x];   /* set this buffer line to current sequence char */
				i++;
			}
		}
		if (i<n)
			seqbuf[i] = '\0';
		if (strlen(seqbuf) > 0)
			fprintf(fp, "%s\n",seqbuf);
	} else {                     /* seqlength < column length, so just print the full sequence */
		if (comment == NULL)
			fprintf(fp, ">%s\n%s\n",name,seq);
		else
			fprintf(fp, ">%s %s\n%s\n",name,comment,seq);
	}
	free(seqbuf);
}




/*
   void print_seq(kseq_t *seq, int n)
   {
   int l = seq->seq.l;
   int x,i=0;
   char *seqbuf = NULL;
   seqbuf = (char *)malloc(sizeof(char) * (n + 1));
   if (seqbuf == NULL) {
   fprintf(stderr,"print_seq: out of memory for seqbuf!\n");
   exit(EXIT_FAILURE);
   }

   if (n <= 0)
   n = 50;

   if (l > n) {
   int bins = 0;
   if ((l % n) == 0)
   bins = l / n;
   else
   bins = (l / n) + 1;

   if (seq->comment.s == NULL) {
   printf(">%s\n",seq->name.s);
   } else {
   printf(">%s %s\n",seq->name.s,seq->comment.s);
   }

   for (x=0; x<l;x++) {
   if (i < n) {
   seqbuf[i] = seq->seq.s[x];
   i++;
   } else {
   seqbuf[i] = '\0';
   i = 0; //reset i
   printf("%s\n",seqbuf);
   seqbuf[0] = '\0';
   seqbuf[i] = seq->seq.s[x];
   i++;
   }
   }
   if (i<n)
   seqbuf[i] = '\0';
   if (strlen(seqbuf) > 0)
   printf("%s\n",seqbuf);
   } else {
   if (seq->comment.s == NULL)
   printf(">%s\n%s\n",seq->name.s,seq->seq.s);
   else
   printf(">%s %s\n%s\n",seq->name.s,seq->comment.s,seq->seq.s);
   }
   free(seqbuf);
   } */
