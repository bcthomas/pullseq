#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "pull_by_name.h"
#include "bst.h"
#include "file_read.h"
#include "global.h"
#include "output.h"
#include "kseq.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

extern char const *progname;
extern int verbose_flag;

int pull_by_name(char *input_file, char *names_file, int min, int max, int length) {
  FILE *fp;
  int i,count=0,l,capacity=80;
  node_t *n;
  tree_t *tree = (tree_t*)malloc(sizeof(tree_t*));
  char *fasta_name;
  char *line;
  kseq_t *seq;

  fp = fopen(names_file,"r");
  if (!fp) {
    fprintf(stderr,"%s - failed to open file %s\n",progname,names_file);
    exit(EXIT_FAILURE);
  }

  tree->root = NULL;                    /* initialize tree to NULL      */
  n = (node_t *) NULL;

  /* get some space for the line */
  line = malloc(sizeof(char) * capacity); /* get memory allocated */
  if (!line) {
    fprintf(stderr, "%s - line malloc: %s\n",progname, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while((i = getl(&line,fp)) != -1) {
    fasta_name = parse_name(line);
    if (fasta_name) {
       insertnode(tree,fasta_name);             /* create a new node with fasta_name */
    }
  }

  free(line); /* free up line */
  fclose(fp); /* close file */
  if (verbose_flag) {
	  fprintf(stderr,"\n");
	  fprintf(stderr,"done reading from %s\n",progname);
  }

  /* open fasta file */
  fp = gzopen(input_file,"r");
  if (!fp) {
    fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,input_file);
    exit(EXIT_FAILURE);
  }

  seq = kseq_init(fp);
  /* search through list and see if this header matches */
  while((l = kseq_read(seq)) >= 0) {
    if (searchtree(tree,seq->name.s)) {            /* found name in list */
      if (min > 0 && max > 0) { /* got a min and max */
        if (seq->seq.l >= min && seq->seq.l <= max) {
			count++;
			print_seq(seq,length);
        }
      } else if (min > 0 || max > 0) { /* either  min or max is 0 */
        if (min > 0 && seq->seq.l >= min) {
			count++;
			print_seq(seq,length);
        } else if (max > 0 && seq->seq.l <= max) {
			count++;
			print_seq(seq,length);
        }
      } else {
		count++;
		print_seq(seq,length);
      }
    }
  }
  kseq_destroy(seq);
  gzclose(fp); /* done reading file */

  deletetree(tree); /* free the list nodes */

  if (verbose_flag)
	  fprintf(stderr,"Output contained %i entries\n",count);
  return count;
}
