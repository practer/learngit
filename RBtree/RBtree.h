#ifndef RBTREE_H
#define RBTREE_H

typedef struct rb_tree *rb_tree;

/* Creates an empty Red-Black tree. */
rb_tree RBcreate();
/* Frees an entire tree.*/
void RBfree(rb_tree tree);
/* cleans up.*/
void RBcleanup();

/* Inserts an element with specified key into tree.*/
int RBinsert(rb_tree, int key);

/* Deletes an element with a particular key.*/
int RBdelete(rb_tree, int key);

/* Writers a tree to stdout in preorder format.
 * Outputs everything on the same line. */
void RBwrite(rb_tree);
/* Reads a tree in preorder format from file. */
rb_tree RBread(char *fname);

/* Draws an SVG picture of the tree int the specified file. */
void RBdraw(rb_tree tree, char *fname);

#endif // RBTREE_H
