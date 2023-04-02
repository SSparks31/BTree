#include "BTree.h"
#include <stdio.h>
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
    function_key_print f_print;
};

BTree_Node btree_create_node(int order, int is_leaf) {
    BTree_Node node = calloc(1, sizeof(struct btree_node));

    node->key_n = 0;
    node->keys = calloc(order, sizeof(BTree_Key));

    node->children = calloc(order + 1, sizeof(struct btree_node));

    node->is_leaf = is_leaf;

    return node;
}

BTree btree_create(int order, function_compare f_comp, function_free f_free, function_key_print f_print) {
    if (order < 3 || f_comp == NULL) {
        return NULL;
    }

    BTree btree = malloc(sizeof(struct btree));
    btree->order = order;
    
    btree->f_comp = f_comp;
    btree->f_free = f_free;
    btree->f_print = f_print;
    
    btree->root = btree_create_node(order, 1);
    
    return btree;
}

int btree_insert_nonfull(BTree btree, BTree_Node node, BTree_Key elem) {
    int i;
    for (i = node->key_n++; i > 0 && btree->f_comp(node->keys[i-1], elem) > 0; i--) {
        node->keys[i] = node->keys[i-1];
    }
    node->keys[i] = elem;
    return 1;
}


void btree_split_root(BTree btree) {
    BTree_Node x = btree_create_node(btree->order, 0);
    BTree_Node y = btree->root;
    BTree_Node z = btree_create_node(btree->order, y->is_leaf);

    int median = btree->order/2 - (1 - btree->order % 2);
    BTree_Key to_move = y->keys[median];

    x->keys[0] = to_move;
    x->children[0] = y;
    x->children[1] = z;
    x->key_n++;

    for (int j = 0; j < y->key_n - median - 1; j++) {
        z->keys[j] = y->keys[j + median + 1];
        z->children[j] = y->children[j + median + 1];
        z->key_n++;
    }

    z->children[z->key_n] = y->children[y->key_n];
    y->key_n = median;

    btree->root = x;
}

int btree_split_child(BTree btree, BTree_Node node, BTree_Node target) {
    if (!node) {
        return 0;
    }

    if (node == target) {
        return 1;
    }

    int i;
    for (i = 0; i <= node->key_n; i++) {
        if (btree_split_child(btree, node->children[i], target) == 1) {
            BTree_Node x = node;
            BTree_Node y = x->children[i];
            BTree_Node z = btree_create_node(btree->order, y->is_leaf);
            
            int median = y->key_n/2 - (1 - y->key_n % 2);
            BTree_Key to_move = y->keys[median];

            int j;
            for (j = x->key_n; j > i; j--) {
                x->keys[j] = x->keys[j - 1];
                x->children[j + 1] = x->children[j];
            }

            x->keys[j] = to_move;
            x->children[j + 1] = z;
            x->key_n++;

            for (j = 0; j < y->key_n - median - 1; j++) {
                z->keys[j] = y->keys[j + median + 1];
                z->children[j] = y->children[j + median + 1];
                z->key_n++;
            }

            z->children[z->key_n] = y->children[y->key_n];
            y->key_n = median;

            if (x->key_n == btree->order) {
                return 1;
            }

            return 0;
        }
    }
    
    return 0;
}

int btree_insert(BTree btree, BTree_Key elem) {
    BTree_Node to_insert = btree_search(btree, elem);
    for (int i = 0; i < to_insert->key_n; i++) {
        if (btree->f_comp(to_insert->keys[i], elem) == 0) {
            return 0;
        }
    }

    if (to_insert->key_n == btree->order - 1) {
        btree_insert_nonfull(btree, to_insert, elem);
        if (btree_split_child(btree, btree->root, to_insert)) {
            btree_split_root(btree);
        };
        return 1;
    } else {
        return btree_insert_nonfull(btree, to_insert, elem);
    }

    return -1;
}

int _btree_remove(BTree btree, BTree_Node node, BTree_Key elem) {
    if (!node) {
        return 0;
    }

}

int btree_remove(BTree btree, BTree_Key elem) {
    
}

void btree_print_inorder(BTree btree) {
    if (!btree->f_print) {
        return;
    }

    BTree_Node* queue = malloc(sizeof(BTree_Node));
    queue[0] = btree->root;
    int n = 1;

    while (n > 0) {
        BTree_Node node = queue[0];
        n--;

        for (int i = 0; i < n; i++) {
            queue[i] = queue[i+1];
        }

        for (int i = 0; i <= node->key_n; i++) {
            if (node->children[i]) {
                queue = realloc(queue, (n + 1) * sizeof(BTree_Node));
                queue[n++] = node->children[i];
            }
        }

        printf("Node {");
        for (int i = 0; i < node->key_n; i++) {
            btree->f_print(node->keys[i]);
        }
        printf("} - %s\n", node->is_leaf ? "Folha" : "Interno");
    }

    // free(queue);
}

BTree_Node btree_search(BTree btree, BTree_Key elem) {
    BTree_Node node = btree->root;
    if (!node) {
        return NULL;
    }

    while (node) {
        BTree_Node new_node;
        for (int i = 0; i <= node->key_n; i++) {
            if (i == node->key_n) {
                new_node = node->children[i];

            } else {
                int comp = btree->f_comp(node->keys[i], elem);
                if (comp == 0) {
                    return node;
                }

                if (comp > 0) {
                    new_node = node->children[i];
                    break;
                }
            }
        }

        if (new_node == NULL) {
            return node;
        } else {
            node = new_node;
        }
    }

    return NULL;
}

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