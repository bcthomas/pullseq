#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "hash.h"

lookup_t *lookup = NULL;

void add_name(char *name)
{
	lookup_t *s;
	s = (lookup_t *)malloc(sizeof(lookup_t));
	if (s == NULL) {
		fprintf(stderr,"couldn't get memory for lookup_t\n");
		exit(EXIT_FAILURE);
	}
	s->name = (char *)malloc(strlen(name) + 1);
	strcpy(s->name,name);

	HASH_ADD_KEYPTR( hh, lookup, s->name, strlen(s->name), s );
}

lookup_t *find_name(char *name)
{
	lookup_t *s;
	HASH_FIND_STR(lookup, name, s);
	if (s)
		return s;
	else
		return (lookup_t *)NULL;
}

void delete_name(lookup_t *s)
{
	HASH_DEL(lookup, s);
	free(s->name);
	free(s);
}

void print_hash(void)
{
	lookup_t *s;
	for(s=lookup;s!=NULL;s=s->hh.next)
		fprintf(stderr,"name %s\n",s->name);
}

void delete_hash()
{
	lookup_t *current_name, *tmp;
	HASH_ITER(hh,lookup,current_name,tmp) {
		HASH_DEL(lookup, current_name);
		free(current_name->name);
		free(current_name);
	}
}

int hash_key_count(void)
{
	lookup_t *s;
	int count = 0;
	for(s=lookup;s!=NULL;s=s->hh.next)
		count++;
	return(count);
}
