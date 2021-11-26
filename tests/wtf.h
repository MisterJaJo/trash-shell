/*
MIT License

Copyright (c) 2021 MisterJaJo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */


#ifndef _WTF_H
#define _WTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define wtf_str(v) #v


///// wtf Main Function /////

#define wtf_main int main()




///// wtf Test results /////

typedef enum {
	WTF_TEST_RESULT_UNDEFINED,

	WTF_TEST_RESULT_SUCCESS,
	WTF_TEST_RESULT_FAILURE
} wtf_test_result_t;

char *wtf_test_result_string(wtf_test_result_t result) {
	if (result == WTF_TEST_RESULT_SUCCESS) {
		return "SUCCESS";
	}
	if (result == WTF_TEST_RESULT_FAILURE) {
		return "FAILURE";
	}
	return "UNKNOWN";
}

void wtf_test_result_write_counter(wtf_test_result_t result, int *success, int *failure) {
	if (result == WTF_TEST_RESULT_SUCCESS) {
		*success += 1;
	}
	if (result == WTF_TEST_RESULT_FAILURE) {
		*failure += 1;
	}
}




///// wtf Tests /////

typedef struct {
	char *name;
	wtf_test_result_t result;

	unsigned int successful_assertions;
	unsigned int failed_assertions_count;

	size_t failed_assertions_cap; 
	char **failed_assertions;
} wtf_test_t;

#define WTF_TEST_ASSERTIONS_CAP_INCREMENT 20

wtf_test_t *wtf_test_new(char *name) {
	wtf_test_t *test = (wtf_test_t *)malloc(sizeof(wtf_test_t));

	test->name = name;
	test->result = WTF_TEST_RESULT_UNDEFINED;
	test->successful_assertions = 0;
	test->failed_assertions_count = 0;

	test->failed_assertions_cap = WTF_TEST_ASSERTIONS_CAP_INCREMENT;
	test->failed_assertions = (char **)malloc(test->failed_assertions_cap * sizeof(char *));
        
	return test;
}

#define wtf_assert(expr)						\
	if (expr) {							\
		wtf_test_suite_current_success(suite);			\
	} else {							\
		wtf_test_suite_current_failure(suite, wtf_str(expr));	\
	}

#define wtf_assert_success()               wtf_test_suite_current_success(suite)
#define wtf_assert_failure()               wtf_test_suite_current_failure(suite)

#define wtf_assert_eq(a, b)                wtf_assert(a == b)
#define wtf_assert_neq(a, b)               wtf_assert(a != b)

#define wtf_assert_null(expr)              wtf_assert(!expr)
#define wtf_assert_notnull(expr)           wtf_assert(expr)

#define wtf_assert_streq(a, b)             wtf_assert(strcmp(a, b) == 0)
#define wtf_assert_strneq(a, b)            wtf_assert(strcmp(a, b) != 0)

#define wtf_assert_predicate(a, predicate) wtf_assert(predicate(a))




///// wtf Test Suites /////

#ifndef WTF_TESTS_PER_SUITE
#define WTF_TESTS_PER_SUITE 10
#endif

struct wtf_test_suite;

typedef void (*wtf_test_suite_runner_t)(struct wtf_test_suite *);

typedef void (*wtf_test_suite_method_t)(void);

struct wtf_test_suite {
	char *suite_name;
  
	unsigned int tests_count;
	wtf_test_t **tests;

	wtf_test_suite_method_t before_each;
	wtf_test_suite_method_t after_each;

	wtf_test_suite_runner_t runner;
};

typedef struct wtf_test_suite wtf_test_suite_t;

wtf_test_suite_t *wtf_test_suite_new(char *suite_name, wtf_test_suite_runner_t runner) {
	wtf_test_suite_t *suite = (wtf_test_suite_t *)malloc(sizeof(wtf_test_suite_t));

	suite->tests_count = 0;
	suite->tests = (wtf_test_t **)malloc(WTF_TESTS_PER_SUITE * sizeof(wtf_test_t *));
	suite->suite_name = suite_name;
	suite->runner = runner;

	// Initialize before and after each to NULL when creating a new suite.
	suite->before_each = NULL;
	suite->after_each = NULL;

	return suite;
}

void wtf_test_suite_destroy(wtf_test_suite_t *suite) {
	for (int i = 0; i < suite->tests_count; ++i) {
		free(suite->tests[i]);
	}
	free(suite->tests);
}

void wtf_test_suite_add_test(wtf_test_suite_t *suite, wtf_test_t *test) {
	if (suite->tests_count >= WTF_TESTS_PER_SUITE) {
		fprintf(stderr, "no more tests can be added to the suite '%s'\n",
			suite->suite_name);
		return;
	}
	suite->tests[suite->tests_count] = test;
	suite->tests_count++;
}

wtf_test_t *wtf_test_suite_current(wtf_test_suite_t *suite) {
	if (suite->tests_count <= 0) {
		return NULL;
	}
	if (suite->tests_count >= WTF_TESTS_PER_SUITE) {
		fprintf(stderr, "more tests in suite '%s' than possible", suite->suite_name);
		exit(-1);
	}
	return suite->tests[suite->tests_count - 1];
}

void wtf_test_suite_current_success(wtf_test_suite_t *suite) {
	wtf_test_t *test = wtf_test_suite_current(suite);
	test->successful_assertions++;

	if (test->result != WTF_TEST_RESULT_FAILURE) {
		test->result = WTF_TEST_RESULT_SUCCESS;
	}
}

