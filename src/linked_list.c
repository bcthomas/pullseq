#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linked_list.h"
#include "global.h"

/* initialize the list */
void initialize_list(list_t *list)
{
  list->head =  NULL;
  list->end =  NULL;
}

/* create a new node and add the word */
/* return l to newnode */
node *initnode(char *word)
{
  node *newnode;
  newnode = (node *)malloc(sizeof(node));
  if (newnode == NULL)
    return (node *) NULL; /* OOM */
  else {
    char *w;
    w = (char *)malloc(strlen(word)+1);
    if (!w)
      return NULL;
    newnode->word = w;
    strcpy(newnode->word, word);
    /*newnode->word = strndup(word,strlen(word)+1);*/
    newnode->next = NULL;
    return newnode;
  }
}


/* adds a node to given list */
/* returns l to "current" node */
void add_to_list(list_t *list, node *n)
{
  if (list->head == NULL) {
    list->head = n; /* first in list */
    list->end = n;
  } else {
    list->end->next = n;
    list->end = n;
  }
  /*fprintf(stderr,"n is %p; head is %p; end is %p\n",n,list->head,list->end);*/
}

/* find first node with this word */
/* returns node with word */
node *search_list(list_t *list, char *word)
{
  node *l = list->head;
  while(l != NULL) {
    if (strcmp(word, l->word) == 0)
      return l;
    l = l->next;
    if( l == NULL )
      break;
  }
  return NULL;
}

/* chuck the whole list */
void delete_list(list_t *list, node *n)
{
  node *temp;

  if ( list->head == NULL ) return;   /* dont try to delete an empty list       */

  if (n == list->head) {    /* if we are deleting the entire list         */
    list->head = NULL;         /* then reset head and end to signify empty   */
    list->end = NULL;          /* list                                       */
  } else {
    temp = list->head;         /* if its not the entire list, readjust end  */
    while( temp->next != n )         /* locate previous node to l  */
    temp = temp->next;
    list->end = temp;                        /* set end to node before l   */
  }

  while( n != NULL ) {   /* whilst there are still nodes to delete     */
    temp = n->next;     /* record address of next node                */
    free(n->word);          /* free this node                             */
    free(n);          /* free this node                             */
    n = temp;           /* point to next node to be deleted           */
  }
}
