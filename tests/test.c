#include "wtf.h"
#include "input_tok_test.h"

wtf_main {
	wtf_init();

	wtf_register_suite(input_tok);

        wtf_run_suites();

	wtf_destroy();
}

