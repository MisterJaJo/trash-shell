#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

char *trash_get_home_dir(int *len_ptr) {
	char username[LOGIN_NAME_MAX];
	getlogin_r(username, LOGIN_NAME_MAX);
	
	int username_len = strlen(username);

	*len_ptr = (6 + username_len);
	char *home_path = (char *) malloc(sizeof(char) * *len_ptr);

	sprintf(home_path, "/home/%s", username);

	return home_path;
}

char *trash_expand_home(char *arg) {
	if (!arg)
		return NULL;

	int home_path_len;
	char *home_path = trash_get_home_dir(&home_path_len);

	// Count the number of ~ characters in the input string
	int replace_count = 0;
	int arg_len = strlen(arg);

	for (int i = 0; i < arg_len; ++i) {
		if (arg[i] == '~')
			replace_count++;
	}

	// Create the new buffer that will contain the expanded home paths
	int expanded_len = arg_len - replace_count + replace_count * home_path_len;
	char *arg_new = (char *)malloc(sizeof(char) * expanded_len + 1);
	if (!arg_new) {
		fprintf(stderr, "trash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	// Expand ~
	int position = 0;
	for (int i = 0; i < arg_len; ++i) {
		if (!arg[i]) {
			return NULL;
		}

		if (arg[i] == '~') {
			for (int j = 0; j < home_path_len; ++j) {
				arg_new[position++] = home_path[j];
			}
		} else {
			arg_new[position++] = arg[i];
		}
	}

	// Insert \0 character afterwards
	arg_new[position] = 0;

	return arg_new;
}

void trash_expand_envs(int argc, char **args) {
	for (int i = 0; i < argc; ++i) {
		// Replace all occurrences of ~ in args[i] with the home directory
		if ((args[i] = trash_expand_home(args[i])) == NULL) {
			fprintf(stderr, "failed to expand home in input\n");
			return;
		} else {
			printf("Expanded args[%d] -> %s\n", i, args[i]);
		}

		// Expand environment variables
		if (args[i][0] == '$') {
			int len = sizeof(args[i]) - sizeof(char);
			char *env = (char *)malloc(len);

			strcpy(env, args[i] + 1);

			char *ppath = getenv(env);
			if (ppath != NULL) {
				args[i] = ppath;
			}

			free(env);
		}
	}
}

