#include "wtf.h"

#include "../src/input.h"

wtf_suite(input_tok) {
	char *input = "ls -lars \"some string\"\n";

	char *token = NULL;
	wtf_suite_test(token1) {
		token = trash_tok(input, TRASH_TOK_DELIM);
		printf("Got token: '%s'\n", token);

		wtf_assert_streq(token, "ls");
	}

	wtf_suite_test(token2) {
		token = trash_tok(input, TRASH_TOK_DELIM);
		printf("Got token: '%s'\n", token);

		wtf_assert_streq(token, "-lars");
	}

	wtf_suite_test(token3) {
		token = trash_tok(input, TRASH_TOK_DELIM);
		printf("Got token: '%s'\n", token);

		wtf_assert_streq(token, "\"some string\"");
	}

	wtf_suite_test(token_last) {
		token = trash_tok(input, TRASH_TOK_DELIM);
		printf("Got token: '%s'\n", token);

		wtf_assert_null(token);
	}
}
