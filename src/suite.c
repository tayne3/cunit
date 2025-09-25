#include <setjmp.h>

#include "cunit.h"
#include "init.h"
#include "once.h"

// Represents a single test case.
struct cunit_test {
	const char        *name;  // The name of the test.
	cunit_test_func_t  func;  // A pointer to the test function.
	struct cunit_test *next;  // A pointer to the next test in the suite.
};

// Represents a test suite, which is a collection of tests.
struct cunit_suite {
	const char           *name;          // The name of the test suite.
	cunit_setup_func_t    setup;         // A pointer to the setup function for the suite.
	cunit_teardown_func_t teardown;      // A pointer to the teardown function for the suite.
	cunit_test_t         *tests;         // A pointer to the first test in the suite.
	cunit_test_t         *last_test;     // A pointer to the last test in the suite.
	struct cunit_suite   *next;          // A pointer to the next test suite.
	int                   test_count;    // The number of tests in the suite.
	int                   passed_count;  // The number of passed tests in the suite.
	int                   failed_count;  // The number of failed tests in the suite.
};

// Represents the global registry for all test suites and test results.
typedef struct {
	cunit_suite_t     *suites;          // A pointer to the first test suite.
	cunit_suite_t     *current_suite;   // A pointer to the current test suite being added to.
	cunit_suite_t     *last_suite;      // A pointer to the last test suite in the list.
	int                total_tests;     // The total number of tests across all suites.
	int                total_passed;    // The total number of passed tests across all suites.
	int                total_failed;    // The total number of failed tests across all suites.
	cunit_error_mode_t error_mode;      // The error handling mode.
	bool               is_initialized;  // A flag indicating whether the registry has been initialized.
	bool               test_running;    // A flag indicating whether a test is currently running.
	bool               test_failed;     // A flag indicating whether the current test has failed.
	jmp_buf            test_jmp_buf;    // Jump buffer for early test exit in COLLECT mode.
} cunit_registry_t;

// Initializes a cunit_registry_t struct with default values.
#define CUNIT_REGISTRY_INIT                         \
	{                                               \
		.suites         = NULL,                     \
		.current_suite  = NULL,                     \
		.last_suite     = NULL,                     \
		.total_tests    = 0,                        \
		.total_passed   = 0,                        \
		.total_failed   = 0,                        \
		.error_mode     = CUNIT_ERROR_MODE_COLLECT, \
		.is_initialized = false,                    \
		.test_running   = false,                    \
		.test_failed    = false,                    \
	}

// The global instance of the test registry.
static cunit_registry_t cunit__registry = CUNIT_REGISTRY_INIT;

// Runs a single test case.
static void cunit__run_test(cunit_suite_t *suite, cunit_test_t *test) {
	cunit__registry.test_failed = false;

	if (suite->setup) { suite->setup(); }

	// Use setjmp/longjmp for early exit in COLLECT mode
	if (cunit__registry.error_mode == CUNIT_ERROR_MODE_COLLECT) {
		if (setjmp(cunit__registry.test_jmp_buf) == 0) {
			// First time through - run the test
			test->func();
		}
		// If longjmp was called, we jump here and skip the rest of the test
	} else {
		// In FAIL_FAST mode, run normally (will exit on first failure)
		test->func();
	}

	if (suite->teardown) { suite->teardown(); }

	if (cunit__registry.test_failed) {
		suite->failed_count++;
		cunit__registry.total_failed++;
		printf("[ \033[31mFAILED\033[0m ] %s\n", test->name);
	} else {
		suite->passed_count++;
		cunit__registry.total_passed++;
		printf("[ \033[32mPASSED\033[0m ] %s\n", test->name);
	}
}

// Marks the current test as failed.
static inline void cunit__mark_failed(void) { cunit__registry.test_failed = true; }

// Prints the header for a test suite.
static inline void cunit__print_header(const char *suite_name) { printf("\n\033[33mRunning test suite: %s\033[0m\n", suite_name); }

// Prints the summary for a test suite.
static void cunit__print_summary(cunit_suite_t *suite) {
	printf("\033[33mSuite Summary: %d passed, %d failed, %d total\033[0m\n", suite->passed_count, suite->failed_count, suite->test_count);
}

// Prints the final summary of all test results.
static inline void cunit__print_final(void) {
	printf("\n\033[33mFinal Summary: %d passed, %d failed, %d total\033[0m\n", cunit__registry.total_passed, cunit__registry.total_failed,
		   cunit__registry.total_tests);
}

// This function is called when a test passes.
void cunit__handle_pass(const cunit_context_t ctx) {
	if (!cunit__registry.test_running) {
		printf("\033[32;2m%s:%d\033[0m ", __cunit_relative(ctx.file), ctx.line);
		fputs("test passed!" STR_NEWLINE, stdout);
		exit(EXIT_SUCCESS);
	}
}

