#ifndef shellHeader
#define shellHeader

void display_prompt();
int tokenize(char *input, char **output);
int runCommand(char* input, char* home, char* path);
void executeExternalCommand(char** string);
void changeDirectory(char** string, int commSize, char* home);
void setPath(char** string, int commSize);
void getPath(int commSize);

#endif /* shellHeader */