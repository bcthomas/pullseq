#ifndef SEQDIFF_RESULTS_H
#define SEQDIFF_RESULTS_H

#include <stdio.h>

typedef struct _seqdiff_results_t {
	int first_file_total;
	int first_file_uniq;
	int second_file_total;
	int second_file_uniq;
	int common;
	char *first_file;
	char *second_file;
	FILE *a_output_fp;
	FILE *b_output_fp;
	FILE *c_output_fp;
	int use_header;
	int only_summarize;
} seqdiff_results_t;

seqdiff_results_t *seqdiff_results_init(void);
void seqdiff_results_destroy(seqdiff_results_t *results);

#endif
