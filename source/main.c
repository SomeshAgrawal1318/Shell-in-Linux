#include "shell.h"
#include <stdbool.h>


// The main function where the shell's execution begins
int main(void)
{
    char *cmd[MAX_ARGS];
    pid_t pid;
    int status, child_exit_status;
    char shell_bin_prefix[PATH_MAX];

    if (getcwd(shell_bin_prefix, sizeof(shell_bin_prefix)) == NULL) {
        perror("getcwd");
        return 1;
    }
    strncat(shell_bin_prefix, "/bin/", sizeof(shell_bin_prefix) - strlen(shell_bin_prefix) - 1);

    // Define an array to hold the command and its arguments
    while (true){
        /* Initialize cmd array */
        for (int i = 0; i < MAX_ARGS; i++)
            cmd[i] = NULL;

        type_prompt();     // Display the prompt
        read_command(cmd); // Read a command from the user
        bool builtin = false;
        // Formulate the full path of the command to be executed
        if (cmd[0] != NULL && strcmp(cmd[0], "") != 0){
            for (int command_index = 0; command_index < num_builtin_functions(); command_index++) {
                if (strcmp(cmd[0], builtin_commands[command_index]) == 0) {
                    builtin = true;
                    int result = (*builtin_command_func[command_index])(cmd);
                    if (result == 0) {  // exit requested
                        for (int i = 0; cmd[i] != NULL; i++)
                            free(cmd[i]);
                        return 0;
                    }
                    break;
                }
            }

            if (builtin) {
                for (int i = 0; cmd[i] != NULL; i++)
                    free(cmd[i]);
                continue;  // skip fork/exec, go to next iteration
            }

            /* External command: fork and exec */
            pid = fork();
            
            if (pid == 0){
                char full_path[PATH_MAX];
                snprintf(full_path, sizeof(full_path), "%s%s", shell_bin_prefix, cmd[0]);
                execv(full_path, cmd);

                // If execv returns, command execution has failed
                printf("Command %s not found\n", cmd[0]);
                exit(1);
                }
            else if (pid >0){
                printf("Parent waiting for the child to finish\n");
                waitpid(pid, &status, 0);
                // if child terminates properly,
                if (WIFEXITED(status))
                {
                    child_exit_status = WEXITSTATUS(status);
                    if (child_exit_status == 0){
                        printf("Child finished working\n");
                    }else{
                        printf("There was some Error\n");
                    }
                }

                for (int i = 0; cmd[i] != NULL; i++)
                {
                    free(cmd[i]);
                }
            }
        }
    }
}