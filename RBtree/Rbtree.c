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
     ret->nil->rchild = ret->nil;
     ret->nil->parent = ret->nil;
     ret->root = ret->nil;
     return ret;
}
/* Frees an entire tree. */
void RBfree(rb_tree tree) {
    rb_free_subtree(tree, tree->root);
    rb_free_node(tree->nil);
    free(tree);
}
/* Helper routine: frees a subtree at specified node. */
static void rb_free_subtree(rb_tree tree, rb_node node) {
    if (node == tree->nil) return; /* we only free tree->nil once */
    rb_free_subtree(tree, node->lchild);
    rb_free_subtree(tree, node->rchild);
    rb_free_node(node);
}
/* Create a new node. */
static rb_node rb_new_node(rb_tree tree, int data) {
    rb_node ret;
    if (rb_mem_pool != NULL) {
        ret = rb_mem_pool;
        rb_mem_pool = ret->parent;
    }
    else {
        if ((ret = malloc(sizeof(*ret))) == NULL) {
            fprintf(stderr, "Error: out of memory.\n");
            return NULL;
        }
    }
    ret->key = data;
    ret->parent = tree->nil;
    ret->lchild = tree->nil;
    ret->rchild = tree->nil;
    ret->color = 'r';
    return ret;
}
/* Frees a node to the memory pool. */
static void rb_free_node(rb_node node) {
    node->parent = rb_mem_pool;
    rb_mem_pool = node;
}
/* Frees entire memory pool to main memory. */
void RBcleanup() {
    while (rb_mem_pool != NULL) {
        rb_node cur = rb_mem_pool;
        rb_mem_pool = cur->parent;
        free(cur);
    }
}



/* ***************************************
 * Section 2: Insertion
 ***************************************/
 /* Inserts an element with specified key into tree. */
int RBinsert(rb_tree tree, int key) {
    /* the node we will create */
    rb_node newnode;
    /* newnode's parent */
    rb_node newparent = tree->nil;
    /* The position into which we will put newnode */
    rb_node pos = tree->root;
    /* Locate the correct position */
    while (pos != tree->nil) {
        newparent = pos;
        if (key < pos->key) {
            pos = pos->lchild;
        }
        else if (key > pos->key) {
            pos = pos->rchild;
        }
        else {
            /* We don't support two nodes with the same value. */
            fprintf(stderr, "Error: node %i already in the tree.\n", key);
            return 0;
        }
    }
    /* Allocate our node */
    newnode = rb_new_node(tree, key);
    if (newnode == NULL) {
        return 0;
    }
    /* Set up the parent node */
    newnode->parent = newparent;
    if(newparent == tree->nil) {
        tree->root = newnode;
    }
    else if(key < newparent->key) {
        newparent->lchild = newnode;
    }
    else {
        newparent->rchild = newnode;
    }
    /* Fix the tree strucure */
    rb_insert_fix(tree, newnode);
    return 1;
}
/* Corrects for properties violated on an insertion. */
static void rb_insert_fix(rb_tree tree, rb_node n) {
    rb_node gp = n->parent->parent,/* grandparent */
        uncle = rb_get_uncle(tree, n);
    while (n->parent->color == 'r' && uncle->color == 'r') {
        gp->color = 'r';
        uncle->color = 'b';
        n->parent->color = 'b';
        n = gp;
        gp = n->parent->parent;
        uncle = rb_get_uncle(tree, n);
    }

    if(n->parent->color == 'b') {
        if (n == tree->root) n->color = 'b';
        return ;
    }

    /* case 2: node is "close to" uncle */
    if ((n->parent->lchild == n) == (gp->lchild == uncle)) {
        rb_node new_n = n->parent;
        rb_rotate(tree, new_n, new_n->rchild==n);
        n = new_n;
    } /* Fall through */

    /*case 3: node is "far from" uncle */
    n->parent->color = 'b';
    gp->color = 'r';
    rb_rotate(tree, gp, gp->lchild == uncle);
    tree->root->color = 'b';
}
/* Helper routine: returns the uncle of a given node. */
static rb_node rb_get_uncle(rb_tree tree, rb_node n) {
    rb_node gp;
    if (n->parent == tree->nil || n->parent->parent == tree->nil) {
        return tree->nil;
    }
    gp = n->parent->parent;
    return (gp->lchild == n->parent)? gp->lchild : gp->rchild;
}


