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

int main() {
    BTree tree = btree_create(3, compare, free);
    btree_insert(tree, create_elem(5));

    return 0;
}