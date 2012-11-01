Summary:
  pullseq: extract sequences from a fasta/fastq file.  This program is
  fast, and can be useful in a variety of situations.  You can use it to
  extract sequences from one fasta/fastq file into a new file, given
  either a list of header ids to include / exclude or a size minimum /
  maximum sequence lengths.

  Additionally, it can convert from fastq to fasta or visa-versa and
  can change the length of the output sequence lines.

  NOTE: pullseq prints to standard out, so you need to use redirection
  (>) to create outpus files.

Synopsis:
  # general extraction with a list of names
  pullseq --input=<input fasta/fastq file> --names=<fasta header ids file>
  
  # general extraction with a minimum size requirement
  pullseq --input=<input fasta/fastq file> --min=<minimum size sequence to extract>
  
  # only sequences with min 200 and max 500
  pullseq -i input.fasta -m 200 -a 500 > new.fasta

  Options:
    -i, --input,     Input fasta/fastq file (required)
    -n, --names,     File of header id names to select
    -m, --min,       Minimum sequence length
    -a, --max,       Maximum sequence length
    -l, --length,    Sequence characters per line (default 50)
    -c, --convert,   Convert input to fastq/fasta (e.g. if input is fastq, output will be fasta)
    -q, --quality,   ASCII code to use for fasta->fastq quality conversions
    -e, --excluded,  Exclude the header id names in the list (-n)
    -h, --help,      Display this help and exit
    -v, --verbose,   Print extra details during the run
    --version,       Output version information and exit
