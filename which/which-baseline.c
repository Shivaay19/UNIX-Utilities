#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: which <command>\n");
        exit(1);
    }

    const char* command = argv[1];
    int i = 0;
    while(command[i] != '\0')
    {
        if(command[i] == '/')
        {
            if(access(command, X_OK) == 0)
            {
                printf("%s\n", command);
                return 0;
            }
            else
            {
                printf("%s not found\n", argv[1]);
                exit(1);
            }
        }
        ++i;
    }
    const char* path_variable = getenv("PATH");

    if(path_variable == NULL)
    {
        fprintf(stderr, "No such PATH exist\n");
        exit(1);
    }

    char* duplicatePathVariable = strdup(path_variable);
    char* directory = strtok(duplicatePathVariable, ":");

    while(directory != NULL)
    {
        char fullPath[1024];
        snprintf(fullPath, 1024, "%s/%s", directory, command);

        if(access(fullPath, X_OK) == 0)
        {
            printf("%s\n", fullPath);
            free(duplicatePathVariable);
            return 0;
        }
        directory = strtok(NULL, ":");
    }

    free(duplicatePathVariable);

    printf("%s not found\n", command);
    exit(1);
}
