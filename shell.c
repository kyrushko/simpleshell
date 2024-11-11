#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "alias.h"
#include "history.h"
#include "shell.h"

void display_prompt(){
    printf("%s> ", getcwd(NULL, 0));
}


// Execute the appropriate command
int runCommand(char* input, char* home, char* path)
{
    char *string[MAX_INPUT_SIZE];
    char tempCommand[MAX_INPUT_SIZE];
    char *aliasCommand[MAX_INPUT_SIZE];

    *aliasCommand = strdup(input);

    int commSize = tokenize(input, string);
	
	//prevent space segmentation fault 
	if (string[0]==NULL){
		return 2;
	}

    strcpy(tempCommand, "");
	
	int maxSubsExceededFlag = 0;
	// Perform alias substitution and check for maximum substitutions
    checkforAlias(string, commSize, 0,&maxSubsExceededFlag);
	
	// Check if the input command refers to a command in history
	int returnValue = checkifHistory(input, string, commSize);
	
	// If the command is invalid (-1), return 2
	if (returnValue==-1){
		return 2;//escape runCommand
	}// If the command is a history command (0), re-tokenize the input
    else if (returnValue == 0) {
        commSize = tokenize(input, string);
		// Perform alias substitution again after history command processing
		checkforAlias(string, commSize, 0,&maxSubsExceededFlag);
    }

	// Concatenate the tokens into a single command string
    for (int i = 0; string[i] != NULL; i++) {
        strcat(tempCommand, string[i]);
        if (string[i + 1] != NULL) {
            strcat(tempCommand, " ");
        }
    }
    commSize = tokenize(tempCommand, string);
	
	// Perform final alias substitution
    checkforAlias(string, commSize, 0,&maxSubsExceededFlag);
	
	 // Store the alias command back into input
    input = strdup(*aliasCommand);
    addToHistory(*aliasCommand);

    if (strcmp(input, "exit") == 0) {
        printf("%s", path);
        setenv("PATH", path, 1);
        return EXIT_SUCCESS;
    }

    else if ((commSize == 2) && strcmp(string[0], "clear") == 0) {
        if (strcmp(string[1], "history") == 0) {
            clearHistory();
        }
    }

    else if (strcmp(string[0], "alias") == 0) {
        if (commSize == 1) {
            printAliases();
        } 
        else if (commSize == 2) {
            printf("Missing corresponding alias command \n");
        } 
        else {
            handleAddCommand(string, commSize);
        }
    }

    else if (strcmp(string[0], "unalias") == 0) {
        if (commSize == 1) {
            printf("missing alias name\n");
        } 
        else {
            removeAlias(string[1]);
        }
    }

    else if (strcmp(string[0], "getpath") == 0) {
        getPath(commSize);
    }

    else if (strcmp(string[0], "setpath") == 0) {
        setPath(string, commSize);
    }

    else if (strcmp(string[0], "cd") == 0) {
        changeDirectory(string, commSize, home);
    }

    else if (strcmp(string[0], "history") == 0) {
        if (commSize > 1) {
            printf("too many parameters\n");
        }
		else{
		printHistory();
		}
    }

    else {
        executeExternalCommand(string);
    }

    memset(string, 0, sizeof(string)); // clearing string
    return 2;
}

// Separate the input command into output
int tokenize(char *input, char **output)
{
    char delim[] = " \t\n;&><|";
    char *ptr = strtok(input, delim);
    int commSize = 0; // Keep track of the number of arguments
    while (ptr != NULL)
    {
        output[commSize++] = ptr;
        ptr = strtok(NULL, delim);
    }
    return commSize;
}


// Print the current PATH environment variable
void getPath(int commSize){
    if (commSize > 1) {
        printf("Too many parameters\n");
    } else {
        // Print the current PATH
        printf("%s\n", getenv("PATH"));
    }
}

// Set the PATH environment variable
void setPath(char** string, int commSize) {
    if (commSize == 1) {
        printf("No parameters\n");
    } else if (commSize > 2) {
        printf("Too many parameters\n");
    } else if (access(string[1], F_OK) == -1) {
        printf("Not a valid path\n");
    } else {
        // Set the PATH and print the updated value
        setenv("PATH", string[1], 1);
        printf("PATH set to: %s\n", getenv("PATH"));
    }
}

// Change the current working directory
void changeDirectory(char** string, int commSize, char* home) {
    if (commSize == 1) {
        // If no argument provided, change to home directory
        if (chdir(home) == -1) {
            perror("chdir");
        }
    } else if (strcmp(string[1], "..") == 0) {
        // If argument is "..", go to the parent directory
        if (access(string[1], F_OK) == -1) {
            printf("Not a valid path\n");
        } else {
            if (chdir(string[1]) == -1) {
                perror(string[1]);
            }
        }
    } else if (commSize > 2) {
        printf("Too many parameters\n");
    } else {
        if (access(string[1], F_OK) == -1) {
            printf("no such file or directory\n");
        } else {
            // change to the specified directory
            if (chdir(string[1]) == -1) {
                perror(string[1]);
            }
        }
    }
}


void executeExternalCommand(char** string) {
    pid_t pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
    } else if (pid == 0) {
        // Child process: execute the command
        execvp(string[0], string);
        perror(string[0]);
        exit(1);
    } else {
        // Parent process: wait for the child to finish
        wait(NULL);
    }
}


