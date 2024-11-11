#ifndef historyHeader
#define historyHeader

#define MAX_ENTRIES 20
#define MAX_INPUT_SIZE 512

void createOrOpenFile();
void getHistory();
void writeHistory();
int checkifHistory(char* input, char** string, int commSize);
void addToHistory(char *command);
void clearHistory();
void printHistory();

#endif /* historyHeader */
