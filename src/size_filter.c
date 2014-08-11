#include "global.h"
#include "output.h"
#include <stdio.h>

int size_filter(kseq_t *seq, int is_fasta, int min, int max, int length, int convert, int just_count) {
	int count=0;
	if (min > 0 && max > 0) { /* got a min and max */
		if (seq->seq.l >= min && seq->seq.l <= max) {
			count++;
			if (!just_count) {
				if (convert)
					is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
				else
					is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
			}
		}
	} else if (min > 0 || max > 0) { /* either  min or max is 0 */
		if (min > 0 && seq->seq.l >= min) {
			count++;
			if (!just_count) {
				if (convert)
					is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
				else
					is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
			}
		} else if (max > 0 && seq->seq.l <= max) {
			count++;
			if (!just_count) {
				if (convert)
					is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
				else
					is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
			}
		}
	} else {
		/* neither min nor max was > 0, so we print this sequence */
		count++;
		if (!just_count) {
			if (convert)
				is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
			else
				is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
		}
	}
	return count;
}
