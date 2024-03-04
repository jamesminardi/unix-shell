# unix-shell
Custom interactive interface between the OS and the user. 


## Requirements

1.  Shell must accept a `p <prompt>` option on the command line when it is initialized. The `<prompt>` option should become the user prompt. If this option is not specified, the default prompt should be used.


2. Shell must run in an infinite loop accepting input from the user and running commands until the user requests to exist.


3. Shell should support two types of user commands: (1) built-in commands, and (2) program commands.
    1. Built-in commands interact with the shell program, such as `cd`, `cd <dir>`, etc.
    2. Program commands require shell to spawn a child process to execute the user input (exactly as typed) using the `execvp()` call.The user command will be entered *either* using an absolute path, a path relative to the current working directory, *or* a path relative to a directory listed in the `PATH` environment variable. Note that `execvp` will search the `PATH` for you.

4. Shell should notify the user if the request command is not found and cannot be run.

5. When executing a program command, print out the reation and exit status information for child process.
    1. ~info on child processes~


6. Shell should support background commands using the suffix ampersand (&), i.e. the child should run in the background, meaning the shell will not wait for the child to exit before prompting the user for further input. You will need to print the creation and exit statuses of these background processes.

## Special Notes

1. `build` directory must exist in the top level for make.
2. The application `shell` is in the `build` folder including generated object files.
3. When executing a background process, return status updates are only polled once per input.
4. Multiple background processes can be executing at once, but program names are not saved.
5. Does not support background process initialization when the `&` is not separated by a space at the end of the command.
6. Does not manipulate the input command futher than what `strtok` does.

## Built-in Commands

- `exit` : Shell should terminate and accept no further input
- `pid` : Shell should print its process ID
- `ppid` : Shell should print the process ID of its parent
- `cd <dir>` : Change the working directory. With no args, change to user's home directory. This is stored in `HOME` environment variable.
- `pwd` : Print the current working directory