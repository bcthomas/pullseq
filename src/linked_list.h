
typedef struct _node {
  char *word;
  struct _node *next;
} node ;

typedef struct _list_t {
  node *head;
  node *end;
} list_t;

void initialize_list(list_t *list);
node * initnode(char *word);
void add_to_list(list_t *list, node *n);
node *search_list(list_t *list, char *word);
void delete_list(list_t *list,node *n);
