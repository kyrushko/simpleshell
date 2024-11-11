#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alias.h"
#include "history.h"

// Array to store aliases
Alias aliases[MAX_ALIASES];

void createOrOpenAliasesFile()
{
    FILE *file;
    char *filename = ".aliases";
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Alias File does not exist. Creating...\n");

        file = fopen(filename, "w");

        if (file == NULL)
        {
            printf("Error creating Alias file!\n");
            return;
        }
        fclose(file);
    }
    else
    {
        fclose(file);
        getAliases();
    }
}

// Read aliases from file
void getAliases()
{
    FILE *file;
    file = fopen(".aliases", "r");

    if (file == NULL)
    {
        printf("Failed to open the Alias file.\n");
        return;
    }
    char line[MAX_INPUT_SIZE];
	
	int count = 0;
    while (fgets(line, sizeof(line), file) != NULL && count<=MAX_ALIASES )
    {
        if (strlen(line) == 1 && line[0] == '\n')
        {
            continue;
        }

        char *string[MAX_INPUT_SIZE];
        int commSize = tokenize(line, string);
        char *command = (char *)malloc(MAX_INPUT_SIZE);
        strcpy(command, "");
        for (int y = 1; y < commSize; y++)
        {
            strcat(command, string[y]);
            if (y != commSize - 1)
                strcat(command, " ");
        }
        if (strcmp(command, "") != 0 && strcmp(string[0], "") != 0)
        {

            addAlias(string[0], command);
			count++;
        }
    }

    fclose(file);
}

// Write aliases to file
void writeAliases(char *hm)
{
    FILE *file;
    char *filename = "/.aliases";
    char result[MAX_INPUT_SIZE];

    sprintf(result, "%s%s", hm, filename); //Concatenation of strings

    file = fopen(result, "w");

    if (file == NULL)
    {
        printf("Error writing to Alias file!\n");
        return;
    }

    for (int x = 0; x < MAX_ALIASES; x++)
    {
        if (aliases[x].name != NULL)
        {	// Write alias into the file
            fprintf(file, "%s %s\n", aliases[x].name, aliases[x].command);
        }
    }

    fclose(file);
}

// Find the index for the next available alias slot
int findNextAlias()
{
    for (int y = 0; y < MAX_ALIASES; y++)
    {
        if (aliases[y].name == NULL)
        {
            return y;
        }
    }

    return -1;
}

// Find if an alias with the given name already exists
int findDuplicateAlias(char *name)
{
    for (int y = 0; y < MAX_ALIASES; y++)
    {
        if (aliases[y].name != NULL && strcmp(aliases[y].name, name) == 0)
        {
            return y;
        }
    }

    return -1;
}

// Add a new alias or update an existing one
void addAlias(char *name, char *command)
{
    int index = findDuplicateAlias(name);
    
    if (index != -1)
    {
        free(aliases[index].name);
        free(aliases[index].command);
    }
    else
    {
        index = findNextAlias();
    }

    if (index != -1)
    {
        aliases[index].name = strdup(name);
        aliases[index].command = strdup(command);
        printf("Alias added: %s -> %s\n", name, command);
    }
    else
    {
        printf("No more aliases can be added.\n");
    }
}

    
void checkforAlias(char **string, int commSize, int substitutions,int *maxSubsExceededFlag) {
    if (strcmp(string[0], "unalias") != 0 && strcmp(string[0], "alias") != 0) {
        if (substitutions > MAX_SUBSTITUTIONS && !(*maxSubsExceededFlag)) {
            printf("Maximum substitutions exceeded. Possible circular alias dependency.\n");
			*maxSubsExceededFlag = 1; // Set the flag to indicate the message has been printed
            return;
        }else if (*maxSubsExceededFlag){
			return;
		}

    
        int substitutionMade = 0; // indicate if any substitution is made
        for (int comm = 0; comm < commSize; comm++) {
            int aliasIndex = findDuplicateAlias(string[comm]);
            if (aliasIndex != -1) {
                string[comm] = strdup(aliases[aliasIndex].command);
                substitutionMade = 1; //indicate a substitution is made
				substitutions++;
				//make sure output is tokenized when replaced with history
                if (checkifHistory(*string, string, commSize)==0){
					commSize = tokenize(*string, string);
				};
            }
        }

        // If no substitutions were made, break out of the function
        if (substitutionMade == 0) {
            return;
        }
    

        // Recursive call for further substitutions
        checkforAlias(string, commSize, substitutions,maxSubsExceededFlag);
    }
}

void printAliases() {
    if (aliases[0].name != NULL) {
        for (int y = 0; y < MAX_ALIASES; y++) {
            if (aliases[y].name != NULL) {
                printf("Alias: %s -> %s\n", aliases[y].name, aliases[y].command);
            }
        }
    } else {
        printf("No aliases found :(\n");
    }
}

void removeAlias(char *name) {
	int count = 0;
	for (int i = 0;i < MAX_ALIASES;i++){
		if(aliases[i].name==NULL){
			count++;
		}
	}
	
	if (count==MAX_ALIASES){
		printf("No Aliases in List!\n");
		return;
	}
	
    int y = findDuplicateAlias(name);
    if (y != -1) {
        free(aliases[y].name);
        free(aliases[y].command);
        aliases[y].name = NULL;
        aliases[y].command = NULL;
        printf("Alias '%s' removed.\n", name);
    } else {
        printf("No alias: %s\n", name);
    }
}


// Handle the addition of a new alias in shell
void handleAddCommand(char **string, int commSize) {
    if (findDuplicateAlias(string[1]) != -1) {
        printf("Alias '%s' already exists. Overwriting...\n", string[1]);
    }

    char *command = (char *)malloc(MAX_INPUT_SIZE);
    strcpy(command, "");
    for (int y = 2; y < commSize; y++) {
        strcat(command, string[y]);
        if (y != commSize - 1)
            strcat(command, " ");
    }

    addAlias(string[1], command);
}
