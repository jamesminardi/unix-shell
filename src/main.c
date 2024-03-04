#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>


// Builtin commands 
#define CMD_EXIT "exit" 
#define CMD_CD "cd"
#define CMD_PWD "pwd"
#define CMD_PID "pid"
#define CMD_PPID "ppid"


typedef struct {
    char** tokens;
    int num_tokens;
    bool background;
} command_t;

typedef struct {
    char* prompt; // Prompt string
    char* input; // Buffer for user input
    char* input_cmd; // Buffer for tokenized command
    size_t input_size; // Number of bytes allocated for input
    command_t command; // Current command being processed
    
    pid_t* children; // Ongoing list of background child processes
    int num_children;
} state_t;



// Initialize shell state and return pointer.
// Must be freed with shell_terminate
state_t* shell_init() {
    state_t* state = malloc(sizeof(state_t));

    state->prompt = "308sh> ";
    state->input = NULL;
    state->input_size = 0;

    
    state->command.tokens = NULL;
    state->command.num_tokens = 0;
    state->command.background = false;


    state->children = NULL;
    state->num_children = 0;

    return state;
}

// Set the shell prompt
void shell_set_prompt(state_t* state, char* prompt) {
    state->prompt = prompt;
}

// Get the shell prompt
char* shell_get_prompt(state_t* state) {
    return state->prompt;
}

// Terminate the shell state and free memory
void shell_terminate(state_t* state) {

    free(state->children);
    free(state->command.tokens);
    free(state->input_cmd);
    free(state->input);
    free(state);

}

// Output the shell prompt
void shell_output_prompt(state_t* state) {
    usleep(1000);
    printf("%s", state->prompt);
}

// Print the user input for debugging
void shell_print_input(state_t* state) {
    printf("%s", state->input);
}

// Get user input
ssize_t shell_get_input(state_t* state) {
    // returns number of characters read including new line, but not including null terminator
    // return -1 on failure
    return getline(&(state->input), &(state->input_size), stdin); // MUST FREE INPUT EVEN ON FAILURE

}

// Converts input string into command tokens/args.
void shell_tokenize_command(state_t* state) {
    
    state->command = (command_t){.tokens = NULL, .num_tokens = 0, .background = false};

    // Strtok is destructive, so we need to copy the input
    state->input_cmd = malloc(state->input_size);
    strcpy( state->input_cmd, state->input);

    // Find the total number of tokens
    char* token = strtok( state->input_cmd, " \n");
    while (token != NULL) {
        token = strtok(NULL, " \n");
        state->command.num_tokens++;
    }

    // Assign tokens to tokens
    free(state->input_cmd);
    state->input_cmd = malloc(state->input_size);
    strcpy(state->input_cmd, state->input); // Reset copied input
    state->command.tokens = (char**)malloc((state->command.num_tokens + 1) * sizeof(char*));


    // Assign the tokens to the array
    token = strtok(state->input_cmd, " \n");
    state->command.tokens[0] = token;
    for (int i = 1; i < state->command.num_tokens; i++) {
        token = strtok(NULL, " \n");
        state->command.tokens[i] = token;
    }
    
    // Tokenize the background process flag
    if (state->command.num_tokens > 0 && strlen(state->command.tokens[state->command.num_tokens - 1]) == 1 && state->command.tokens[state->command.num_tokens - 1][0] == '&') {
        state->command.background = true;
        state->command.tokens[state->command.num_tokens - 1] = (char*)NULL;
    }

    // Null terminate tokens for execvp
    state->command.tokens[state->command.num_tokens] = (char*)NULL; 

}


// Shell command: exit
void shell_run_exit(state_t* state) {
    shell_terminate(state);
    exit(0);
}


// Shell command: change directory
void shell_run_cd(state_t* state) {

    // Change directory to home
    if (state->command.num_tokens == 1) {
        if (chdir(getenv("HOME")) != 0) {
            printf("-shell: cd: %s: No such file or directory\n", getenv("HOME"));
        }
    }
    // Change directory to arg
    else if (state->command.num_tokens == 2) {
        if (chdir(state->command.tokens[1]) != 0) {
            printf("-shell: cd: %s: No such file or directory\n", state->command.tokens[1]);
        }
    }
    // Too many arguments
    else {
        // Too many args
        printf("-shell: cd: too many arguments\n");
    }
}


