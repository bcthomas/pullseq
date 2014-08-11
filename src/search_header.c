#include <stdio.h>
/* #include <stdlib.h> */
#include <string.h>
#include <pcre.h>

#include "search_header.h"
#include "global.h"


int search_header(pcre *re, pcre_extra *re_extra, char *str) {
	int pcreExecRet;
	int pcre_ovector[MAX_CAPTURE_COUNT];


	pcreExecRet = pcre_exec(re, re_extra,
			str, 
			strlen(str),    // length of header string
			0,                      // Start looking at this point
			0,                      // pcre exec OPTIONS
			pcre_ovector,           // capture groups vector
			MAX_CAPTURE_COUNT);     // Length of output capture groups

	if (pcreExecRet < 0) {
		if (pcreExecRet == PCRE_ERROR_NOMATCH)
			return 0;
		else
			fprintf(stderr, "Problem with your regex (%d)\n", pcreExecRet);
	} else
		return 1;
}
