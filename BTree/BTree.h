#ifndef MODULE_BTREE_H
#define MODULE_BTREE_H

#include <stdlib.h>

typedef void* BTree_Key;

typedef struct btree_node *BTree_Node;

typedef struct btree *BTree;

/* 
    Função que compara duas chaves
*/
typedef int (*function_compare)(BTree_Key a, BTree_Key b);

/*
    Função que libera a memória de uma chave
*/
typedef void (*function_free)(BTree_Key a);

/*
    Função que imprime o valor de uma chave
*/
typedef void (*function_key_print)(BTree_Key a);


/*
    Cria uma árvore B
    `order` representa o grau máximo da árvore, ou seja, a quantidade de filhos que um nó pode ter. Seu valor deve ser maior ou igual a três
    `f_comp` representa uma função de comparação de duas chaves `a` e `b`. Seu valor deve ser < 0 caso a < b, 0 caso a == b, e > 0 caso a > b
    `f_free` representa uma função de liberação de memória. Chamada quando a árvore ainda contém elementos ao ser destruída. Pode ser NULL
    `f_print` representa uma função de impressão de chave. Chamada nas funções de impressão da árvore
*/
BTree btree_create(int order, function_compare f_comp, function_free f_free, function_key_print f_print);

/* 
    Insere uma chave na árvore `btree`
    Caso `btree` seja inválido, retorna -1
    Retorna 1 caso inserido com sucesso, 0 caso elemento já esteja presente na árvore
*/
int btree_insert(BTree btree, BTree_Key elem);

/* 
    Retorna a chave armazenada em `btree` que seja comparável a `elem`
    Retorna NULL caso nenhum elemento da árvore seja comparável
*/
BTree_Key btree_get(BTree btree, BTree_Key elem);

/*
    Remove uma chave armazenada na árvore `betree`
    Retorna a chave armazenada, sem liberar a memória
    Caso árvore seja inválida ou chave não seja encontrada na árvore, retorna NULL
*/
BTree_Key btree_remove(BTree btree, BTree_Key elem);

/*
    Encontra o nó que armazena a chave `elem` em `btree`
    Caso chave não esteja presente na árvore, retorna o nó no qual ela seria inserida
*/
BTree_Node btree_search(BTree btree, BTree_Key elem);

/*
    Imprime um nó individual da árvore `btree`, utilizando a sua função `f_print`
*/
void btree_print_node(BTree btree, BTree_Node node);

/*
    Imprime a árvore `btree`, utilizando a sua função `f_print`
    Nós são impressos em ordem de altura, da esquerda para a direita
*/
void btree_print_breadth(BTree btree);

/*
    Imprime a árvore `btree`, utilizando a sua função `f_print`
    Elementos são impressos in-order, ou seja, de forma ordenada
*/
void btree_print_depth_inorder(BTree btree);

/*
    Imprime a árvore `btree`, utilizando a sua função `f_print`
    Elementos são impressos em preorder
*/
void btree_print_depth_preorder(BTree btree);

/*
    Destrói a árvore apontada por `btree`, e atualiza seu valor na função chamadora para NULL
    Caso árvore ainda contenha elementos, a memória é liberada automaticamente por meio da função `f_free`
*/
void btree_destroy(BTree* btree);

#endif