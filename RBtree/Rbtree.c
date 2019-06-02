#include "Rbtree.h"
#include "RBtree_priv.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>


/* ***************************************
 * Section 1: Creation and Deallocation
 ***************************************/
/* Creates an empty Red_Black tree. */
rb_tree RBcreate() {
    rb_tree ret; /* The tree we are returning */
    if ((ret = malloc(sizeof(*ret))) == NULL ) {
        fprintf(stderr, "Error: out of memory.\n");
        return NULL;
    }
    /* we can't use rb_new_node() because it wants to set some of the values
     * to tree->nil. */
     if ((ret->nil = malloc(sizeof(*ret->nil)))==NULL) {
        fprintf(stderr, "Error: out of memory.\n");
        /* Allocation of ret had been successful; we need to free it. */
        free(ret);
        return NULL;
     }
     ret->nil->color = 'b';
     ret->nil->lchild = ret->nil;

}
