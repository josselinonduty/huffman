#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoding_table.h"
#include "shared.h"

void encoding_table_destroy(encoding_table_t table)
{
	for (int i = 0; i < 256; i++) {
		if (table[i] != NULL) {
			binary_code_destroy(table[i]);
		}
	}
}

binary_code binary_code_create()
{
	binary_code code = malloc(16 * sizeof(bit));
	for (int i = 0; i < 16; i++) {
		code[i] = -1;
	}
	return code;
}

void binary_code_destroy(binary_code code)
{
	free(code);
}

void binary_code_set(binary_code code, int index, bit b)
{

	code[index] = (b & 0x1);
}

bit binary_code_get(binary_code code, int index)
{
	return code[index] & 0x1;
}

int binary_code_length(binary_code code)
{
	int pos = 0;
	while (code[pos] != -1 && pos < 16) {
		pos++;
	}
	return pos;
}

void binary_code_print(binary_code code)
{
	for (int i = 0; i < binary_code_length(code); i++) {
		printf("%d", binary_code_get(code, i));
	}
}

binary_code binary_code_copy(binary_code code)
{
	binary_code copy = binary_code_create();

	for (int i = 0; i < binary_code_length(code); i++) {
		binary_code_set(copy, i, binary_code_get(code, i));
	}

	return copy;
}

void binary_code_free(binary_code code)
{
	binary_code_destroy(code);
}

bool binary_code_compare(binary_code a, binary_code b)
{
	if (binary_code_length(a) != binary_code_length(b)) {
		return false;
	}

	for (int i = 0; i < binary_code_length(a); i++) {
		if (binary_code_get(a, i) != binary_code_get(b, i)) {
			return false;
		}
	}

	return true;
}
