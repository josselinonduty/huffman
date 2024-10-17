#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman/encoding_table.h"
#include "huffman/huffman.h"
#include "huffman/huffman_tree.h"
#include "huffman/statistics.h"
#include "types/queue.h"

void usage(const char *progname, const char *subcommand)
{
	if (NULL == subcommand)
		goto default_usage;

	if (strcmp(subcommand, "compress") == 0) {
		fprintf(stderr, "Usage: %s compress <input> [<output>]\n",
			progname);
		goto exit_program;
	}

	if (strcmp(subcommand, "decompress") == 0) {
		fprintf(stderr, "Usage: %s decompress <input> <output>\n",
			progname);
		goto exit_program;
	}

 default_usage:
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  %s compress <input> [<output>]\n", progname);
	fprintf(stderr, "  %s decompress <input> <output>\n", progname);

 exit_program:
	exit(1);
}

void read_file(FILE *file, frequency_table_t table)
{
	int c;
	while ((c = fgetc(file)) != EOF) {
		frequencies_increment(table, c);
	}
}

queue build_queue(frequency_table_t table)
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

void build_encoding_table_recursive(const huffman_tree_t huffman_tree,
				    encoding_table_t encoding_table,
				    encoding_t *encoding)
{
	if (NULL == huffman_tree || binary_tree_is_leaf(huffman_tree)) {
		if (encoding_length(*encoding) < 1) {
			encoding_set(encoding, 0, 1);
		}

		encoding_table[huffman_tree_get_data(huffman_tree)->symbol] =
		    *encoding;
		return;
	}

	encoding_t encodingLeft = encoding_copy(*encoding);
	encoding_t encodingRight = encoding_copy(*encoding);
	encoding_destroy(encoding);

	if (huffman_tree_get_left(huffman_tree) != NULL) {
		encoding_set(&encodingLeft, encoding_length(encodingLeft), 0);
		build_encoding_table_recursive(huffman_tree_get_left
					       (huffman_tree), encoding_table,
					       &encodingLeft);
	}

	if (huffman_tree_get_right(huffman_tree) != NULL) {
		encoding_set(&encodingRight, encoding_length(encodingRight), 1);
		build_encoding_table_recursive(huffman_tree_get_right
					       (huffman_tree), encoding_table,
					       &encodingRight);
	}
}

int build_encoding_table(const huffman_tree_t huffman_tree,
			 encoding_table_t table)
{
	encoding_t encoding = encoding_create();
	build_encoding_table_recursive(huffman_tree, table, &encoding);

	return 0;
}

void write_file(const char *filename, char *output_filename,
		frequency_table_t frequency_table,
		encoding_table_t encoding_table)
{
	int has_output_filename = 1;
	if (NULL == output_filename) {
		has_output_filename = 0;
		output_filename = malloc(strlen(filename) + 6);
		strcpy(output_filename, filename);
		strcat(output_filename, ".huff");
		output_filename[strlen(filename) + 5] = '\0';
	}

	FILE *input = fopen(filename, "r");
	FILE *output = fopen(output_filename, "w");

	int c;

	char buffer = 0;
	int buffer_length = 0;

	// "HUFF" magic number
	fwrite("HUFF", sizeof(char), 4, output);
	// Length of original file
	fseek(input, 0, SEEK_END);
	long file_length = ftell(input);
	fwrite(&file_length, sizeof(file_length), 1, output);
	fseek(input, 0, SEEK_SET);
	// Number of symbols
	int symbols = 0;
	for (int i = 0; i < 256; i++) {
		if (0 != frequency_table[i]) {
			symbols++;
		}
	}
	fwrite(&symbols, sizeof(symbols), 1, output);
	// Frequencies
	for (int i = 0; i < 256; i++) {
		if (0 != frequency_table[i]) {
			fwrite(&i, sizeof(char), 1, output);
			fwrite(&frequency_table[i], sizeof(frequency_table[i]),
			       1, output);
		}
	}

	while ((c = fgetc(input)) != EOF) {
		encoding_t encoding = encoding_table[c];

		for (int i = 0; i < encoding_length(encoding); i++) {
			bit b = encoding_get(encoding, i);
			buffer = (buffer << 1) | b;
			buffer_length++;

			if (buffer_length == 8) {
				fwrite(&buffer, sizeof(buffer), 1, output);
				buffer = 0;
				buffer_length = 0;
			}
		}
	}

	if (buffer_length > 0) {
		buffer = buffer << (8 - buffer_length);
		fwrite(&buffer, sizeof(char), 1, output);
	}

	fclose(input);
	fclose(output);

	if (0 == has_output_filename)
		free(output_filename);
}

int compress(const char *filename, char *output_filename)
{
	frequency_table_t frequency_table;
	frequencies_create(&frequency_table);

	FILE *file = fopen(filename, "r");
	if (NULL == file)
		return -1;

	read_file(file, frequency_table);
	if (0 != fclose(file))
		return -1;

	queue queue = build_queue(frequency_table);

	huffman_tree_t *huffman_tree = build_huffman_tree(&queue);
	queue_destroy(&queue);

	encoding_t encoding_table[256] = { 0 };
	build_encoding_table(*huffman_tree, encoding_table);
	huffman_tree_free(huffman_tree);

	write_file(filename, output_filename, frequency_table, encoding_table);
	frequencies_destroy(&frequency_table);

	encoding_table_destroy(encoding_table);

	return 0;
}

int decompress(const char *filename, char *output_filename)
{
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		usage(argv[0], NULL);

	if (strcmp(argv[1], "compress") == 0) {
		if (argc < 3)
			usage(argv[0], "compress");

		compress(argv[2], argv[3]);
	} else if (strcmp(argv[1], "decompress") == 0) {
		if (argc < 4)
			usage(argv[0], "decompress");

		decompress(argv[2], argv[3]);
	} else
		usage(argv[0], NULL);

	return 0;
}
