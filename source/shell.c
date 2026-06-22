// This code was written by ChatGPT4
// Modify it for your own usage to implement features for PA1 (or completely
// rewrite it) Include the shell header file for necessary constants and
// function declarations
#include "shell.h"
#include <errno.h>
extern char **environ; 
const char *builtin_commands[] = {
    "cd",    // Changes the current directory of the shell to the specified path. If no path is given, it defaults to the user's home directory.
    "help",  //  List all builtin commands in the shell
    "exit",  // Exits the shell
    "usage", // Provides a brief usage guide for the shell and its built-in command
    "env", // Lists all the environment variables currently set in the shell
    "setenv", // Sets or modifies an environment variable for this shell session
    "unsetenv", // Removes an environment variable from the shell
    "history" // Prints the last MAX_HISTORY commands entered
    };

const char *builtin_usage[] = {
    "cd [dir] - Change the current working directory. If no directory is given, use HOME.",
    "help - List all builtin commands.",
    "exit - Exit the shell.",
    "usage [command] - Show usage for builtin commands.",
    "env - List all environment variables currently set in the shell.",
    "setenv KEY=VALUE - Set or modify an environment variable.",
    "unsetenv KEY - Remove an environment variable.",
    "history - Show the last 10 commands entered.",
};

/*** This is array of functions, with argument char ***/
int (*builtin_command_func[])(char **) = {
    &shell_cd,     // builtin_command_func[0]: cd 
    &shell_help,   // builtin_command_func[1]: help
    &shell_exit,   // builtin_command_func[2]: exit
    &shell_usage,  // builtin_command_func[3]: usage
    &list_env,     // builtin_command_func[4]: env
    &set_env_var,    // builtin_command_func[5]: setenv
    &unset_env_var,  // builtin_command_func[6]: unsetenv
    &shell_history   // builtin_command_func[7]: history
};

int num_builtin_functions(void)
{
    return sizeof(builtin_commands) / sizeof(char *);
}


// Function to read a command from the user input
void read_command(char **cmd)
{
  // Define a character array to store the command line input
  char line[MAX_LINE];
  // Initialize count to keep track of the number of characters read
  int count = 0, i = 0;
  // Array to hold pointers to the parsed command arguments
  char *array[MAX_ARGS], *command_token;

  // Infinite loop to read characters until a newline or maximum line length is
  // reached
  for (;;)
  {
    // Read a single character from standard input
    int current_char = fgetc(stdin);
    // Store the character in the line array and increment count
    line[count++] = (char)current_char;
    // If a newline character is encountered, break out of the loop
    if (current_char == '\n')
      break;
    // If the command exceeds the maximum length, print an error and exit
    if (count >= MAX_LINE)
    {
      printf("Command is too long, unable to process\n");
      exit(1);
    }
  }
  // Null-terminate the command line string
  line[count] = '\0';

  // If only the newline character was entered, return without processing
  if (count == 1)
    return;

  // Use strtok to parse the first token (word) of the command
  command_token = strtok(line, " \n");

  // Continue parsing the line into words and store them in the array
  while (command_token != NULL)
  {
    array[i++] = strdup(command_token);  // Duplicate the token and store it
    command_token = strtok(NULL, " \n"); // Get the next token
  }

  // Copy the parsed command and its parameters to the cmd array
  for (int j = 0; j < i; j++)
  {
    cmd[j] = array[j];
  }
  // Null-terminate the cmd array to mark the end of arguments
  cmd[i] = NULL;
}

// Function to display the shell prompt
void type_prompt()
{
  // Use a static variable to check if this is the first call to the function
  static int first_time = 1;
  if (first_time)
  {
    // Clear the screen on the first call
#ifdef _WIN32
    system("cls"); // Windows command to clear screen
#else
    // system("clear"); // UNIX/Linux command to clear screen
#endif
    first_time = 0;
  }
  fflush(stdout); // Flush the output buffer
  printf("$$ ");  // Print the shell prompt
}

