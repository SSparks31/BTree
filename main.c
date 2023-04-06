#include <stdio.h>
#include <stdlib.h>

#include "BTree/BTree.h"


int compare(void* a, void* b) {
    int int_a = *((int*) a);
    int int_b = *((int*) b);

    return int_a < int_b ? -1 : int_a > int_b;
}

void print(void* a) {
    printf(" [%d] ", *(int*) a);
}

int* create_value(int value) {
    int* ptr = malloc(sizeof(int));
    *ptr = value;

    return ptr;
}

int main() {
    int order;
    printf("Insira a ordem maxima da arvore: ");
    scanf("%d", &order);
    BTree btree = btree_create(order, compare, free, print);
    if (!btree) {
        printf("Erro\n");
        return -1;
    }

    int input;
    do {
        printf("Insira a opcao desejada\n");
        printf("1- Inserir valor\n");
        printf("2- Remover valor\n");
        printf("3- Imprimir arvore - Nodes em ordem de altura\n");
        printf("4- Imprimir arvore - Elementos ordenados\n");
        printf("5- Buscar node contendo elemento\n");
        printf("6- Inserir valores de 1 a 25 na arvore\n");
        printf("Outro valor: Encerrar operacao\n");
        scanf("%d", &input);

        int value;
        BTree_Key key;
        switch (input) {
            case 1:
                printf("Insira a chave a ser inserida: ");
                scanf("%d", &value);

                key = create_value(value);
                if (btree_insert(btree, key) == 1) {
                    printf("Chave inserida com sucesso!\n");
                } else {
                    printf("Chave ja presente na arvore\n");
                }
                break;

            case 2:
                printf("Insira a chave a ser removida: ");
                scanf("%d", &value);
                
                if ((key = btree_remove(btree, &value))) {
                    printf("Chave removida com sucesso\n");
                    free(key);
                } else {
                    printf("Chave nao encontrada na arvore\n");
                }
                
                break;
            case 3:
                btree_print_breadth(btree);
                break;
            
            case 4:
                btree_print_depth_inorder(btree);
                break;

            case 5:
                printf("Insira o valor a ser buscado: ");
                scanf("%d", &value);
                if (btree_get(btree, &value) == NULL) {
                    printf("Elemento nao encontrado na arvore");
                } else {
                    BTree_Node node = btree_search(btree, &value);
                    btree_print_node(btree, node);
                }
                break;

            case 6:
                for (int i = 1; i <= 25; i++) {
                    key = create_value(i);
                    btree_insert(btree, key);
                }
                break;

            default:
                input = 0;
        }
    } while (input != 0);

    btree_destroy(&btree);
    return 0;
}