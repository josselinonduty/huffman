#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <stdbool.h>

#include "base/generic.h"
#include "datatypes/binary_tree.h"
#include "huffman/statistics.h"

typedef binary_tree huffman_tree_t;

huffman_tree_t huffman_tree_create(any value);
void huffman_tree_destroy(huffman_tree_t *tree);
statistic_t *huffman_tree_get_data(huffman_tree_t tree);
huffman_tree_t huffman_tree_get_left(huffman_tree_t tree);
huffman_tree_t huffman_tree_get_right(huffman_tree_t tree);
void huffman_tree_set_data(huffman_tree_t tree, any value);
void huffman_tree_set_left(huffman_tree_t tree, huffman_tree_t left);
void huffman_tree_set_right(huffman_tree_t tree, huffman_tree_t right);

any huffman_tree_copy(any t);
bool huffman_tree_compare(any a, any b);
void huffman_tree_free(any tree);
void huffman_tree_print(any tree);

#endif