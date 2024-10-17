#ifndef ENCODING_TABLE_H
#define ENCODING_TABLE_H

#include <stdbool.h>

#include "huffman/huffman.h"

typedef unsigned char bit;
typedef struct binary_code {
    unsigned char *code;
    unsigned char length;
} binary_code;
typedef binary_code encoding_t;
typedef binary_code *encoding_table_t;

void encoding_table_destroy(encoding_table_t table);

binary_code binary_code_create(void);
void binary_code_destroy(binary_code *code);
void binary_code_set(binary_code *code, int index, bit b);
bit binary_code_get(binary_code code, int index);
int binary_code_length(binary_code code);

void binary_code_print(binary_code code);
binary_code binary_code_copy(binary_code code);
void binary_code_free(binary_code *code);
bool binary_code_compare(binary_code a, binary_code b);

#endif