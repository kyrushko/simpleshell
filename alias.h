#ifndef aliasHeader
#define aliasHeader

#define MAX_ALIASES 10
#define MAX_SUBSTITUTIONS 3
#define MAX_INPUT_SIZE 512

typedef struct Alias
{
    char *name;
    char *command;
} Alias;

void createOrOpenAliasesFile();
void getAliases();
void writeAliases(char *hm);
int findNextAlias();
int findDuplicateAlias(char *name);
void addAlias(char *name, char *command);
void checkforAlias(char **string, int commSize, int substitutions,int *maxSubsExceededFlag);
int tokenize(char *input, char **output);
void printAliases();
void removeAlias(char *name);
void handleAddCommand(char** string, int commSize);

#endif /* ALIASES_H */
