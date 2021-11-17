#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

char *get_home_dir() {
	char username[LOGIN_NAME_MAX];
	getlogin_r(username, LOGIN_NAME_MAX);
	
	int username_len = strlen(username);

	int home_path_len = (6 + username_len);
	char *home_path = (char *) malloc(sizeof(char) * home_path_len);

	sprintf(home_path, "/home/%s", username);

	return home_path;
}

void trash_expand_envs(int argc, char **args) {
	for (int i = 0; i < argc; ++i) {
		if (strcmp(args[i], "~") == 0) {
			args[i] = get_home_dir();
			continue;
		}

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
