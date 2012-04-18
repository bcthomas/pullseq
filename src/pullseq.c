#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>
#include <getopt.h>

#include "global.h"
#include "pullseq.h"
#include "pull_from_list.h"


void show_usage(int status) {
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
                  "    -e, --excluded,  Exclude the header id names in the list (-n)\n");
  fprintf(stderr, "    -h, --help,      Display this help and exit\n"
                  "    --verbose,       Print extra details during the run\n"
                  "    --version,       Output version information and exit\n\n");  

  exit(status);
}

int main(int argc, char *argv[]) {
  int c;
  char *in = NULL,*names = NULL;
  int min = -1, max = -1;
  int exclude = 0;

  extern char *optarg; /* external from getopt */

  progname = argv[0];
  if (argc < 5) {
    show_usage(EXIT_FAILURE);
  }

  while(1) {
    static struct option long_options[] =
        {
          /* These options set a flag. */
          {"verbose",  no_argument,       &verbose_flag, 1},
          {"brief",    no_argument,       &verbose_flag, 0},
          /* These options don't set a flag.
             We distinguish them by their indices.
          {"add",     no_argument,       0, 'a'},
          {"append",  no_argument,       0, 'b'}, */
          {"input",   required_argument, 0, 'i'},
          {"names",   required_argument, 0, 'n'},
          {"min",     required_argument, 0, 'm'},
          {"max",     required_argument, 0, 'a'},
          {"exclude", no_argument, 0, 'e'},
          {"version", no_argument, 0, 'V'},
          {"help",    no_argument, 0, 'h'},
          {0, 0, 0, 0}
        };

    /* getopt_long stores the option index here. */
    int option_index = 0;
 
    c = getopt_long (argc, argv, "Vh?ei:n:m:a:", long_options, &option_index);
 
    /* Detect the end of the options. */
    if (c == -1)
      break;
 
    switch (c) {
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
          break;
        printf ("option %s", long_options[option_index].name);
        if (optarg)
          printf (" with arg %s", optarg);
        printf ("\n");
        break;
 
      case 'i':
        DEBUGP("Input file (-i) has value `%s'\n", optarg);
        in = (char*) malloc(strlen(optarg)+1);
        strcpy(in,optarg);
        break;
     
      case 'n':
        DEBUGP("Names file (-n) has value `%s'\n", optarg);
        names = (char*) malloc(strlen(optarg)+1);
        strcpy(names,optarg);
        break;

      case 'm':
        DEBUGP("Minimum value (-m) is `%s'\n", optarg);
        min = atoi(optarg);
        break;
 
      case 'a':
        DEBUGP("Maximum value (-a) is `%s'\n", optarg);
        max = atoi(optarg);
        break;

      case 'e':
        DEBUGP("Items in names file will be excluded\n",NULL);
        exclude = 1;
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
  if (verbose_flag)
    puts ("verbose flag is set");
 
   /* Print any remaining command line arguments (not options). */
  if (optind < argc) {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    putchar ('\n');
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
    fprintf(stderr,"about to call pull_from_list() with %s, %s, %i, %i\n",in,names,min,max);
    pull_from_list(in,names,min,max);
  }

  free(in);
  if (names) {
    free(names);
  }
  return EXIT_SUCCESS;
}
