#include <stdio.h>
#include <stdlib.h>

#include "BTree/BTree.h"

int* create_elem(int a) {
    int* elem = malloc(sizeof(int));
    *elem = a;

    return elem;
}

int compare(void* a, void* b) {
    int int_a = *((int*) a);
    int int_b = *((int*) b);

    return int_a < int_b ? -1 : int_a > int_b;
}

void print(void* a) {
    printf(" [%d] ", *(int*) a);
}

int main() {
    BTree tree = btree_create(4, compare, free, print);
    printf("%d\n", btree_insert(tree, create_elem(9)));
    printf("%d\n", btree_insert(tree, create_elem(1)));
    printf("%d\n", btree_insert(tree, create_elem(7)));
    printf("%d\n", btree_insert(tree, create_elem(6)));
    printf("%d\n", btree_insert(tree, create_elem(5)));
    printf("%d\n", btree_insert(tree, create_elem(8)));
    printf("%d\n", btree_insert(tree, create_elem(10)));
    printf("%d\n", btree_insert(tree, create_elem(11)));
    printf("%d\n", btree_insert(tree, create_elem(4)));
    printf("%d\n", btree_insert(tree, create_elem(3)));
    printf("%d\n", btree_insert(tree, create_elem(12)));
    printf("%d\n", btree_insert(tree, create_elem(13)));
    printf("%d\n", btree_insert(tree, create_elem(14)));
    printf("%d\n", btree_insert(tree, create_elem(15)));
    printf("%d\n", btree_insert(tree, create_elem(16)));
    
    btree_print_inorder(tree);

    return 0;
}