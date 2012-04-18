#include "pullseq.h"

int main(int argc, char *argv[]) {
  int i;
  node *n;
  list_t *list;
  char *words[3] = {"one","two","three"};

  list = (list_t *) malloc(sizeof(list_t));
  initialize_list(list);
  n = (node *) NULL;


  for(i=0; i<3; i++) {
    fprintf(stderr,"creating node for word %s\n",words[i]);
    n = initnode(words[i]);
    add_to_list(list,n);
  }

  n = list->head;
  while(n != NULL) {
    fprintf(stderr,"%p: %s\n",n,n->word);
    n = n->next;
  }

  n = search_list(list,"two");
  fprintf(stderr,"found %s (%p)\n", n->word,n);
  n = search_list(list,"wo");
  if (n == NULL)
    fprintf(stderr,"did not find \n");

  delete_list(list,list->head);
  return EXIT_SUCCESS;
}
