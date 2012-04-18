Summary:
  pullseq: extract sequences from a fasta/fastq file

Synopsis:
  # general extraction with a list of names
  pullseq --input=<input fasta/fastq file> --names=<fasta header ids file>
  
  # general extraction with a minimum size requirement
  pullseq --input=<input fasta/fastq file> --min=<minimum size sequence to extract>
  
  # only sequences with min 200 and max 500
  pullseq -i input.fasta -m 200 -a 500 > new.fasta

Options:
    --input, -i <s>:   contig file (fasta format)
    --names, -n <s>:   file containing single column list of contig_ids
      --min, -m <i>:   minimum size contig to pull
      --max, -a <i>:   maximum size contig to pull
      --exclude, -e:   contigs list will be excluded
      --version, -v:   Print version and exit
         --help, -h:   Show this message




# c parts
1. sequence parts
kseq.h:
  kseq_t
  kstring_t

2. filter file list

struct
  list

linked_list
readline # read from input
 FILE
parseline # grab 1st word

main
filter # find matching header or minimum/maximum seq length
print # dump to file or if -o option, open file and dump

Scenarios
1) given a fasta file and a file of header names to select
  open header names file
  read each line and extract the first word
  create a linked_list node for the word
  close the file
  open the fasta file
  read each kseq_t entry
  search for kseq_t entry in linked list of header names
  if found, print
  close fasta file

2) given a fasta file and a minimum length
  open the fasta file
  read each kseq_t entry
  search for kseq_t entry where sequence length >= cutoff
  if found, print
  close fasta file
