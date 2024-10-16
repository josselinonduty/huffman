#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "statistics.h"

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

	frequencies_destroy(&table);

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
