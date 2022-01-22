#define PCRE2_CODE_UNIT_WIDTH 8

#include <stdio.h>
#include <string.h>
#include <pcre2.h>

#include "search_header.h"
#include "global.h"


/* re is a compiled pcre2 regex */
int search_header(pcre2_code *re, char *str) {
	int pcreExecRet;
	pcre2_match_data *match_data;

	if (str == NULL) {
		return 0;
	}
	match_data = pcre2_match_data_create_from_pattern(re, NULL); // init structure for result

	/* run the match */
	pcreExecRet = pcre2_match(re,
			str, 
			strlen(str),    // length of header string
			0,                      // Start looking at this point
			0,                      // pcre exec OPTIONS
			match_data, // pcre2_match_data
			NULL); // default match context

	pcre2_match_data_free(match_data);   /* Release memory used for the match */

	if (pcreExecRet < 0) {
		if (pcreExecRet == PCRE2_ERROR_NOMATCH)
			return 0;
		else
			fprintf(stderr, "Problem with your regex (%d)\n", pcreExecRet);
	} else
		return 1;
}
