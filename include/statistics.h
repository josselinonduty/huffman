#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdbool.h>

#include "base/generic.h"
#include "shared.h"

typedef unsigned long frequency_t;
typedef frequency_t *frequency_table_t;

int frequencies_create(frequency_table_t *table);
int frequencies_destroy(frequency_table_t *table);
int frequencies_set(frequency_table_t table, symbol_t symbol, frequency_t frequency);
int frequencies_increment(frequency_table_t table, symbol_t symbol);
frequency_t frequencies_get(frequency_table_t table, symbol_t symbol);

typedef struct statistic_t
{
    symbol_t symbol;
    frequency_t count;
} statistic_t;

any statistic_copy(any statistic);
void statistic_free(any statistic);
bool statistic_leq(any a, any b);
void statistic_print(any statistic);

#endif