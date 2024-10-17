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
		encoding_destroy(&(table[i]));
	}
}

encoding_t encoding_create()
{
	encoding_t code = {
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

void encoding_destroy(encoding_t *code)
{
	free(code->code);
	code->code = NULL;
	code->length = 0;
}

void __encoding_resize(encoding_t *code, int new_size)
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

void encoding_set(encoding_t *code, int index, bit b)
{
	__encoding_resize(code, index + 1);

	if (b) {
		code->code[index] |= 0x01;
	} else {
		code->code[index] &= 0xFE;
	}

	code->length = index + 1;
}

bit encoding_get(encoding_t code, int index)
{
	if (index >= code.length) {
		return -1;
	}

	return code.code[index] & 0x01;
}

int encoding_length(encoding_t code)
{
	return code.length;
}

void encoding_print(encoding_t code)
{
	for (int i = 0; i < encoding_length(code); i++) {
		printf("%d", encoding_get(code, i));
	}

	printf("\n");
}

encoding_t encoding_copy(encoding_t code)
{
	encoding_t copy = encoding_create();

	__encoding_resize(&copy, encoding_length(code));

	for (int i = 0; i < encoding_length(code); i++) {
		encoding_set(&copy, i, encoding_get(code, i));
	}

	return copy;
}

void encoding_free(encoding_t *code)
{
	encoding_destroy(code);
}

bool encoding_compare(encoding_t a, encoding_t b)
{
	if (encoding_length(a) != encoding_length(b)) {
		return false;
	}

	for (int i = 0; i < encoding_length(a); i++) {
		if (encoding_get(a, i) != encoding_get(b, i)) {
			return false;
		}
	}

	return true;
}
