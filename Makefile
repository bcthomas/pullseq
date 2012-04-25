PROGRAM_NAME = main
VERSION = 0.1
CC = gcc
CFLAGS = -g -Wall -pedantic -DVERSION=$(VERSION)
DEBUG = -g
OPT = -O3
ARCHIVE = $(PROGRAM_NAME)_$(VERSION)
LDFLAGS = -lz
SDIR = src

.PHONY: clean default build distclean dist debug

default: build

file_read.o: $(SDIR)/file_read.c $(SDIR)/file_read.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c

bst.o: $(SDIR)/bst.c $(SDIR)/bst.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c

output.o: $(SDIR)/output.c $(SDIR)/output.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c

pull_by_name.o: $(SDIR)/pull_by_name.c $(SDIR)/pull_by_name.h $(SDIR)/kseq.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c

pull_by_size.o: $(SDIR)/pull_by_size.c $(SDIR)/pull_by_size.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c

pullseq.o: $(SDIR)/pullseq.c $(SDIR)/pullseq.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c

clean:
	rm -rf *.o $(SDIR)/*.o $(SDIR)/*.gch ./pullseq

distclean: clean
	rm -rf *.tar.gz

dist:
	tar -zcf $(ARCHIVE).tar.gz *.ch Makefile

build: file_read.o bst.o output.o pull_by_name.o pull_by_size.o pullseq.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPT) $? -o pullseq

debug:
	$(MAKE) build "CFLAGS=-Wall -pedantic -g -DDEBUG"

