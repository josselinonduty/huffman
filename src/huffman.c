#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman_tree.h"
#include "shared.h"
#include "statistics.h"
#include "types/queue.h"

void usage(const char *progname)
{
	fprintf(stderr, "Usage: %s <compress|decompress> <filename>\n",
		progname);
	exit(1);
}

void read_file(FILE *file, frequency_table_t table)
{
	int c;
	while ((c = fgetc(file)) != EOF) {
		frequencies_increment(table, c);
	}
}

queue convert_frequencies_to_queue(frequency_table_t table)
{
	queue queue =
	    queue_create(FLAG_SORTED, huffman_tree_copy, huffman_tree_free,
			 huffman_tree_compare);

	for (int i = 0; i < 256; i++) {
		statistic_t statistic = {.symbol = i,.count =
			    frequencies_get(table, i)
		};
		if (0 == statistic.count)
			continue;

		huffman_tree_t huffman_tree = huffman_tree_create(&statistic);
		queue_enqueue(&queue, &huffman_tree);
		huffman_tree_destroy(&huffman_tree);
	}

	return queue;
}

huffman_tree_t *build_huffman_tree(queue *queue)
{
	long length = queue_length(*queue);
	while (length >= 2) {
		huffman_tree_t *left = queue_dequeue(queue);
		huffman_tree_t *right = queue_dequeue(queue);

		statistic_t *left_statistic = huffman_tree_get_data(*left);
		statistic_t *right_statistic = huffman_tree_get_data(*right);

		statistic_t statistic = {.symbol = 0,.count =
			    left_statistic->count + right_statistic->count
		};

		huffman_tree_t huffman_tree = huffman_tree_create(&statistic);
		statistic_t *huffman_statistic =
		    huffman_tree_get_data(huffman_tree);
		huffman_tree_set_left(huffman_tree, *left);
		huffman_tree_set_right(huffman_tree, *right);

		queue_enqueue(queue, &huffman_tree);
		free(huffman_tree);
		free(huffman_statistic);
		free(left);
		free(right);
		length--;
	}

	return queue_dequeue(queue);
}

int compress(const char *filename)
{
	frequency_table_t table;
	frequencies_create(&table);

	FILE *file = fopen(filename, "r");
	if (NULL == file)
		return -1;

	read_file(file, table);
	if (0 != fclose(file))
		return -1;

	queue queue = convert_frequencies_to_queue(table);
	frequencies_destroy(&table);

	huffman_tree_t *huffman_tree = build_huffman_tree(&queue);
	queue_destroy(&queue);

	huffman_tree_free(huffman_tree);
	return 0;
}

int decompress(const char *filename)
{
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}

	if (strcmp(argv[1], "compress") == 0) {
		compress(argv[2]);
	} else if (strcmp(argv[1], "decompress") == 0) {
		decompress(argv[2]);
	} else {
		usage(argv[0]);
		exit(1);
	}

	return 0;
}