/* ***************************************
 * Section 3: Deletion
 ***************************************/
/* Deletes an element with a particular key. */
int RBdelete(rb_tree tree, int key) {
    /* The node with the actual key */
    rb_node dead = rb_get_node_by_key(tree, key);
    /* The node where we will fix the tree structure */
    rb_node fixit;
    /* Original color of the deleted node */
    char orig_col = dead->color;
    /* Node does not exist, so we cannot delete it */
    if(dead == tree->nil) {
        fprintf(stderr, "Error: node %i does not exist.\n", key);
        return 0;
    }
    /* Here we perform binary tree deletion */
    if (dead->lchild == tree->nil) {
        fixit = dead->rchild;
        rb_transplant(tree, dead, fixit);
    }
    else if (dead->rchild == tree->nil) {
        fixit = dead->lchild;
        rb_transplant(tree, dead, fixit);
    }
    else {
        /* Replace dead with its successor */
        rb_node successor = rb_min(tree, dead->rchild);
        orig_col = successor->color;
        fixit = successor->rchild;
        if(successor->parent == dead) {
            fixit->parent = successor;
        }
        else {
            /*put the successor's right child into its place */
            rb_transplant(tree, successor, successor->rchild);
            successor->rchild = dead->rchild;
            successor->rchild->parent = successor;
        }
        rb_transplant(tree, dead, successor);
        successor->lchild = dead->rchild;
        successor->lchild->parent = successor;
        successor->color = dead->color;
    }
    rb_free_node(dead);
    /* Only need to fix if we deleted a black node */
    if(orig_col == 'b'){
        rb_delete_fix(tree, fixit);
    }
    return 1;
}
/* Helper routine: transplants node 'from' into node 'to's position. */
static void rb_delete_fix(rb_tree tree, rb_node n) {
    /* It's always safe to change the root black, and if we reach a red
     * node, we can fix the tree by changing it black. */
     while (n != tree->root && n->color == 'b') {
        /* Instead of duplicating code, we judge have a flag to test
         * which direction we are dealing with. */
         int is_left = (n == n->parent->lchild);
         rb_node sibling = (is_left)? n->parent->rchild : n->parent->lchild;
         /*case 1: sibing red */
         if(sibling->color == 'r') {
            sibling->color = 'b';
            sibling->parent->color = 'r';
            rb_rotate(tree, sibling->parent, is_left);
            sibling = (is_left)? n->parent->rchild : n->parent->rchild;
         }
         /* cast 2:sibling black, both sibling's children black */
         if(sibling->lchild->color == 'b' && sibling->rchild->color == 'b') {
            sibling->color = 'r';
            n = n->parent;
         }
         else {
            /* case 2: sibling blace,"far" child black */
            if((is_left && sibling->rchild == 'b') ||
            (!is_left && sibling->lchild->color == 'b')) {
                if (is_left) {
                    sibling->lchild->color = 'b';
                }
                else sibling->rchild->color = 'b';
            sibling->color = 'r';
            rb_rotate(tree, sibling, !is_left);
            sibling = (is_left) ? n->parent->rchild : n->parent->lchild;
            }
            /* Fall through*/
            /* Case 4: sibling black, "far" child red */
            sibling->color = n->parent->color;
            n->parent->color = 'b';
            if (is_left) {
                sibling->rchild->color = 'b';
            }
            else sibling->lchild->color = 'b';
            rb_rotate(tree, n->parent, is_left);
            /*We're done, so set n to root node */
            n = tree->root;
         }
     }
     n->color = 'b';
}




/* ***************************************
 * Section 4: I/O
 ***************************************/
/* Write a tree to stdout in preorder format.*/
void RBwrite(rb_tree tree) {
    if(tree->root == tree->nil) {
        fprintf(stderr, "Error: empty tree\n");
        return ;
    }
    /*Special case to account for missing semicolon */

}













