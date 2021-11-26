#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TRASH_RL_BUFSIZE 1024

char *trash_read_line(void) {
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

static int curr_index = 0;
static int tok_state = 0;

#define TRASH_TOK_LEN 64

char *trash_tok(char *input, char *delimiters) {
	if(!input || !delimiters)
		return NULL;

	if (input[curr_index] == '\0')
		return NULL;

	if (tok_state == 1) {
		tok_state = 0;
		return NULL;
	}

	int bufsize = TRASH_TOK_LEN;
	char *tok_buffer = (char *)malloc(sizeof(char) * bufsize);
	int i = curr_index, k = 0, j = 0;
	bool inside_string, escaped_string;
	
	inside_string = false;
	escaped_string = false;
	while (input[i] != '\0') {
		for (j = 0; delimiters[j] != '\0'; ++j) {
			if (input[i] == '\\') {
				escaped_string = true;
			}
			else if (input[i] == '"' || input[i] == '\'') {
				if (escaped_string) {
					escaped_string = false;
				} else {
					inside_string = !inside_string;
				}
			} else {
				escaped_string = false;
			}
			
			if (input[i] != delimiters[j])
				tok_buffer[k] = input[i];
			else if (!inside_string) {
				tok_buffer[k] = 0;
				curr_index = i + 1;
				return tok_buffer;
			}
		}

		i++;
		k++;

		// If the length of the token exceeds the buffer length, reallocate.
		if (k >= bufsize) {
			bufsize += TRASH_TOK_LEN;
			tok_buffer = (char *)realloc(tok_buffer, bufsize * sizeof(char));
			if (!tok_buffer) {
				fprintf(stderr, "trash: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	tok_buffer[k] = 0;
	curr_index = 0;
	tok_state = 1;

	return tok_buffer;
}

#define TRASH_TOK_BUFSIZE 64
#define TRASH_TOK_DELIM " \t\r\n\a"

char **trash_split_line(char *line, int *argc) {
	int bufsize = TRASH_TOK_BUFSIZE, position = 0;
	char **tokens = (char **)malloc(bufsize * sizeof(char *));
	char *token, **tokens_backup;

	if (!tokens) {
		fprintf(stderr, "trash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = trash_tok(line, TRASH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		token = trash_tok(line, TRASH_TOK_DELIM);

		position++;

		if (position >= bufsize) {
			bufsize += TRASH_TOK_BUFSIZE;
			tokens_backup = tokens;
			tokens = (char **)realloc(tokens, bufsize * sizeof(char *));
			if (!tokens) {
				fprintf(stderr, "trash: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		*argc += 1;
	}

	tokens[position] = NULL;	
	return tokens;
}

