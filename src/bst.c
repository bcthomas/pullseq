#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "bst.h"

/* create a new node */
/* return pointer to new node */
node_t *initnode(char *name)
{
	node_t *newnode;
	newnode = (node_t *)malloc(sizeof(node_t));
	if (newnode == NULL)
		return (node_t *)NULL; /* OOM */
	else {
		char *newname;
		newname = (char *)malloc(strlen(name)+1); /* alloc space for name */
		if (!newname)
			return (node_t *)NULL; /* oom */
		newnode->name = newname;
		strcpy(newnode->name, name);

		newnode->parent = newnode->left = newnode->right = (node_t *)NULL;
		return newnode;
	}
}

/* simple alphabetic comparison */
int compare(char *left, char *right)
{
	int result = 0;
	result = strcmp(left,right);
	if (result == 0)
		return 0; /* equal */
	else if (result > 0)
		return 1; /* gt */
	else if (result < 0)
		return -1; /* lt */
	return result;
}

/* find node with this name */
/* returns node or NULL if not found */
node_t *searchtree(tree_t *tree, char *name)
{
	int result;
	node_t *node = tree->root;

	while(node != NULL) {
		result = compare(name,node->name);
		if (result == 0) {
			return node;
		} else if (result < 0) {
			node = node->left;
		} else if (result > 0) {
			node = node->right;
		} else
			break;
	}
	return (node_t *)NULL;
}

/* adds a node to tree */
/* returns 0 or 1 depending on success/fail */
int insertnode(tree_t *tree, char *name)
{
	int result;
	node_t *node;

	if (tree->root == NULL) {
		tree->root = initnode(name);
	} else {
		node = tree->root;
		while(1) {
			result = compare(name,node->name);

			if (result == 0)
				return 0; /* return fail, since data is already in tree */
			else if (result < 1) { /*left*/
				if (node->left == NULL)
					break;
				else
					node = node->left;
			} else { /*right*/
				if (node->right == NULL)
					break;
				else
					node = node->right;
			}
		} /* while(1) */

		/* we've broken from the loop, so we have a NULL leaf*/
		if (result < 1) {
			node->left = initnode(name);
			node->left->parent = node;
		} else {
			node->right = initnode(name);
			node->right->parent = node;
		}
	}
	return 1;
}

/* delete node from tree */
int deletenode(tree_t *tree, char *name)
{
	if (tree->root != NULL) {
		node_t head = {NULL,NULL,NULL,NULL};
		node_t *node = &head;
		node_t *t = NULL;
		int result;

		node->right = tree->root; /* point the head node at the tree top */
		tree->root->parent = &head; /* also point the tree root's head */
	
		/* walk the tree, looking for data to delete */
		while(1) {
			result = compare(name,node->name);

			if (result == 0) /* found the node to delete */
				break;
			else if (result < 1) { /*left*/
				if (node->left == NULL)
					return 0; /* not found */
				else
					node = node->left;
			} else { /*right*/
				if (node->right == NULL)
					return 0; /* not found */
				else
					node = node->right;
			}
		} /* while(1) */


		/* if we found matching name, f is pointing to the matching
		 * node */
		if (node != NULL) {
			if (node->left != NULL && node->right != NULL) { /* two children */
				t = node->right;
				while (t->left != NULL) {
					t = t->left;
				}
				t->parent = node->parent;
				if (node->parent->right == node)
					node->parent->right = t;
				else
					node->parent->left = t;
				free(node);
			} else if (node->left == NULL && node->right == NULL) {
				/* leaf */
				free(node);
			} else if (node->left == NULL) {
				/* set right */
				t = node->right; /* temp copy of right node */
				node->right->parent = node->parent;
				node->parent->right = t;
				free(node);
			} else {
				/* set set left */
				t = node->left; /* temp copy of left node */
				node->left->parent = node->parent;
				node->parent->left = t;
				free(node);
			}
		}
	}
	/* tree->root is NULL, so just return */
	return 1;
}

/* print from node down, inorder */
void print_inorder(node_t *node)
{
	if (node == NULL)
		return;
	print_inorder(node->left);
	fprintf(stderr,"%s\n",node->name);
	print_inorder(node->right);
	return;
}

/* print from node down, preorder */
void print_preorder(node_t *node)
{
	if (node == NULL)
		return;
	fprintf(stderr,"%s\n",node->name);
	print_preorder(node->left);
	print_preorder(node->right);
	return;
}

/* print from node down, postorder */
void print_postorder(node_t *node)
{
	if (node == NULL)
		return;
	print_postorder(node->left);
	print_postorder(node->right);
	fprintf(stderr,"%s\n",node->name);
	return;
}

void deletetreenode(node_t *node)
{
	while(1) {
		if (node == NULL)
			break;
		else if (node->left != NULL) {
			deletetreenode(node->left);
			node->left = NULL;
		} else if (node->right != NULL) {
			deletetreenode(node->right);
			node->right = NULL;
		} else {
			free(node->name);
			free(node);
			return;
		}
	}
	return;
}

/* chuck the whole list */
void deletetree(tree_t *tree)
{
	if (tree->root == NULL) {
		return;
	} else {
		deletetreenode(tree->root);
	}
	free(tree);
	return;
}
