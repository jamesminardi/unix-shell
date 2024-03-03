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
    // char* input_file;
    // char* output_file;
    // bool append;
    // bool background;
} command_t;

typedef struct {
    char* prompt;

    char* input; // Buffer for user input
    size_t input_size; // Number of bytes allocated for input
    command_t command;
    
    pid_t* children;
    int num_children;
} state_t;




state_t* shell_init() {
    state_t* state = malloc(sizeof(state_t));

    state->prompt = "308sh> ";
    state->input = NULL;
    state->input_size = 0;

    return state;
}

void shell_set_prompt(state_t* state, char* prompt) {
    state->prompt = prompt;
}

char* shell_get_prompt(state_t* state) {
    return state->prompt;
}


void shell_terminate(state_t* state) {

    free(state->input);
    free(state);


}

void shell_output_prompt(state_t* state) {
    printf("%s", state->prompt);
}

void shell_print_input(state_t* state) {
    printf("%s", state->input);
}

void shell_fix_input(state_t* state) {

    // Remove newline from input
    if (state->input_size > 0) {
        state->input[state->input_size - 1] = '\0';
    }

    // Remove trailing spaces
    int i = state->input_size - 2;
    while (i >= 0 && state->input[i] == ' ') {
        state->input[i] = '\0';
        i--;
    }

}

ssize_t shell_get_input(state_t* state) {
    // Get line of user input using getline
    // returns number of characters read including new line, but not including null terminator
    // return -1 on failure
    return getline(&(state->input), &(state->input_size), stdin); // MUST FREE INPUT EVEN ON FAILURE

}

void shell_tokenize_input(state_t* state) {

    command_t command = {.tokens = NULL, .num_tokens = 0};
    
    // Strtok is destructive, so we need to copy the input
    char* input_cpy = (char*)malloc(state->input_size);
    strcpy(input_cpy, state->input);

    // Get number of tokens
    char* token = strtok(input_cpy, " \n");
    while (token != NULL) {
        token = strtok(NULL, " \n");
        command.num_tokens++;
    }

    printf("Num Tokens: %d\n", command.num_tokens);

    // Assign tokens to tokens
    strcpy(input_cpy, state->input); // Reset copied input
    command.tokens = (char**)malloc(command.num_tokens * sizeof(char*));
    token = strtok(state->input, " \n");
    command.tokens[0] = token;
    for (int i = 1; i < command.num_tokens; i++) {
        token = strtok(NULL, " \n");
        command.tokens[i] = token;
    }

    free(input_cpy);

}


void shell_run_exit(state_t* state) {
    shell_terminate(state);
    exit(0);
}


void shell_run_cd(state_t* state) {
    // Change directory
    // Print error if directory not found
}


void shell_run_pwd(state_t* state) {
    // Print working directory
}

void shell_run_pid(state_t* state) {
    // Print process id
}

void shell_run_ppid(state_t* state) {
    // Print parent process id
}

void shell_run_user_program(state_t* state) {
    // Create child process
    pid_t pid = fork();

    if (pid == -1) {
        // Print error
        // Abort
    }

    if (pid == 0) {
        // Child process
        // Execute command
        // Print error if command not found
        // Exit
    } else {
        // Parent process
        // Wait for child process to finish
        // Print error if child process failed
    }
}

void shell_run_command(state_t* state) {
    
    char* cmd = state->command.tokens[0];

    // Test for built in command
    if(strcmp(cmd, CMD_EXIT) == 0) {

    }
    else if (strcmp(cmd, CMD_CD) == 0) {

    }
    else if (strcmp(cmd, CMD_PWD) == 0) {

    }
    else if (strcmp(cmd, CMD_PID) == 0) {

    }
    else if (strcmp(cmd, CMD_PPID) == 0) {

    }
    else {
        // Not a build in cmd
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
                    exit(0);
                    // Print error
                    // Abort
                }
                shell_set_prompt(state, optarg);
                break;

            case 'h':
            default:
                // Print help
                // Exit
                exit(0);
                break;

        }
    }
    



    while(true) {

        // Output prompt

        // Get line of user input using getline
        // returns number of characters read including new line, but not including null terminator
        // return -1 on failure
        // printf("Output prompt...\n");
        shell_output_prompt(state);


        // printf("Get Input...\n");
        shell_get_input(state); // MUST FREE INPUT EVEN ON FAILURE


        // printf("Print input...\n");
        shell_print_input(state);

        // process input (create command from input)


        printf("Tokenize input...\n");
        shell_tokenize_input(state);

        // shell_run_command(state);

        free(state->input);
    }

    

    return 0;
}