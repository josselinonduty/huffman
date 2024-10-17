#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "huffman/huffman.h"
#include "huffman/statistics.h"
#include "statistics_test.h"

void test_frequencies_instance(void)
{
	frequency_table_t table = NULL;

	frequencies_create(&table);
	CU_ASSERT_PTR_NOT_NULL(table);

	frequencies_destroy(&table);
	CU_ASSERT_PTR_NULL(table);
}

void test_frequencies_get_set(void)
{
	frequency_table_t table = NULL;

	symbol_t symbol_1 = 42;
	symbol_t symbol_2 = 150;

	frequencies_create(&table);

	frequencies_increment(table, symbol_1);
	CU_ASSERT_EQUAL(frequencies_get(table, symbol_1), 1);

	frequencies_increment(table, symbol_1);
	CU_ASSERT_EQUAL(frequencies_get(table, symbol_1), 2);

	frequencies_increment(table, symbol_2);
	CU_ASSERT_EQUAL(frequencies_get(table, symbol_2), 1);

	frequencies_destroy(&table);
}

void test_statistic_instance(void)
{
	statistic_t statistic_value = {
		.symbol = 42,
		.count = 100
	};

	statistic_t *statistic = statistic_copy(&statistic_value);
	CU_ASSERT_PTR_NOT_NULL(statistic);

	statistic_free(statistic);
}

void test_statistic_copy(void)
{
	statistic_t statistic = {
		.symbol = 42,
		.count = 100
	};

	statistic_t *copy = (statistic_t *) statistic_copy(&statistic);
	CU_ASSERT_PTR_NOT_NULL(copy);
	CU_ASSERT_EQUAL(copy->symbol, 42);
	CU_ASSERT_EQUAL(copy->count, 100);

	statistic_free(copy);
}

void test_statistic_compare(void)
{
	CU_ASSERT_TRUE(statistic_leq(&(statistic_t) {
				     .symbol = 42,.count = 100}, &(statistic_t) {
				     .symbol = 42,.count = 100}));

	CU_ASSERT_TRUE(statistic_leq(&(statistic_t) {
				     .symbol = 42,.count = 100}, &(statistic_t) {
				     .symbol = 42,.count = 101}));

	CU_ASSERT_FALSE(statistic_leq(&(statistic_t) {
				      .symbol = 42,.count = 100}, &(statistic_t) {
				      .symbol = 42,.count = 99}));

	CU_ASSERT_TRUE(statistic_leq(&(statistic_t) {
				     .symbol = 42,.count = 100}, &(statistic_t) {
				     .symbol = 43,.count = 100}));

	CU_ASSERT_FALSE(statistic_leq(&(statistic_t) {
				      .symbol = 42,.count = 100}, &(statistic_t) {
				      .symbol = 41,.count = 100}));

	CU_ASSERT_TRUE(statistic_leq(&(statistic_t) {
				     .symbol = 42,.count = 100}, &(statistic_t) {
				     .symbol = 43,.count = 101}));

	CU_ASSERT_FALSE(statistic_leq(&(statistic_t) {
				      .symbol = 42,.count = 100}, &(statistic_t) {
				      .symbol = 41,.count = 99}));
}