int shell_exit(char **args){
  (void)args;
  return 0;
}
int shell_cd(char **args){

  char *target = args[1];
  if (target == NULL) {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
        return 1;
    } else {
        perror("getcwd() error");
        return 1;
    }
  }

  if (chdir(target) != 0) {
    fprintf(stderr, "cd: %s: %s\n", target, strerror(errno));
    return 1;
  }

  return 1;
}
int shell_help(char **args){
  (void)args;
  printf("These following commands are implemented within the shell\n");
  for (int command_index = 0; command_index < num_builtin_functions(); command_index++)
  {
    printf("%s\n", builtin_commands[command_index]);
  }
  return 1;
}
int shell_usage(char **args){
    if (args[1] == NULL) {
        printf("usage [builtin]\n");
        printf("Builtin commands:\n");
        for (int command_index = 0; command_index < num_builtin_functions(); command_index++) {
            printf("  %s\n", builtin_usage[command_index]);
        }
        return 1;
    }

    for (int command_index = 0; command_index < num_builtin_functions(); command_index++) {
        if (strcmp(args[1], builtin_commands[command_index]) == 0) {
            printf("%s\n", builtin_usage[command_index]);
            return 1;
        }
    }

    fprintf(stderr, "usage: '%s' is not a shell builtin.\n", args[1]);
    fprintf(stderr, "Type 'help' to see the list of available builtin commands.\n");
    return 1;
}
int list_env(char **args){
    char **env = environ; // Pointer to the array of environment strings
    
    while (*env) { // Loop until NULL pointer is encountered
        printf("%s\n", *env); // Print the current environment variable
        env++; // Move to the next environment variable
    }
    return 1;
}
int set_env_var(char **args){
  char *key = args[1];
  char *value = NULL;
  char *delimiter = strchr(key, '=');
  if (delimiter != NULL){
    *delimiter = '\0';
    value = delimiter +1;
  }
  setenv(key, value, 1);
  return 1;

}
int unset_env_var(char **args){
  unsetenv(args[1]);
  return 1;
}

// Circular buffer storing the last MAX_HISTORY commands
static char *history[MAX_HISTORY];
static int history_count = 0;  // total commands added so far (never resets)

void add_to_history(char **cmd)
{
    // Reconstruct the full command string from its tokens (e.g. "cd files")
    char entry[MAX_LINE] = "";
    for (int i = 0; cmd[i] != NULL; i++) {
        if (i > 0)
            strncat(entry, " ", sizeof(entry) - strlen(entry) - 1);
        strncat(entry, cmd[i], sizeof(entry) - strlen(entry) - 1);
    }

    // Overwrite the oldest slot once the buffer is full
    int slot = history_count % MAX_HISTORY;
    free(history[slot]);          // free previous entry in this slot (NULL-safe)
    history[slot] = strdup(entry);
    history_count++;
}

int shell_history(char **args)
{
    (void)args;
    // How many entries are actually stored (capped at MAX_HISTORY)
    int stored = history_count < MAX_HISTORY ? history_count : MAX_HISTORY;
    // Index of the oldest entry in the circular buffer
    int oldest = history_count > MAX_HISTORY ? history_count % MAX_HISTORY : 0;

    for (int i = 0; i < stored; i++) {
        int slot = (oldest + i) % MAX_HISTORY;
        // Print with a global sequence number so it looks like bash history
        printf("%3d  %s\n", history_count - stored + i + 1, history[slot]);
    }
    return 1;
}
bool check_path(char *line){
  char path[5];
  memcpy(path, line, 4);
  path[4] = '\0';
  if (strcmp(path, "PATH") == 0){
    return true;
  }
  return false;
}

void process_rc_file(void)
{
    const char *filePath = ".cseshellrc";
    FILE *file = fopen(filePath, "r");
    if (file == NULL)
        return;

    char line[1024];
    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (line[0] == '\0')
            continue;

        if (check_path(line))
        {
            char *delimiter = strchr(line, '=');
            if (delimiter != NULL)
            {
                *delimiter = '\0';
                setenv(line, delimiter + 1, 1);
            }
            continue;
        }

        char *rc_cmd[MAX_ARGS];
        int rc_argc = 0;
        char *token = strtok(line, " \t");
        while (token != NULL && rc_argc < MAX_ARGS - 1)
        {
            rc_cmd[rc_argc++] = token;
            token = strtok(NULL, " \t");
        }
        rc_cmd[rc_argc] = NULL;

        if (rc_argc == 0)
            continue;

        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(rc_cmd[0], rc_cmd);
            printf("Command %s not found\n", rc_cmd[0]);
            _exit(1);
        }
        else if (pid > 0)
        {
            int status;
            waitpid(pid, &status, 0);
        }
    }

    fclose(file);
}