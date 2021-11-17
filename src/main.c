#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "input.h"
#include "launch.h"
#include "prompt.h"

int main(int argc, char **argv) {

	char *line;
	char **args;
	int status;

	do {
		trash_print_prompt();
		line = trash_read_line();

		int trash_argc = 0;
		args = trash_split_line(&trash_argc, line);
		
		trash_expand_envs(trash_argc, args);
		
		status = trash_execute(trash_argc, args);
		printf("\n");

		free(line);
		free(args);
	} while (status);

	return EXIT_SUCCESS;
}
