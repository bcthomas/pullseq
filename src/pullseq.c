#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>
#include <getopt.h>

#include "global.h"
#include "pullseq.h"
#include "pull_by_name.h"
#include "pull_by_size.h"

extern int QUALITY_SCORE;
extern int verbose_flag;

void show_usage(int status) {
  fprintf(stderr, "pullseq - a bioinformatics tool for manipulating fasta and fastq files\n");
  fprintf(stderr, "\n(Written by bct - 2012)     ");
  fprintf(stderr, "search method: %s",PULLSEQ_SORTMETHOD);
  fprintf(stderr, "\nUsage:\n"
                  "%s -i <input fasta/fastq file> -n <header names to select>\n\n"
                  "%s -i <input fasta/fastq file> -m <minimum sequence length>\n\n"
                  "%s -i <input fasta/fastq file> -m <minimum sequence length> -a <max sequence length>\n\n",
                  progname,progname,progname);
  fprintf(stderr, "  Options:\n"
                  "    -i, --input,     Input fasta/fastq file (required)\n"
                  "    -n, --names,     File of header id names to select\n");

  fprintf(stderr, "    -m, --min,       Minimum sequence length\n"
                  "    -a, --max,       Maximum sequence length\n"
                  "    -l, --length,    Sequence characters per line (default 50)\n");
  fprintf(stderr, "    -c, --convert,   Convert input to fastq/fasta (e.g. if input is fastq, output will be fasta)\n"
  				  "    -q, --quality,   ASCII code to use for fasta->fastq quality conversions\n"
                  "    -e, --excluded,  Exclude the header id names in the list (-n)\n");
  fprintf(stderr, "    -h, --help,      Display this help and exit\n"
                  "    -v, --verbose,   Print extra details during the run\n"
                  "    --version,       Output version information and exit\n\n");

  exit(status);
}

int main(int argc, char *argv[]) {
  int c;
  char *in = NULL,*names = NULL;
  int min = -1, max = -1;
  int exclude = 0;
  int count = 0;
  int convert = 0;
  int length = 50;
  long value;
  char *end;

  extern char *optarg; /* external from getopt */

  verbose_flag = 0; /* assume not verbose */

  progname = argv[0];
  if (argc < 4) {
    show_usage(EXIT_FAILURE);
  }

  while(1) {
    static struct option long_options[] =
        {
          {"verbose", no_argument, 0, 'v'},
          {"convert", no_argument, 0, 'c'},
          {"exclude", no_argument, 0, 'e'},
          {"version", no_argument, 0, 'V'},
          {"help",    no_argument, 0, 'h'},
          {"input",   required_argument, 0, 'i'},
          {"names",   required_argument, 0, 'n'},
          {"min",     required_argument, 0, 'm'},
          {"max",     required_argument, 0, 'a'},
          {"length",  required_argument, 0, 'l'},
          {"quality", required_argument, 0, 'q'},
          {0, 0, 0, 0}
        };

    /* getopt_long stores the option index here. */
    int option_index = 0;
 
    c = getopt_long (argc, argv, "Vvh?ecq:i:n:m:a:l:", long_options, &option_index);
 
    /* Detect the end of the options. */
    if (c == -1)
      break;
 
    switch (c) {
      case 'v':
        verbose_flag = 1;
        break;

      case 'i':
        in = (char*) malloc(strlen(optarg)+1);
        strcpy(in,optarg);
        break;
     
      case 'n':
        names = (char*) malloc(strlen(optarg)+1);
        strcpy(names,optarg);
        break;

      case 'm':
		value = strtol(optarg,&end,0);
		if (*end == '\0' && errno == 0) {
				min = atoi(optarg);
		} else {
				fprintf(stderr, "Maximum value (-m) argument '%s' is not an integer\n",optarg);
				return EXIT_FAILURE;
		}
        break;
 
      case 'a':
		value = strtol(optarg,&end,0);
		if (*end == '\0' && errno == 0) {
				max = atoi(optarg);
		} else {
				fprintf(stderr, "Maximum value (-a) argument '%s' is not an integer\n",optarg);
				return EXIT_FAILURE;
		}
        break;

      case 'c':
        convert = 1;
        break;

      case 'q':
		value = strtol(optarg,&end,0);
		if (*end == 0 && errno == 0) {
				QUALITY_SCORE = atoi(optarg);
		} else {
				fprintf(stderr, "Quality ASCII value (-q) is invalid - must be an ASCII code (e.g. 73, which is 'I')\n");
				return EXIT_FAILURE;
		}
        break;

      case 'e':
        exclude = 1;
        break;

      case 'l':
		value = strtol(optarg,&end,0);
		if (*end == '\0' && errno == 0) {
			length = atoi(optarg);
		} else {
				fprintf(stderr, "Sequence length value (-l) argument '%s' is not an integer\n",optarg);
				return EXIT_FAILURE;
		}
        break;

      case 'V':
        /* version */
        printf("Version is %s\n","version");
        break;

      case 'h':
        show_usage(EXIT_FAILURE);
        break;

      case '?':
        /* getopt_long already printed an error message. */
        break;
 
      default:
        abort ();
    }
  }
 
   /* Instead of reporting '--verbose'
   and '--brief' as they are encountered,
   we report the final status resulting from them. */
	if (verbose_flag) {
		fprintf(stderr, "verbose flag is set\n");
		fprintf(stderr,"Input is %s\n", in);
		if (convert)
			fprintf(stderr,"Input will be converted between FASTQ and FASTA\n");
		if (exclude)
			fprintf(stderr,"Names in %s will be excluded\n", names);
		else
			fprintf(stderr,"Names in %s will be included\n", names);
		if (max > 0)
			fprintf(stderr,"Only sequences less than %i will be output\n", max);
		if (min > 0)
			fprintf(stderr,"Only sequences greater than %i will be output\n", min);
		if (length > 0)
			fprintf(stderr,"Output will be %i columns long\n", length);
	}
 
  /* check validity of given argument set */
  if (!in) {
    fprintf (stderr, "Error: Input file is required.\n");
    return EXIT_FAILURE;
  }

  if (names) {
    if (!strcmp(in,names)) {
      fprintf (stderr, "Error: Input file is same as names file.\n");
      return EXIT_FAILURE;
    }
  }

  if (min > 0 && max > 0) {
    if (max <= min) {
      fprintf (stderr, "Error: Max is less than or equal to min.\n");
      return EXIT_FAILURE;
    }
  }

  if (names) {
    count = pull_by_name(in,names,min,max,length,exclude,convert);
  } else {
    count = pull_by_size(in,min,max,length,convert);
  }

  free(in);
  if (names) {
    free(names);
  }
  if (verbose_flag)
	  fprintf(stderr,"Pulled %i entries\n",count);
  fclose(stderr);
  fclose(stdout);
  fclose(stdin);
  return EXIT_SUCCESS;
}
