#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "alias.h"
#include "history.h"
#include "shell.h"

int main(void)
{
    char input[MAX_INPUT_SIZE];
    char *home = getenv("HOME");
    chdir(home);
    char *path = strdup(getenv("PATH"));
    createOrOpenFile(); // Creates if not exists
    createOrOpenAliasesFile();

    while (1)
    {
        display_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            strcpy(input, "exit");
        }
        else
        {
            if (strlen(input) == 1 && input[0] == '\n')
            {
                continue;
            }
			
            input[strcspn(input, "\n")] = '\0'; // Allows for exit
        }

        if (strlen(input) == 1 && input[0] == '\n')
        {
            printf("String is empty.\n");
        }

        int responseCode = runCommand(input, home, path);
        if (responseCode != 2)
            return responseCode;
		
		writeHistory(home);
		writeAliases(home);
    }

    return 0;
}