// This function is called when a test fails.
void cunit__handle_fail(const cunit_context_t ctx) {
	printf("\033[31;2m%s:%d\033[0m ", __cunit_relative(ctx.file), ctx.line);
	fputs("test failed!" STR_NEWLINE, stdout);
	if (!cunit__registry.test_running) { exit(EXIT_FAILURE); }
	cunit__mark_failed();
	if (cunit__registry.error_mode == CUNIT_ERROR_MODE_FAIL_FAST) {
		printf("[ \033[31mFAILED\033[0m ] Stopping on first failure\n");
		exit(EXIT_FAILURE);
	} else if (cunit__registry.error_mode == CUNIT_ERROR_MODE_COLLECT) {
		// In COLLECT mode, jump back to the test runner to skip the rest of the test
		longjmp(cunit__registry.test_jmp_buf, 1);
	}
}

// Initializes the cunit framework.
void cunit__internal_init(void) {
	if (cunit__registry.is_initialized) { return; }
	cunit__internal_relative_init();
	cunit__registry.error_mode     = CUNIT_ERROR_MODE_COLLECT;
	cunit__registry.is_initialized = true;
}

// Initializes the cunit framework using a once-only mechanism.
void cunit_init(void) {
	static cunit_once_flag_t initialization_flag = CUNIT_ONCE_FLAG_INIT;
	cunit_call_once(&initialization_flag, cunit__internal_init);
}

// Cleans up all resources used by cunit.
void cunit_cleanup(void) {
	cunit_suite_t *suite = cunit__registry.suites;
	while (suite) {
		cunit_test_t *test = suite->tests;
		while (test) {
			cunit_test_t *next_test = test->next;
			free(test);
			test = next_test;
		}
		cunit_suite_t *next_suite = suite->next;
		free(suite);
		suite = next_suite;
	}
	memset(&cunit__registry, 0, sizeof(cunit_registry_t));
}

// Adds a new test suite to the registry.
void cunit_suite(const char *name, cunit_setup_func_t setup, cunit_teardown_func_t teardown) {
	if (!cunit__registry.is_initialized) { cunit_init(); }

	cunit_suite_t *suite = (cunit_suite_t *)calloc(1, sizeof(cunit_suite_t));
	if (!suite) { return; }

	suite->name     = name;
	suite->setup    = setup;
	suite->teardown = teardown;

	if (!cunit__registry.suites) {
		cunit__registry.suites = suite;
	} else {
		cunit__registry.last_suite->next = suite;
	}
	cunit__registry.last_suite    = suite;
	cunit__registry.current_suite = suite;
}

// Adds a new test to the current test suite.
void cunit_test(const char *name, cunit_test_func_t test_func) {
	if (!cunit__registry.current_suite) { return; }

	cunit_test_t *test = (cunit_test_t *)calloc(1, sizeof(cunit_test_t));
	if (!test) { return; }

	test->name = name;
	test->func = test_func;

	cunit_suite_t *current_suite = cunit__registry.current_suite;
	if (!current_suite->tests) {
		current_suite->tests = test;
	} else {
		current_suite->last_test->next = test;
	}
	current_suite->last_test = test;
	current_suite->test_count++;
	cunit__registry.total_tests++;
}

// Runs all test suites.
int cunit_run(void) {
	cunit__registry.test_running = true;

	cunit_suite_t *suite = cunit__registry.suites;
	while (suite) {
		cunit__print_header(suite->name);
		cunit_test_t *test = suite->tests;
		while (test) {
			cunit__run_test(suite, test);
			test = test->next;
		}
		cunit__print_summary(suite);
		suite = suite->next;
	}

	cunit__print_final();

	const int failed_count = cunit__registry.total_failed;
	cunit_cleanup();
	return failed_count;
}

// Runs a specific test suite.
int cunit_run_suite(const char *suite_name) {
	cunit_suite_t *suite = cunit__registry.suites;
	while (suite) {
		if (strcmp(suite->name, suite_name) == 0) {
			cunit__registry.test_running = true;
			cunit__print_header(suite->name);
			cunit_test_t *test = suite->tests;
			while (test) {
				cunit__run_test(suite, test);
				test = test->next;
			}
			cunit__print_summary(suite);
			cunit__registry.test_running = false;
			return suite->failed_count;
		}
		suite = suite->next;
	}
	return -1;  // Suite not found
}

// Sets the error handling mode.
void cunit_set_error_mode(cunit_error_mode_t mode) { cunit__registry.error_mode = mode; }

// Gets the total number of tests.
int cunit_test_count(void) { return cunit__registry.total_tests; }

// Gets the total number of failed tests.
int cunit_failure_count(void) { return cunit__registry.total_failed; }

// Gets the total number of test suites.
int cunit_suite_count(void) {
	int            count = 0;
	cunit_suite_t *suite = cunit__registry.suites;
	while (suite) {
		count++;
		suite = suite->next;
	}
	return count;
}
