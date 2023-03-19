#include "BTree.h"
#include <stdlib.h>


struct btree_node {
    int key_n;
    BTree_Key* keys;

    BTree_Node* children;
    int is_leaf;
};

struct btree {
    int order;
    BTree_Node root;

    function_compare f_comp;
    function_free f_free;
};

BTree btree_create(int order, function_compare f_comp, function_free f_free) {
    if (order < 2 || f_comp == NULL) {
        return NULL;
    }

    BTree btree = malloc(sizeof(struct btree));
    btree->order = 2;
    
    btree->f_comp = f_comp;
    btree->f_free = f_free;
    
    btree->root = malloc(sizeof(struct btree_node));

    btree->root->key_n = 0;
    btree->root->keys = malloc(sizeof(BTree_Node));

    btree->root->children = NULL;
    btree->root->is_leaf = 1;

    return btree;
}

int btree_insert(BTree btree, BTree_Key elem) {
    if (!btree) {
        return -1;
    }

    BTree_Node node = btree->root;
    if (node->key_n == 0) {
        node->keys[0] = elem;
        node->key_n++;
        return 1;
    } else {

    }

    return 1;
}

int btree_remove(BTree btree, BTree_Key elem);
BTree_Node btree_search(BTree btree, BTree_Key elem);

void btree_destroy_elements(BTree btree, BTree_Node node) {
    if (node == NULL) {
        return;
    }

    if (node->is_leaf != 1) {
        for (int i = 0; i < node->key_n + 1; i++) {
            btree_destroy_elements(btree, node->children[i]);
        }
        free(node->children);
    }

    for (int i = 0; i < node->key_n; i++) {
        btree->f_free(node->keys[i]);
    }
    
    free(node->keys);
}

void btree_destroy(BTree* btree) {
    if (!btree || !*btree);
    btree_destroy_elements((*btree), (*btree)->root);

    free((*btree));

    *btree = NULL;
}