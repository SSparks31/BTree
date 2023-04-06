#ifndef MODULE_BTREE_H
#define MODULE_BTREE_H

#include <stdlib.h>

typedef void* BTree_Key;

typedef struct btree_node *BTree_Node;

typedef struct btree *BTree;

typedef int (*function_compare)(BTree_Key a, BTree_Key b);
typedef void (*function_free)(BTree_Key a);
typedef void (*function_key_print)(BTree_Key a);

typedef void (*function_key)(BTree_Key a, void* data);

BTree btree_create(int order, function_compare f_comp, function_free f_free, function_key_print f_print);

int btree_insert(BTree btree, BTree_Key elem);
BTree_Key btree_get(BTree btree, BTree_Key elem);
BTree_Key btree_remove(BTree btree, BTree_Key elem);
BTree_Node btree_search(BTree btree, BTree_Key elem);

void btree_print_breadth(BTree btree);
void btree_print_depth_inorder(BTree btree);
void btree_print_depth_preorder(BTree btree);
void btree_print_depth_postorder(BTree btree);

void btree_destroy(BTree* btree);

#endif