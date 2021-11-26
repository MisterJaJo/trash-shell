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
	char *home_path = (char *) malloc(sizeof(char) * *len_ptr + 1);

	sprintf(home_path, "/home/%s", username);

	// Insert \0 character afterwards
	home_path[*len_ptr] = 0;

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
	free(home_path);

	for (int i = 1; i < arg_len; ++i) {
		if (!arg[i]) {
			free(arg_new);
			return NULL;
		}
		arg_new[position++] = arg[i];
	}

	// Clear the original argument after the remaining contents were copied over.
	free(arg);

	// Insert \0 character afterwards
	arg_new[position] = 0;

	return arg_new;
}

char *trash_expand_env_vars(char *arg) {
	if (!arg)
		return NULL;

	if (arg[0] != '$')
		return arg;

	
	size_t len = sizeof(arg) - sizeof(char);
	char *env = (char *)malloc(len);

	strncpy(env, arg + 1, len);

	// Here, env_val is a pointer to the *actual* environment variable.
        char *env_val = getenv(env); 
	if (env_val != NULL) {
		// To write this value to our args list,
		// we have to allocate some memory...
		int env_val_len = strlen(env_val);
		char *env_val_buffer = (char *)malloc((env_val_len + 1) * sizeof(char));

		// And copy over the bytes of env_val. 
		// (This way, we can also free the buffer later on)
		memcpy(env_val_buffer, env_val, env_val_len * sizeof(char));
		env_val_buffer[env_val_len] = 0;

		// Clear the original argument and insert the new value.
		free(arg);
		arg = env_val_buffer;
	}

	free(env);

	return arg;
}

void trash_expand_envs(int argc, char **args) {
	for (int i = 0; i < argc; ++i) {
		// Replace ~ at the beginning of args[i] with the home directory
		if ((args[i] = trash_expand_home(args[i])) == NULL) {
			fprintf(stderr, "failed to expand home specifier (~) in input\n");
			exit(EXIT_FAILURE);
		}

		// Expand environment variables
		if ((args[i] = trash_expand_env_vars(args[i])) == NULL) {
			fprintf(stderr, "failed to expand environment variables in input\n");
			exit(EXIT_FAILURE);
		}
	}
}