// Shell command: print working directory
void shell_run_pwd() {
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        // Print error
        printf("-shell: pwd: error\n");
    }
    else {
        printf("%s\n", cwd);
        free(cwd);
    }
}

// Shell command: print process id
void shell_run_pid() {
    printf("%d\n", getpid());
}

// Shell command: print parent process id
void shell_run_ppid() {
    printf("%d\n", getppid());
}

// Shell command: runn user program
void shell_run_user_program(state_t* state) {
    
    // Create child process
    pid_t pid = fork();
    int status;

    if (pid == -1) {
        printf("-shell: Failed to create child process for user program\n");
        shell_terminate(state);
    }

    // Child process
    if (pid == 0) {

        printf("[%d]: %s\n", getpid(), state->command.tokens[0]);

        // If child process, execute execvp to replace child process with new program
        if (execvp(state->command.tokens[0], state->command.tokens) != 0) {

            printf("Cannot excecute %s: No such file or directory\n", state->command.tokens[0]);

            exit(-1); // Just exit child process, because terminate will free state

        }

    }
    // Parent process
    else {
        
        // If background process, do not wait for child process to finish
        if (state->command.background) {

            state->num_children++;
            state->children = (pid_t*)realloc(state->children, state->num_children * sizeof(pid_t));
            state->children[state->num_children - 1] = pid;

        }
        // If foreground process, wait for child process to finish
        else {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("[%d]: %s Exit %d\n", pid, state->command.tokens[0], WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)) {
                printf("[%d]: %s Killed by signal %d\n", pid, state->command.tokens[0],  WTERMSIG(status));
            }
            else if (WIFSTOPPED(status)) {
                printf("[%d]: %s Stopped by signal %d\n", pid, state->command.tokens[0],  WSTOPSIG(status));
            }
        }
    }
}

// Select command to run using the tokenized command array
void shell_run_command(state_t* state) {
    
    char* cmd = state->command.tokens[0];


    // Checks for command of length 0
    if (cmd == NULL) {
        return;
    }

    // Test for built in command
    if(strcmp(cmd, CMD_EXIT) == 0) {
        shell_run_exit(state);
    }
    else if (strcmp(cmd, CMD_CD) == 0) {
        shell_run_cd(state);
    }
    else if (strcmp(cmd, CMD_PWD) == 0) {
        shell_run_pwd();
    }
    else if (strcmp(cmd, CMD_PID) == 0) {
        shell_run_pid();
    }
    else if (strcmp(cmd, CMD_PPID) == 0) {
        shell_run_ppid();
    }
    else {
        // Not a built in cmd
        shell_run_user_program(state);
    }
    
}

// Check for child processes that have finished
void check_child_processes(state_t* state) {
    int status;
    pid_t pid;

    for (int i = 0; i < state->num_children; i++) {

        pid = state->children[i];

        // Don't block for waiting, just test if it has happened yet.
        if (waitpid(pid, &status, WNOHANG) != 0) {

            if (WIFEXITED(status)) {
                printf("[%d]: Exit %d\n", pid, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)) {
                printf("[%d]: Killed by signal %d\n", pid, WTERMSIG(status));
            }
            else if (WIFSTOPPED(status)) {
                printf("[%d]: Stopped by signal %d\n", pid, WSTOPSIG(status));
            }

            // remove exited child from list
            state->num_children--;
            for (int j = i; j < state->num_children; j++) {
                state->children[j] = state->children[j + 1];
            }
            i--;
        }
    }
}

int main(int argc, char** argv) {

    state_t* state = shell_init();

    // Parse args
    int opt;
    while((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
            case 'p':
                if (optarg == NULL) {
                    exit(1);
                }
                shell_set_prompt(state, optarg);
                break;

            default:
                printf("Usage: %s [-p prompt]\n", argv[0]);
                exit(1);
                break;

        }
    }
    
    // Main loop for input and processing
    while(true) {

        check_child_processes(state);

        shell_output_prompt(state);

        if (shell_get_input(state) == -1) {
            free(state->input); // During failure, input is still allocated
            continue;
        }

        shell_tokenize_command(state);

        shell_run_command(state);

        free(state->command.tokens);

    }

    return 0;
}