#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "kseq.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

extern char const *progname;
extern int verbose_flag;

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
				i = 0; /* reset i */
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
}
