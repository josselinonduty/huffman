#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman/encoding_table.h"
#include "huffman/huffman.h"

void encoding_table_destroy(encoding_table_t table)
{
	for (int i = 0; i < 256; i++) {
		binary_code_destroy(&(table[i]));
	}
}

binary_code binary_code_create()
{
	binary_code code = {
		.code = malloc(sizeof(unsigned char)),
		.length = 0
	};

	if (code.code == NULL) {
		fprintf(stderr,
			"Failed to allocate memory for binary code: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	return code;
}

void binary_code_destroy(binary_code *code)
{
	free(code->code);
	code->code = NULL;
	code->length = 0;
}

void __binary_code_resize(binary_code *code, int new_size)
{
	if (new_size <= code->length) {
		return;
	}

	code->code = realloc(code->code, new_size * sizeof(unsigned char));

	if (code->code == NULL) {
		fprintf(stderr, "Failed to resize binary code: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void binary_code_set(binary_code *code, int index, bit b)
{
	__binary_code_resize(code, index + 1);

	if (b) {
		code->code[index] |= 0x01;
	} else {
		code->code[index] &= 0xFE;
	}

	code->length = index + 1;
}

bit binary_code_get(binary_code code, int index)
{
	if (index >= code.length) {
		return -1;
	}

	return code.code[index] & 0x01;
}

int binary_code_length(binary_code code)
{
	return code.length;
}

void binary_code_print(binary_code code)
{
	for (int i = 0; i < binary_code_length(code); i++) {
		printf("%d", binary_code_get(code, i));
	}

	printf("\n");
}

binary_code binary_code_copy(binary_code code)
{
	binary_code copy = binary_code_create();

	__binary_code_resize(&copy, binary_code_length(code));

	for (int i = 0; i < binary_code_length(code); i++) {
		binary_code_set(&copy, i, binary_code_get(code, i));
	}

	return copy;
}

void binary_code_free(binary_code *code)
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
