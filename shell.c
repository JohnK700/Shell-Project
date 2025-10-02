#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>


// Function Prototypes
void shellLoop();
void parseCommand(char *input, char **args);
void addHistory(const char *command);
void showHistory();
void clearHistory();
void executeCommand(char **args);
void pipeHandling(char *input); 
void cleanWhitespace(char *input);



char *history[100]; // History Array
int historyCount = 0; // Amount of commands in history array

int main(int argc, char *argv[])
{
    shellLoop(); // Run the shell
    return 0;
}

/*
This function operates the entire shell. It will read
the user's input and add it into the history array. Also
implements built in commands such as exit or cd and checks if
there is any piping in a command.
*/

void shellLoop() 
{
    char *inputCommand = NULL;
    size_t len = 0;
    while (1) 
    {
        printf("sish> "); // Print the shell prompt
        if (getline(&inputCommand, &len, stdin) == -1) // Get the user input
        {
            break;
        }
        inputCommand[strcspn(inputCommand, "\n")] = 0; // Remove new line character from the end
        cleanWhitespace(inputCommand); // Gets rid of any leading or trailing whitespaces
        addHistory(inputCommand); // Add command to history
            
        if (strchr(inputCommand, '|')) // Check if the command is a pipe command
        {  
            pipeHandling(inputCommand); // Handles the pipe commands
            continue;
        } 
             
        char *arguments[100]; // Array for the parsed commands
        parseCommand(inputCommand, arguments); // Tokenize user's input
        if (arguments[0] == NULL) 
        {
            continue;
        } 
            
        if (strcmp(arguments[0], "exit") == 0) // Checks if "exit" is entered
        {
            clearHistory(); // Clean the history memory
            exit(0); // Exits the shell
        } 
        else if (strcmp(arguments[0], "cd") == 0) // Checks if "cd" is entered
        {
            if (arguments[1]) 
            {
                if (chdir(arguments[1]) != 0) 
                {
                    perror("chdir"); // Prints error if changing directories failed
                }
            } 
            else // Prints error if only "cd" is entered
            {
                fprintf(stderr, "cd: Error! Missing an argument.\n");
            }
        } 
        else if (strcmp(arguments[0], "history") == 0) // Checks if at least "history" was entered
        {
            if (arguments[1] && strcmp(arguments[1], "-c") == 0) // Checks if "history -c" was entered
            {
                clearHistory(); // Clears the history
            } 
            else if (arguments[1]) 
            {
                int offset = atoi(arguments[1]); // Converts the string into an integer
                if (offset >= 0 && offset < historyCount) // Checks if a valid offset is entered after "history"
                {
                    char *args[100];
                    parseCommand(history[offset], args); // Parse the command from history
                    executeCommand(args); // Execute that specific command
                } 
                else // Displays if the offset entered is invalid
                {
                    fprintf(stderr, "history: Error! Invalid offset.\n");
                }
            } 
            else // Displays the history if only "history" is entered
            {
                showHistory();
            }
        }
        else 
        {
            executeCommand(arguments); // Executes the non-built-in commands
        } 
        
    }
}

/*
This function tokenizes the commands that are entered
by the user using strtok().
*/

void parseCommand(char *input, char **args)
{
    char *token = strtok (input, " "); // Breaks the input into tokens
        
    for(int i = 0; i < 99; i++) // Store each token into array
    {
        if(token != NULL)
        {
            args[i] = token;
            token = strtok(NULL, " "); // Get the next token
        }
        else
        {
            args[i] = NULL; // Make sure last argument is null
            break;
        }
    } 
     
}

/*
This function adds each command onto the history array.
Once the array exceeds 100 commands, the oldest command 
will be removed.
*/

void addHistory(const char *command) 
{
    if (historyCount > 100) // Check if there is already 100 commands in the array
    {
        free(history[0]); // Remove the oldest command
        
        for (int i = 1; i < 101; i++) 
        {
            history[i - 1] = history[i]; // Shift the history commands
        }
        historyCount--;
    }
    history[historyCount] = strdup(command); // Add in the new command
    historyCount++;
}

/*
This function will print all of the commands that
are currently in history
*/

void showHistory() 
{
    for (int i = 0; i < historyCount; i++) 
    {
        printf("%d %s\n", i, history[i]); // Prints the stored commands
    }
}

/*
This function will free all commands 
that were stored in history.
*/

void clearHistory() 
{
    for (int i = 0; i < historyCount; i++) 
    {
        free(history[i]); // Frees the allocated memory
    }
    historyCount = 0; // Reset the count to 0
}

/* 
This function will run a command using a child
process and it will then call execvp() to execute 
the command.
*/

void executeCommand(char **args) 
{
    pid_t pid = fork(); // Creates a new process
    if (pid == 0) // Child process
    {
        execvp(args[0], args); // Replaces current process with new command
        perror("execvp"); // Prints error if execvp fails
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) // Parent process
    {
        wait(NULL); // Waits for child to finish
    } 
    else 
    {
        perror("fork"); // Prints error if the fork failed
    }
}

/*
This function handles the execution of multiple
piped commands. Creates pipes for IPC and uses dup2()
to redirect input and output. At the end, it will wait 
for all child processes to finish.
*/

void pipeHandling(char *input) 
{
    char *commands[100];  // Array of commands
    int pipeCount = 0; // Total amount of pipes
    char *token = strtok(input, "|");
    
    // Split the input based on the pipe symbol
    while (token != NULL) 
    {
        commands[pipeCount++] = token; // Store the commands
        token = strtok(NULL, "|");
    }

    int prevfd = 0; // Previous pipe's file descriptor
    int fd[2]; // Pipe file descriptors

    for (int i = 0; i < pipeCount; i++) 
    {
        if (i < pipeCount - 1) 
        {
            if(pipe(fd) == -1) // Create pipe
            {
                perror("pipe"); // Displays error if creating pipe failed
                return;
            } 
        }

        pid_t pid = fork();
        if (pid == 0) // Child process
        {
            if (prevfd != 0) 
            {
                dup2(prevfd, STDIN_FILENO); // Connect previous pipe output to stdin
                close(prevfd);
            }

            if (i < pipeCount - 1) 
            {
                dup2(fd[1], STDOUT_FILENO); // Connect stdout to new pipe
                close(fd[1]);
            }

            // Split the current command into arguments
            char *args[100];
            parseCommand(commands[i], args);
            execvp(args[0], args);  // Execute the command

            // Displays error if execvp fails
            perror("execvp");
            exit(EXIT_FAILURE);
        } 
        else // Parent process
        {
            
            if (prevfd != 0) 
            {
                close(prevfd);
            }
            if (i < pipeCount - 1) 
            {
                close(fd[1]);
            }
            prevfd = fd[0];
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < pipeCount; i++) 
    {
        wait(NULL);
    }
}

/*
This function will trim out the leading and trailing
whitespaces, preventing commands that contain
whitespaces from not executing properly.
*/

void cleanWhitespace(char *input) 
{
    char *start = input;
    char *end = input + strlen(input) - 1;

    while (*start && isspace(*start)) // Trim leading spaces
    {
        start++;
    }
    
    while (end > start && isspace(*end)) // Trim trailing spaces
    {
        end--;
    }

    // Shift the string to the start
    memmove(input, start, end - start + 1);
    input[end - start + 1] = '\0';  // Null-terminate the string
}