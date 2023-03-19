#include <stdlib.h>

typedef int (*function_compare)(void* a, void* b);
typedef void (*function_free)(void* key);

typedef void* BTree_Key;

typedef struct btree_node *BTree_Node;

typedef struct btree *BTree;

BTree btree_create(int order, function_compare f_comp, function_free f_free);

int btree_insert(BTree btree, BTree_Key elem);
int btree_remove(BTree btree, BTree_Key elem);
BTree_Node btree_search(BTree btree, BTree_Key elem);

void btree_destroy(BTree* btree);