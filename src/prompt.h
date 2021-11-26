#pragma once

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void trash_print_prompt(void) {
	char hostname[HOST_NAME_MAX];
	char username[LOGIN_NAME_MAX];

	gethostname(hostname, HOST_NAME_MAX);
	getlogin_r(username, LOGIN_NAME_MAX);

	errno = 0;
	char *path = (char *)malloc(sizeof(char) * PATH_MAX);
	if (getcwd(path, PATH_MAX) == NULL) {
		if (errno == ERANGE) {
			printf("path length exceeds the buffer size\n");
		} else {
			perror("getcwd");
		}
		exit(EXIT_FAILURE);
	}

	int username_len = strlen(username);
	char *home_path = (char *) malloc(sizeof(char) * (username_len + 6 + 1));
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

