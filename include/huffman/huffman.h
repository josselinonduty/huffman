#ifndef HUFFMAN_H
#define HUFFMAN_H

#define HUFFMAN_MAGIC "HUFF"
#define HUFFMAN_MAGIC_SIZE 4
#define HUFFMAN_FILE_EXTENSION ".huff"
#define HUFFMAN_FILE_EXTENSION_SIZE 5

#define __huffman_file_length_t long unsigned int
#define HUFFMAN_FILE_LENGTH_SIZE sizeof(__huffman_file_length_t)
#define __huffman_symbol_count_t unsigned char
#define HUFFMAN_SYMBOL_COUNT_SIZE sizeof(__huffman_symbol_count_t)
#define __huffman_symbol_t unsigned char
typedef __huffman_symbol_t symbol_t;
#define HUFFMAN_SYMBOL_SIZE sizeof(__huffman_symbol_t)
#define __huffman_frequency_t long unsigned int
typedef __huffman_frequency_t frequency_t;
#define HUFFMAN_FREQUENCY_SIZE sizeof(__huffman_frequency_t)
#define __huffman_code_t unsigned char
// typedef __huffman_code_t encoding_t;
#define HUFFMAN_CODE_SIZE sizeof(__huffman_code_t)

#define HUFFMAN_MAX_SYMBOLS 256

#endif