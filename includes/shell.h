#ifndef SHELL_H
#define SHELL_H

#include <limits.h> // For PATH_MAX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define BIN_PATH "./bin/"
#define MAX_HISTORY 10

void type_prompt();
void read_command(char **cmd);
/*
Handler of each shell builtin function
*/
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_usage(char **args);
int list_env(char **args);
int set_env_var(char **args);
int unset_env_var(char **args);
int num_builtin_functions();
bool check_path(char *line);
void process_rc_file(void);
void add_to_history(char **cmd);
int shell_history(char **args);
extern const char *builtin_commands[];
extern int (*builtin_command_func[])(char **);
#endif