#ifndef SEQDIFF_RESULTS_H
#define SEQDIFF_RESULTS_H

typedef struct _seqdiff_results_t {
	int first_file_total;
	int first_file_uniq;
	int second_file_total;
	int second_file_uniq;
	int common;
	char *first_file;
	char *second_file;
	char *a_output_file;
	char *b_output_file;
	char *c_output_file;
	int use_headers;
	int use_gt;
} seqdiff_results_t;

seqdiff_results_t *seqdiff_results_init(void);
void seqdiff_results_destroy(seqdiff_results_t *results);

#endif
