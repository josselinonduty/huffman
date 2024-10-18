#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void write_compressed_file(const char *filename, char *output_filename,
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
	int symbol_count = 0;
	for (int i = 0; i < 256; i++) {
		if (0 != frequency_table[i]) {
			symbol_count++;
		}
	}
	fwrite(&symbol_count, 2 * sizeof(char), 1, output);
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
	clock_t start = clock();

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

	write_compressed_file(filename, output_filename, frequency_table,
			      encoding_table);
	frequencies_destroy(&frequency_table);

	encoding_table_destroy(encoding_table);

	clock_t end = clock();
	double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

	printf("Elapsed time: %.2fs\n", elapsed);
	return 0;
}

int read_compressed_file(FILE *file, long unsigned int *file_length,
			 int *symbol_count, frequency_table_t *frequency_table)
{
	frequencies_create(frequency_table);

	char magic[4];
	if (fread(magic, sizeof(char), 4, file) != 4)
		return -1;
	if (0 != strncmp(magic, "HUFF", 4))
		return -1;

	if (fread(file_length, sizeof(*file_length), 1, file) != 1)
		return -1;

	if (fread(symbol_count, 2 * sizeof(char), 1, file) != 1)
		return -1;

	for (int i = 0; i < *symbol_count; i++) {
		unsigned char symbol = 0;
		long unsigned int frequency = 0;
		if (fread(&symbol, sizeof(symbol), 1, file) != 1)
			return -1;
		if (fread(&frequency, sizeof(frequency), 1, file) != 1)
			return -1;
		frequencies_set(*frequency_table, symbol, frequency);
	}

	return 0;
}

void write_file(FILE *file, FILE *output,
		long unsigned int file_length,
		const huffman_tree_t *huffman_tree)
{
	huffman_tree_t current = *huffman_tree;
	long unsigned int symbols_found = 0;
	int c;
	while (symbols_found < file_length) {
		c = fgetc(file);

		for (int i = 0; i < 8; i++) {
			if (binary_tree_is_leaf(current)) {
				symbols_found++;
				statistic_t *statistic =
				    huffman_tree_get_data(current);
				fputc(statistic->symbol, output);
				if (symbols_found == file_length)
					break;
				current = *huffman_tree;
			}

			bit b = (c >> (7 - i)) & 1;
			if (b == 0)
				current = huffman_tree_get_left(current);
			else
				current = huffman_tree_get_right(current);
		}
	}
}

int decompress(const char *filename, char *output_filename)
{
	clock_t start = clock();

	FILE *input = fopen(filename, "r");
	if (NULL == input)
		return -1;

	long unsigned int file_length = 0;
	int symbol_count = 0;
	frequency_table_t frequency_table = NULL;

	if (0 != read_compressed_file(input, &file_length, &symbol_count,
				      &frequency_table))
		return -1;

	queue queue = build_queue(frequency_table);

	huffman_tree_t *huffman_tree = build_huffman_tree(&queue);
	queue_destroy(&queue);

	FILE *output = fopen(output_filename, "w");

	write_file(input, output, file_length, huffman_tree);

	fclose(input);
	fclose(output);
	frequencies_destroy(&frequency_table);
	huffman_tree_free(huffman_tree);

	clock_t end = clock();
	double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

	printf("Elapsed time: %.2fs\n", elapsed);

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