void wtf_test_suite_current_failure(wtf_test_suite_t *suite, char *assertion) {
	wtf_test_t *test = wtf_test_suite_current(suite);
	if (test->failed_assertions_count >= test->failed_assertions_cap) {
		test->failed_assertions_cap += WTF_TEST_ASSERTIONS_CAP_INCREMENT;
		test->failed_assertions = (char **)realloc(test->failed_assertions,
							   test->failed_assertions_cap * sizeof(char *));
	}
	test->failed_assertions[test->failed_assertions_count] = assertion;

	test->failed_assertions_count++;

	test->result = WTF_TEST_RESULT_FAILURE;
}

#define wtf_suite(name)					\
	void test_suite_##name(wtf_test_suite_t *suite)

void wtf_call_suite_method_if_not_null(wtf_test_suite_method_t method) {
	if (method) {
		(*method)();
	}
}

#define wtf_suite_test(name)					\
	if (suite->tests_count != 0) {				\
		wtf_call_suite_method_if_not_null(suite->after_each);	\
	}							\
	wtf_test_t *test_##name = wtf_test_new(wtf_str(name));	\
	wtf_test_suite_add_test(suite, test_##name);		\
	wtf_call_suite_method_if_not_null(suite->before_each);



///// wtf Test Suites - Before and After Each /////

#define wtf_suite_before_each(name) \
	void test_suite_##name##_before_each()

#define wtf_suite_after_each(name) \
	void test_suite_##name##_after_each()




///// wtf Context /////

#ifndef WTF_SUITES_PER_CONTEXT
#define WTF_SUITES_PER_CONTEXT 10
#endif

typedef struct {
	unsigned int suites_count;
	wtf_test_suite_t **suites;
} wtf_context_t;

wtf_context_t *wtf_context_new() {
	wtf_context_t *ctx = (wtf_context_t *)malloc(sizeof(wtf_context_t));

	ctx->suites_count = 0;
	ctx->suites = (wtf_test_suite_t **)malloc(WTF_SUITES_PER_CONTEXT *
						  sizeof(wtf_test_suite_t *));

	return ctx;
}

void wtf_context_destroy(wtf_context_t *ctx) {
	for (int i = 0; i < ctx->suites_count; ++i) {
		wtf_test_suite_destroy(ctx->suites[i]);
		free(ctx->suites[i]);
	}
	free(ctx->suites);
	free(ctx);
}

void wtf_context_add_suite(wtf_context_t *ctx, wtf_test_suite_t *suite) {
	if (ctx->suites_count >= WTF_SUITES_PER_CONTEXT) {
		fprintf(stderr, "no more test suites can be added to the context\n");
		return;
	}
	ctx->suites[ctx->suites_count] = suite;
	ctx->suites_count++;
}

void wtf_context_run_suites(wtf_context_t *ctx, int *success, int *failure) {
        if (ctx->suites_count == 1) {
                printf("Running 1 suite...\n\n");
	}
        else {
                printf("Running %d suites...\n\n", ctx->suites_count);
	}

        for (int i = 0; i < ctx->suites_count; ++i) {
                printf("===== Suite '%s' =====\n\n", ctx->suites[i]->suite_name);

                (*ctx->suites[i]->runner)(ctx->suites[i]);

                for (int j = 0; j < ctx->suites[i]->tests_count; ++j) {
                        wtf_test_t *test = ctx->suites[i]->tests[j];

                        printf("    [%s] Test '%s'",
                               wtf_test_result_string(test->result),
                               test->name
			);

#ifndef WTF_HIDE_SUCCESS_PERCENTAGE					
			float assert_success_percentage =
				(float)test->successful_assertions /
				(test->successful_assertions + test->failed_assertions_count)
				* 100;
			printf(" (%.2f%% successful)\n",
			       assert_success_percentage);
#else
			printf("\n");
#endif

#ifndef WTF_HIDE_FAILED_ASSERTIONS
			if (test->failed_assertions_count > 0) {
				printf("        Failed:\n");
				for (int k = 0; k < test->failed_assertions_count; ++k) {
					printf("        - %s\n", test->failed_assertions[k]);
				}
			}
#endif

                        wtf_test_result_write_counter(test->result, success, failure);
                }
                printf("\n");
        }
}

void wtf_context_log_results(int success, int failure) {
#ifndef WTF_HIDE_FINAL_TEST_RESULTS
	printf("===== Final Test Results =====\n\n");
	printf("Successful: %d\n"
	       "Failed:     %d\n"
	       "--------------\n"
	       "Total:      %d\n",
	       success, failure, success + failure);
#endif
}

#define wtf_init()				\
	wtf_context_t *ctx = wtf_context_new()

#define wtf_register_suite(name)					\
	wtf_test_suite_t *wtf_test_suite_##name =			\
		wtf_test_suite_new(wtf_str(name), &test_suite_##name);	\
	wtf_context_add_suite(ctx, wtf_test_suite_##name)

#define wtf_register_before_each(name) \
	wtf_test_suite_##name->before_each = test_suite_##name##_before_each

#define wtf_register_after_each(name) \
	wtf_test_suite_##name->after_each = test_suite_##name##_after_each

#define wtf_run_suites()					\
	int success = 0, failure = 0;				\
	wtf_context_run_suites(ctx, &success, &failure);	\
	wtf_context_log_results(success, failure)

#define wtf_destroy() \
	wtf_context_destroy(ctx);		\
	return failure > 0 ? 1 : 0

#endif

