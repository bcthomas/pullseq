
typedef struct _node {
  char *name;
  struct _node *left;
  struct _node *right;
  struct _node *parent;
} node_t ;

typedef struct _tree {
	node_t *root;
} tree_t;

node_t *initnode(char *word);
int insertnode(tree_t *tree, char *name);
node_t *searchtree(tree_t *tree, char *word);
int deletenode(tree_t *tree, char *name);
void deletetree(tree_t *tree);
int compare(char *left, char *right);
void print_inorder(node_t *node);
void print_preorder(node_t *node);
void print_postorder(node_t *node);
