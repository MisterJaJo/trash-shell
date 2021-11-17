#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trash_read_line(void) {
#define TRASH_RL_BUFSIZE 1024
	int bufsize = TRASH_RL_BUFSIZE;
	int position = 0;
	char *buffer = (char *)malloc(sizeof(char) * bufsize);
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
			buffer = (char *)realloc(buffer, bufsize);
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
	char **tokens = (char **)malloc(bufsize * sizeof(char *));
	char *token, **tokens_backup;

	if (!tokens) {
		fprintf(stderr, "trash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	// TODO: parse strings in quotes?
	token = strtok(line, TRASH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += TRASH_TOK_BUFSIZE;
			tokens_backup = tokens;
			tokens = (char **)realloc(tokens, bufsize * sizeof(char *));
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

