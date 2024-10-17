#ifndef ENCODING_TABLE_H
#define ENCODING_TABLE_H

#include <stdbool.h>

#include "huffman/huffman.h"

typedef unsigned char bit;
typedef struct encoding_t {
    unsigned char *code;
    unsigned char length;
} encoding_t;
typedef encoding_t *encoding_table_t;

void encoding_table_destroy(encoding_table_t table);

encoding_t encoding_create(void);
void encoding_destroy(encoding_t *code);
void encoding_set(encoding_t *code, int index, bit b);
bit encoding_get(encoding_t code, int index);
int encoding_length(encoding_t code);

void encoding_print(encoding_t code);
encoding_t encoding_copy(encoding_t code);
void encoding_free(encoding_t *code);
bool encoding_compare(encoding_t a, encoding_t b);

#endif