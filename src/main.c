#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "launch.h"

char *trash_read_line(void) {
#define TRASH_RL_BUFSIZE 1024
	int bufsize = TRASH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "trash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		// Read a character
		c = getchar();

		if (c == EOF) {
			exit(EXIT_SUCCESS);
		} else if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;

		// If we have exceeded the buffer, reallocate.
		if (position >= bufsize) {
			bufsize += TRASH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "trash: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

#define TRASH_TOK_BUFSIZE 64
#define TRASH_TOK_DELIM " \t\r\n\a"

char **trash_split_line(int *argc, char *line) {
	int bufsize = TRASH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token, **tokens_backup;

	if (!tokens) {
		fprintf(stderr, "trash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	// TODO: parse strings in quotes
	token = strtok(line, TRASH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += TRASH_TOK_BUFSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens) {
				free(tokens_backup);
				fprintf(stderr, "trash: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TRASH_TOK_DELIM);
		*argc += 1;
	}
	tokens[position] = NULL;
	return tokens;
}

void trash_print_prompt(void) {
	char hostname[HOST_NAME_MAX];
	char username[LOGIN_NAME_MAX];

	gethostname(hostname, HOST_NAME_MAX);
	getlogin_r(username, LOGIN_NAME_MAX);

	errno = 0;
	char *path = malloc(sizeof(char) * PATH_MAX);
	if (getcwd(path, PATH_MAX) == NULL) {
		if (errno == ERANGE) {
			printf("path length exceeds the buffer size\n");
		} else {
			perror("getcwd");
		}
		exit(EXIT_FAILURE);
	}

	int username_len = strlen(username);
	char *home_path = (char *) malloc(sizeof(char) * (username_len + 6));
	sprintf(home_path, "/home/%s", username);

	if (strcmp(home_path, path) == 0) {
		printf("%s@%s\n ~ $ ",
		       username, hostname);		
	} else {
		printf("%s@%s\n %s $ ",
		       username, hostname,
		       path);
	}

	free(home_path);
	free(path);
}

int main(int argc, char **argv) {

	char *line;
	char **args;
	int status;

	do {
		trash_print_prompt();
		line = trash_read_line();

		int argc = 0;
		args = trash_split_line(&argc, line);
		status = trash_execute(argc, args);
		printf("\n");

		free(line);
		free(args);
	} while (status);

	return EXIT_SUCCESS;
}
