#include "seqdiff_results.h"
#include <stdlib.h>
#include <stdio.h>

seqdiff_results_t *seqdiff_results_init(void) {
	seqdiff_results_t *r;
	r = (seqdiff_results_t *)malloc(sizeof(seqdiff_results_t));
	if (!r) {
		fprintf(stderr, "ERROR: could not allocate memory for seqdiff_results_t structure\n");
		exit(1);
	}
	
	r->first_file_total = 0;
	r->first_file_uniq = 0;
	r->second_file_total = 0;
	r->second_file_uniq = 0;
	r->common = 0;
	r->first_file = NULL;
	r->second_file = NULL;
	r->a_output_fp = NULL;
	r->b_output_fp = NULL;
	r->c_output_fp = NULL;
	r->use_header = 0;
	r->only_summarize = 0;
	return r;
}

void seqdiff_results_destroy(seqdiff_results_t *results) {
	free(results);
}

