#include <stdio.h>
#include <stdlib.h>

#include "huffman/huffman.h"
#include "huffman/statistics.h"

int frequencies_create(frequency_table_t *table)
{
	*table = (frequency_table_t) calloc(256, sizeof(frequency_t));

	// GCOV_EXCL_START
	if (NULL == *table)
		return -1;
	// GCOV_EXCL_STOP

	return 0;
}

int frequencies_destroy(frequency_table_t *table)
{
	if (NULL == *table)
		return 0;

	free(*table);
	*table = NULL;

	return 0;
}

int frequencies_set(frequency_table_t table, symbol_t symbol,
		    frequency_t frequency)
{
	table[symbol] = frequency;
	return 0;
}

int frequencies_increment(frequency_table_t table, symbol_t symbol)
{
	table[symbol]++;
	return 0;
}

frequency_t frequencies_get(frequency_table_t table, symbol_t symbol)
{
	return table[symbol];
}

any statistic_copy(any statistic)
{
	statistic_t *copy = (statistic_t *) malloc(sizeof(statistic_t));
	if (NULL == copy) {
		return NULL;
	}

	copy->symbol = ((statistic_t *) statistic)->symbol;
	copy->count = ((statistic_t *) statistic)->count;
	return copy;
}

void statistic_free(any statistic)
{
	free(statistic);
}

bool statistic_leq(any a, any b)
{
	if (((statistic_t *) a)->count == ((statistic_t *) b)->count) {
		return ((statistic_t *) a)->symbol <=
		    ((statistic_t *) b)->symbol;
	}

	return ((statistic_t *) a)->count <= ((statistic_t *) b)->count;
}

void statistic_print(any statistic)
{
	printf("(%c: %lu)", ((statistic_t *) statistic)->symbol,
	       ((statistic_t *) statistic)->count);
}
