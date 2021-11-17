#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "launch.h"

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
	char username[LOGIN_NAME_MAX];
	getlogin_r(username, LOGIN_NAME_MAX);
	
	int username_len = strlen(username);

	int home_path_len = (6 + username_len);
	char *home_path = (char *) malloc(sizeof(char) * home_path_len);
	sprintf(home_path, "/home/%s", username);
	return chdir(home_path);
}

int trash_cd(int argc, char **args) {
	if (args[1] == NULL) {
		if (chdir_home() != 0) {
			perror("trash");
		}
	} else {
		if (strcmp(args[1], "~") == 0) {
			if (chdir_home() != 0) {
				perror("trash");
			}
		} else {
			if (chdir(args[1]) != 0) {
				perror("trash");
			}
		}
	}
	return 1;
}

const char *trash_ascii_art =
    R"(
 __/-\__    _                 _     
|-------|  | |               | |    
 |     |   | |_ _ __ __ _ ___| |__  
 | | | |   | __| '__/ _` / __| '_ \ 
 | | | |   | |_| | | (_| \__ \ | | |
 |_____|    \__|_|  \__,_|___/_| |_|                            

                   -                   
       Trash Ridicules Any SHell               


  ~ the best shell known to mankind ~  
                                       
)";

int trash_help(int argc, char **args) {
	printf("%s", trash_ascii_art);
	return 1;
}

int trash_exit(int argc, char **args) {
	return 0;
}


int trash_ls(int argc, char **args) {
	printf("About to initialize buffer!");
	char **ls_args_with_color_auto =
		(char **) malloc((argc + 1) * sizeof(char *));

	ls_args_with_color_auto[0] = args[0];
	ls_args_with_color_auto[1] = "--color=auto";

	for (int i = 1; i < argc; ++i) {
		printf("Index %d -> arg: %s", i, args[i]);
		ls_args_with_color_auto[i + 1] = args[i];
	}

	return trash_launch(ls_args_with_color_auto);
}
