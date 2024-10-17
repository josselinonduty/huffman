#include <stdbool.h>
#include <stdio.h>

#include "base/generic.h"
#include "datatypes/binary_tree.h"
#include "huffman/huffman_tree.h"
#include "huffman/statistics.h"

huffman_tree_t huffman_tree_create(any value)
{
	return binary_tree_create(value, statistic_copy);
}

void huffman_tree_destroy(huffman_tree_t *tree)
{
	binary_tree_destroy(tree, statistic_free);
}

statistic_t *huffman_tree_get_data(huffman_tree_t tree)
{
	return (statistic_t *) binary_tree_get_data(tree);
}

huffman_tree_t huffman_tree_get_left(huffman_tree_t tree)
{
	return binary_tree_get_left(tree);
}

huffman_tree_t huffman_tree_get_right(huffman_tree_t tree)
{
	return binary_tree_get_right(tree);
}

void huffman_tree_set_data(huffman_tree_t tree, any value)
{
	binary_tree_set_data(tree, value, statistic_copy);
}

void huffman_tree_set_left(huffman_tree_t tree, huffman_tree_t left)
{
	binary_tree_set_left(tree, left);
}

void huffman_tree_set_right(huffman_tree_t tree, huffman_tree_t right)
{
	binary_tree_set_right(tree, right);
}

any huffman_tree_copy(any t)
{
	huffman_tree_t *tree = t;

	statistic_t *root = huffman_tree_get_data(*tree);

	huffman_tree_t *copy = malloc(sizeof(huffman_tree_t));
	*copy = huffman_tree_create(root);
	huffman_tree_set_left(*copy, huffman_tree_get_left(*tree));
	huffman_tree_set_right(*copy, huffman_tree_get_right(*tree));

	return copy;
}

bool huffman_tree_compare(any a, any b)
{
	huffman_tree_t *tree_a = a;
	huffman_tree_t *tree_b = b;

	statistic_t *s_a = huffman_tree_get_data(*tree_a);
	statistic_t *s_b = huffman_tree_get_data(*tree_b);
	if (binary_tree_is_leaf(*tree_a) && binary_tree_is_leaf(*tree_b)) {
		if (s_a->count == s_b->count) {
			return s_a->symbol < s_b->symbol;
		}

		return s_a->count < s_b->count;
	}
	return s_a->count <= s_b->count;
}

void huffman_tree_free(any tree)
{
	huffman_tree_destroy((huffman_tree_t *) tree);
	free(tree);
}

static int huffman_tree_print_indent = 0;
void huffman_tree_print(any tree)
{
	printf("|");
	for (int i = 0; i < huffman_tree_print_indent; i++) {
		printf("-");
	}
	printf(" ");

	huffman_tree_t *huffman_tree = tree;
	statistic_t *statistic = huffman_tree_get_data(*huffman_tree);

	if (binary_tree_is_leaf(*huffman_tree)) {

		printf("(%d): %ld\n", statistic->symbol, statistic->count);
		return;
	} else {
		printf("#: %ld\n", statistic->count);
	}

	huffman_tree_print_indent++;

	huffman_tree_t left = huffman_tree_get_left(*huffman_tree);
	if (left != NULL) {
		huffman_tree_print(&left);
	}

	huffman_tree_t right = huffman_tree_get_right(*huffman_tree);
	if (right != NULL) {
		huffman_tree_print(&right);
	}

	huffman_tree_print_indent--;
}
