#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "launch.h"
#include "env.h"

/*
  Function Declarations for builtin shell commands:
*/
int trash_cd(int argc, char **args);
int trash_help(int argc, char **args);
int trash_exit(int argc, char **args);
int trash_ls(int argc, char **args);

/*
  List of builtin commands, followed by their corresponding functions.
*/
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "ls"
};

int (*builtin_func[])(int, char **) = {
    &trash_cd,
    &trash_help,
    &trash_exit,
    &trash_ls
};


int trash_builtin_count() {
	return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

int chdir_home() {
	int len;
	char *home_path = trash_get_home_dir(&len);
	return chdir(home_path);
}

int trash_cd(int argc, char **args) {
	if (args[1] == NULL) {
		if (chdir_home() != 0) {
			perror("trash");
		}
	} else {
		if (chdir(args[1]) != 0) {
			perror("trash");
		}
	}
	return 1;
}

const char *trash_ascii_art =
"                                         \n\
 __/-\\__    _                 _          \n\
|-------|  | |               | |          \n\
 |     |   | |_ _ __ __ _ ___| |_         \n\
 | | | |   | __| '__/ _` / __| '_ \\      \n\
 | | | |   | |_| | | (_| \\__ \\ | | |    \n\
 |_____|    \\__|_|  \\__,_|___/_| |_|    \n\
                                          \n\
                   -                      \n\
       Trash Ridicules Any SHell          \n\
                                          \n\
                                          \n\
  ~ the best shell known to mankind ~     \n\
                                          \n";


int trash_help(int argc, char **args) {
	printf("%s", trash_ascii_art);
	return 1;
}

int trash_exit(int argc, char **args) {
	return 0;
}


int trash_ls(int argc, char **args) {
	if (argc == 0 || args[0] == NULL) {
		return -1;
	}

	char **ls_args_with_color_auto =
		(char **) malloc((argc + 2) * sizeof(char *));

	if (!ls_args_with_color_auto) {
		fprintf(stderr, "trash: allocation error\n");
		return -1;
	}

	int position = 0;

	// Copy first argument (program name)
	ls_args_with_color_auto[position++] = args[0];

	// Insert --color=auto
	ls_args_with_color_auto[position++] = "--color=auto";

	// Copy arguments
	for (int i = 1; i < argc; ++i) {
		ls_args_with_color_auto[position++] = args[i];
	}

	// Insert null byte
	ls_args_with_color_auto[position++] = 0;

	return trash_launch(argc, ls_args_with_color_auto);
}

