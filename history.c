#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"
#include "alias.h"

// Array to store command history
char *history[MAX_ENTRIES];
// Total number of commands in history
int commandsCount = 0;
// Index for writing to history array
int i = 0;

void createOrOpenFile() {
    FILE *file;
    char *filename = ".hist_list";
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("History File does not exist. Creating...\n");
        file = fopen(filename, "w");
        if (file == NULL) {
            printf("Error creating History file!\n");
            return;
        }
        fclose(file);
    }
	else {
       fclose(file);
       getHistory();
    }
}

// Read command history from file
void getHistory() {
    FILE *file;
    file = fopen(".hist_list", "r");

    if (file == NULL) {
        printf("Failed to open the History file.\n");
        return;
    }
    char line[MAX_INPUT_SIZE];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) == 1 && line[0] == '\n') {
            continue;
        }   

        history[i] = malloc(strlen(line) + 1);
        strcpy(history[i], strtok(line,"\n"));

        i++;
        commandsCount++;
        i = i % MAX_ENTRIES;
    }

    fclose(file);
}

// Write command history to file
void writeHistory(char* home) {
    FILE *file;
    char *filename = ".hist_list";
    char result[MAX_INPUT_SIZE];

    sprintf(result, "%s/%s", home, filename); // Concatenate directory path with filename

    file = fopen(result, "w");

    if (file == NULL) {
        printf("Error writing to History file!\n");
        return;
    }

    int startIndex = i; // Start writing from the current index i

    for (int count = 0; count < MAX_ENTRIES; count++) {
        int index = (startIndex + count) % MAX_ENTRIES; // Calculate the actual index
        if (history[index] != NULL) {
            fprintf(file, "%s\n", history[index]);
        }
    }

    fclose(file);
}

int checkifHistory(char* input, char** string, int commSize){
    if (input[0] == '!') 
    {
        int index = 0;
		if (commandsCount==0){
			printf("Error - Empty History\n");
			return -1;
		}
        else if (input[1] == '!') 
        {
            if (commandsCount == 0)
            {
                printf("There is no last command saved.\n");
                return -1;
            }

            int actualIndex = (commandsCount - 1) % MAX_ENTRIES;
            //prevents malloc() invalid error
            char *temp = malloc(strlen(history[actualIndex]) + 1);
            strcpy(temp, history[actualIndex]);
            strcpy(input, temp);
            return 0;
        }
        else if (sscanf(input+1, "%d", &index) == 1)
        {
            if (index==0){
				printf("Invalid Zero Index\n");
				return -1;
			}
			else if (abs(index) > MAX_ENTRIES)
            {
                printf("History saves only last %d commands.\n", MAX_ENTRIES);
                return -1;
            }
            else if (abs(index) > commandsCount)
            {
                printf("History contains only %d entries.\n", commandsCount);
                return -1;
            }

            else if (index > 0)
            {
                // Calculate the actual index in the history array
                int actualIndex = index-1;
                
                // Copy the command from history to the input
                if (commandsCount > MAX_ENTRIES) {
                    actualIndex = ((commandsCount - 1) + index) % MAX_ENTRIES;
                }
                
                // Copy the command from history to the input
                //prevents malloc() invalid error
                char *temp = malloc(strlen(history[actualIndex]) + 1);
                strcpy(temp, history[actualIndex]);
                strcpy(input, temp);
                return 0;
            }
            else
            {
                // Calculate the actual index in the history array
                int actualIndex = (commandsCount + index) % MAX_ENTRIES;
                if (actualIndex < 0)
                    actualIndex += MAX_ENTRIES;
                
                // Copy the command from history to the input
                //prevents malloc() invalid error
                char *temp = malloc(strlen(history[actualIndex]) + 1);
                strcpy(temp, history[actualIndex]);
                strcpy(input, temp);
                return 0;
            }
        }
        else
        {
            printf("Wrong command format, it's supposed to be !%%d or !!.\n");
            return -1;
        }
    }
    return 2;
}

void clearHistory() {
    memset(history, 0, sizeof(history));
    i = 0;
    commandsCount = 0;
}

void printHistory() {
    int o = commandsCount;
    if (o > MAX_ENTRIES)
    {
        // Start from the 20th item if there are more than 20 history entries
        o = MAX_ENTRIES;
    }

    // Calculate the starting index for printing history
    int start_index = (i - o + MAX_ENTRIES) % MAX_ENTRIES;

    for (int x = 0; x < o; x++)
    {
        printf("%d %s\n", x + 1, history[(start_index + x) % MAX_ENTRIES]);
    }
}

void addToHistory(char *command) {
    if (command[0] != '!') {
        history[i] = malloc(strlen(command) + 1);
        strcpy(history[i], command);
        i = (i + 1) % MAX_ENTRIES;
        commandsCount++;
    }
}
