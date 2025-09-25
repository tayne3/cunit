#include "cunit.h"

static int one_count = 0;
static int two_count = 0;

void test_one(void) {
	assert_true(false);
	assert_true(true);
	++one_count;
}

void test_two(void) {
	assert_true(true);
	++two_count;
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Collect Mode Tests", NULL, NULL)
	CUNIT_TEST("Test One", test_one)
	CUNIT_TEST("Test Two", test_two)
	CUNIT_TEST("Test One", test_one)
	CUNIT_TEST("Test Two", test_two)
	CUNIT_SUITE_END()

	const int failed_count = cunit_run();
	if (failed_count != 2) { return -1; }
	if (one_count != 0) { return -1; }
	if (two_count != 2) { return -1; }
	return 0;
}
