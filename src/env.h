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

	if (arg[0] != '~' || (arg[1] && arg[1] != '/'))
		return arg;
		
	int home_path_len;
	char *home_path = trash_get_home_dir(&home_path_len);

	int arg_len = strlen(arg);
		
	// Create the new buffer that will contain the expanded home paths
	int expanded_len = arg_len - 1 + home_path_len;
	char *arg_new = (char *)malloc(sizeof(char) * expanded_len + 1);
	if (!arg_new) {
		fprintf(stderr, "trash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	// Expand ~
	int position = 0;

	for (int j = 0; j < home_path_len; ++j) {
		arg_new[position++] = home_path[j];
	}

	for (int i = 1; i < arg_len; ++i) {
		if (!arg[i]) {
			return NULL;
		}
		arg_new[position++] = arg[i];
	}

	// Insert \0 character afterwards
	arg_new[position] = 0;

	return arg_new;
}

void trash_expand_envs(int argc, char **args) {
	for (int i = 0; i < argc; ++i) {
		// Replace ~ at the beginning of args[i] with the home directory
		if ((args[i] = trash_expand_home(args[i])) == NULL) {
			fprintf(stderr, "failed to expand home specifier (~) in input\n");
			return;
		}

		// Expand environment variables
		if (args[i][0] == '$') {
			int len = sizeof(args[i]) - sizeof(char);
			char *env = (char *)malloc(len);

			strcpy(env, args[i] + 1);

			char *env_val = getenv(env);
			if (env_val != NULL) {
				args[i] = env_val;
			}

			free(env);
		}
	}
}

