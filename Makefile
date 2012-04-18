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

#file_read.o: $(SDIR)/file_read.c $(SDIR)/file_read.h
file_read.o: $(SDIR)/file_read.c
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c # -o $(SDIR)/$*.o

#linked_list.o: $(SDIR)/linked_list.c $(SDIR)/linked_list.h
linked_list.o: $(SDIR)/linked_list.c
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c # -o $(SDIR)/$*.o

#pull_from_list.o: $(SDIR)/pull_from_list.c $(SDIR)/pull_from_list.h $(SDIR)/kseq.h
pull_from_list.o: $(SDIR)/pull_from_list.c
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c # -o $(SDIR)/$*.o

pullseq.o: $(SDIR)/pullseq.c
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.c # -o $(SDIR)/$*.o

clean:
	rm -rf *.o $(SDIR)/*.o $(SDIR)/*.gch ./pullseq

distclean: clean
	rm -rf *.tar.gz

dist:
	tar -zcf $(ARCHIVE).tar.gz *.ch Makefile

build: file_read.o linked_list.o pull_from_list.o pullseq.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPT) $? -o pullseq

debug:
	$(MAKE) build "CFLAGS=-Wall -pedantic -g -DDEBUG"

