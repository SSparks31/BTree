#include "BTree.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    REBALANCE = 0,
    CASE1,
    CASE2,
    CASE3
} cases;

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

void btree_destroy_node(BTree_Node node);

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
    BTree_Key elem = target->keys[0];
    
    for (i = 0; i < node->key_n && btree->f_comp(node->keys[i], elem) < 0; i++);
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
    
    return 0;
}

int btree_insert(BTree btree, BTree_Key elem) {
    if (!btree) {
        return -1;
    }

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

BTree_Key btree_get(BTree btree, BTree_Key elem) {
    if (!btree) {
        return NULL;
    }

    BTree_Node node = btree_search(btree, elem);
    for (int i = 0; i < node->key_n; i++) {
        if (btree->f_comp(elem, node->keys[i]) == 0) {
            return node->keys[i];
        }
    }

    return NULL;
}

BTree_Key btree_remove(BTree btree, BTree_Key elem) {
    if (!btree) {
        return NULL;
    }

    BTree_Key key = btree_get(btree, elem);
    if (!key) {
        return NULL;
    }

    int min_keys = (btree->order/2 + btree->order%2 - 1);

    BTree_Node parent = NULL;
    BTree_Node node = btree->root;
    BTree_Node target = btree_search(btree, elem);
    
    int parent_idx;
    while (node != target) {
        parent = node;
        
        for (parent_idx = 0; parent_idx < node->key_n && btree->f_comp(node->keys[parent_idx], elem) < 0; parent_idx++);
        node = node->children[parent_idx];
    }

    int key_idx = 0;
    while (btree->f_comp(elem, node->keys[key_idx]) != 0) {
        key_idx++;
    }

    if (node->is_leaf) {
        if (node->key_n > min_keys || node == btree->root) {
            for (int i = key_idx; i < node->key_n - 1; i++) {
                node->keys[i] = node->keys[i+1];
            }
            node->key_n--;

            return key;
        } else {    
            BTree_Node left_sibling = parent_idx > 0 ? parent->children[parent_idx - 1] : NULL;
            BTree_Node right_sibling = parent_idx < parent->key_n ? parent->children[parent_idx + 1] : NULL;

            if (left_sibling && left_sibling->key_n > min_keys) {
                node->key_n--;
                btree_insert_nonfull(btree, node, parent->keys[parent_idx - 1]);
                parent->keys[parent_idx - 1] = btree_remove(btree, left_sibling->keys[left_sibling->key_n - 1]);
            } else if (right_sibling && right_sibling->key_n > min_keys) {
                node->key_n--;
                btree_insert_nonfull(btree, node, parent->keys[parent_idx]);
                parent->keys[parent_idx] = btree_remove(btree, right_sibling->keys[0]);
            } else {
                if (right_sibling) {
                    btree_insert_nonfull(btree, node, parent->keys[parent_idx]);
                    for (int i = 0; i < right_sibling->key_n; i++) {
                        btree_insert_nonfull(btree, node, right_sibling->keys[i]);
                    }

                    for (int i = parent_idx; i < parent->key_n - 1; i++) {
                        parent->keys[i] = parent->keys[i + 1];
                    }
                    for (int i = parent_idx + 1; i <= parent->key_n - 1; i++) {
                        parent->children[i] = parent->children[i + 1];
                    }
                    parent->key_n--;

                    btree_destroy_node(right_sibling);
                } else {
                    btree_insert_nonfull(btree, left_sibling, parent->keys[parent_idx - 1]);
                    for (int i = 0; i < node->key_n; i++) {
                        btree_insert_nonfull(btree, left_sibling, node->keys[i]);
                    }   

                    for (int i = parent_idx - 1; i < parent->key_n - 1; i++) {
                        parent->keys[i] = parent->keys[i + 1];
                    }
                    for (int i = parent_idx; i <= parent->key_n - 1; i++) {
                        parent->children[i] = parent->children[i + 1];
                    }

                    btree_destroy_node(node);
                }

                btree_remove(btree, key);
            }
        } 

    } else {
        BTree_Node prev = node->children[key_idx];
        BTree_Node next = node->children[key_idx + 1];

        if (prev->key_n > min_keys) {
            BTree_Node aux = prev;
            while (!aux->is_leaf) {
                aux = aux->children[aux->key_n];
            }

            node->keys[key_idx] = btree_remove(btree, aux->keys[aux->key_n - 1]);
        } else if (next->key_n > min_keys) {
            BTree_Node aux = next;
            while (!aux->is_leaf) {
                aux = aux->children[0];
            }

            node->keys[key_idx] = btree_remove(btree, aux->keys[0]);
        } else {
            for (int i = 0; i < next->key_n; i++) {
                prev->keys[prev->key_n + i] = next->keys[i];   
            }
            prev->key_n += next->key_n;

            btree_destroy_node(next);

            for (int i = key_idx; i < node->key_n - 1; i++) {
                node->keys[i] = node->keys[i + 1];
            }
            for (int i = key_idx + 1; i <= node->key_n - 1; i++) {
                node->children[i] = node->children[i + 1];
            }

            node->key_n--;
        }
    }

    if (btree->root->key_n <= 0) {
        BTree_Node old_root = btree->root;
        btree->root = btree->root->children[0];
        btree_destroy_node(old_root);
    }

    return key;
}

void btree_print_node(BTree btree, BTree_Node node) {
    printf("Node {");
    for (int i = 0; i < node->key_n; i++) {
        btree->f_print(node->keys[i]);
    }
    printf("} - %s\n", node == btree->root ? "Raiz" : node->is_leaf ? "Folha"  : "Interno");
}

void btree_print_breadth(BTree btree) {
    if (!btree || !btree->f_print) {
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

        btree_print_node(btree, node);
    }

    free(queue);
}

void _btree_print_depth_inorder(BTree btree, BTree_Node node) { 
    if (!node) {
        return;
    }

    for (int i = 0; i < node->key_n; i++) {
        _btree_print_depth_inorder(btree, node->children[i]);

        btree->f_print(node->keys[i]);
    }
    _btree_print_depth_inorder(btree, node->children[node->key_n]);
}

void btree_print_depth_inorder(BTree btree) {
    printf("{");
    _btree_print_depth_inorder(btree, btree->root);
    printf("}\n");
}

void _btree_print_depth_preorder(BTree btree, BTree_Node node) { 
    if (!node) {
        return;
    }

    for (int i = 0; i < node->key_n; i++) {
        btree->f_print(node->keys[i]);
        
        _btree_print_depth_preorder(btree, node->children[i]);
    }
    _btree_print_depth_preorder(btree, node->children[node->key_n]);
}

void btree_print_depth_preorder(BTree btree) {
    printf("{");
    _btree_print_depth_preorder(btree, btree->root);
    printf("}\n");
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

void btree_destroy_node(BTree_Node node) {
    free(node->children);
    free(node->keys);
    free(node);
}

void btree_destroy_elements(BTree btree, BTree_Node node) {
    if (node == NULL) {
        return;
    }

    if (!node->is_leaf) {
        for (int i = 0; i <= node->key_n; i++) {
            btree_destroy_elements(btree, node->children[i]);
        }
    }

    for (int i = 0; i < node->key_n; i++) {
        btree->f_free(node->keys[i]);
    }
    btree_destroy_node(node);
}

void btree_destroy(BTree* btree) {
    if (!btree || !*btree) {
        return;
    }

    btree_destroy_elements((*btree), (*btree)->root);

    free((*btree));

    *btree = NULL;
}