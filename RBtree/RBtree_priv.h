#ifndef RBTREE_PRIV_H
#define RBTREE_PRIV_H

#include "RBtree.h"
#include <stdio.h>
typedef struct rb_node{
        int key;
        struct rb_node *parent;
        struct rb_node *lchild, *rchild;

        char color;
} *rb_node;
struct rb_tree {
        rb_node root;
        rb_node nil;
};

/* our pool of nodes for faster allocation */
static rb_node rb_mem_pool = NULL;


/* Section 1: Creating and freeing trees and nodes. */
/* Helper routine: free a subtree rooted at specified node. */
static void rb_free_subtree (rb_tree tree, rb_node node);
/* Creates a new node, taking form the memory pool if available. */
static rb_node rb_new_node(rb_tree tree, int data);
/* Frees a node to the memory pool. */
static void rb_tree_node(rb_node node);

/* Section 2: Insertion */
/* corrects for properties violated on an insertion. */
static void rb_insert_fix(rb_tree tree, rb_node n);
/* Helper routine: returns the uncle of a give node. */
static rb_node rb_get_uncle(rb_tree tree, rb_node n);

/* Section 3:Deletion */
/* Helper routine: transplants node 'from' into node 'to's position. */
static void rb_transplant(rb_tree tree, rb_node to, rb_node from);
/* Corrects for properties violated on a deletion. */
static void rb_delete_fix(rb_tree tree, rb_node n);

/*Section 4:I/O */
/* Helper routine: write an entire to stdout. */
static void rb_preorder_write(rb_tree tree, rb_node n);
/* Reads a tree in preorder format, limited by the maximum value of max. */
static rb_node rb_read_subtree(rb_tree tree, rb_node *next, int max, FILE *fp);
/* Helper routine: read a single node from file fp. */
static rb_node rb_read_node(rb_tree tree, FILE *fp);

/* Seciont 5: General helper routines */
/* Returns a node with the given key. */
static rb_node rb_get_node_by_key(rb_tree haystack, int needle);
/* Rotates a tree around the given root. */
static rb_node rb_min(rb_tree tree, rb_node node);
/* Computes height of the tree rooted at node n. */
static int rb_height(rb_tree tre, rb_node n);

/* Section 6: SVG */
#define RADIUS 15.0 /* Radius of each node */
#define PADDING 10.0 /* padding between nodes */
#define MAXWIDTH 1000 /* Maximum width of an image in px */
#define IMGBRDER 5 /* Blank space around image */
/* Draws a subtree rooted at a node n */
static void rb_draw_subtree(FILE *fp, rb_tree tree, rb_node n, double x,
                            double y, int h, int rowpos, double factor);
/* Calculates x position of circle exp rows from the top, at position rowpos int
 * its row. factor corrects for an image which would be greater than MAXWIDTH. */
 static double calcpos(int exp, int rowpos, double factor);


#endif // RBTREE_PRIV_H
