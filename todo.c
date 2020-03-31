#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#define TODO_FILENAME "TODO"

#define ADD_COMMAND "add"
#define REMOVE_COMMAND "finish"
#define LIST_COMMAND "list"
#define REMOVE_ALL_COMMAND "clear"

void delete_line(int lineNumToDel, char *fileName) {
    FILE *fp = fopen(fileName, "r");
    FILE *tmp = fopen("temp", "w");

    int c;
    int lineNum = 0;
    while ((c = getc(fp)) != EOF) {
        if(lineNum != lineNumToDel) {
            fputc(c, tmp);
        }
        if(c == '\n') {
            lineNum++;
        }
    }

    fclose(fp);
    fclose(tmp);

    remove(fileName);
    rename("temp", fileName);    
}

bool contains_line(char *lineCmp, char *fileName) {
    FILE *fp = fopen(fileName, "r");
    int c;
    int size = 0;
    int capacity = 10;
    char *line = malloc(capacity * sizeof(char));
    while ((c = getc(fp)) != EOF) {
        if(c == '\n') {
            if(strcmp(line, lineCmp) == 0) {
                return true;
            }
            free(line);
            size = 0;
            capacity = 10;
            line = malloc(capacity * sizeof(char));
        } else {
            line[size] = c;
            size++;
            if(size >= capacity) {
                capacity = capacity * 2;
                line = realloc(line, capacity);
            }
        }
    }
    free(line);
    return false;
}

int num_bytes_in_file(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    int c = 0;
    int numBytes = 0;
    while ((c = getc(fp)) != EOF) {
        numBytes++;
    }
    fclose(fp);
    return numBytes;
}

int main(int argc, char *argv[]) {
    if(access(TODO_FILENAME, F_OK) == -1) {
        fclose(fopen(TODO_FILENAME,"w"));
    }

    if(argc == 1 || argc > 3) {
        printf("incorrect number of args\n");
        exit(1);
    }

    if(strcmp(argv[1], ADD_COMMAND) == 0) {
        if(argc != 3) {
            printf("incorrect number of args\n");
            exit(1);
        }
        if(contains_line(argv[2], TODO_FILENAME)) {
            printf("that task is already listed!\n");
            exit(1);
        }
        FILE *fp = fopen(TODO_FILENAME, "a");
        fprintf(fp, "%s\n", argv[2]);
        fclose(fp);
    } else if (strcmp(argv[1], REMOVE_COMMAND) == 0) {
        if(argc != 3) {
            printf("incorrect number of args");
        }

        int len = strlen(argv[2]);

        FILE *fp = fopen(TODO_FILENAME, "r");
        int c;
        int size = 0;
        int capacity = 10;
        char *line = malloc(capacity * sizeof(char));

        int lineNum = 0;
        int matchLineNum = 0;
        int numMatches = 0;
        bool match = true;
        while ((c = getc(fp)) != EOF) {
            if(c == '\n') {
                if(match && size >= len) {
                    printf("%s", line);
                    printf("\n");
                    matchLineNum = lineNum;
                    numMatches++;
                }
                free(line);
                size = 0;
                capacity = 10;
                line = malloc(capacity * sizeof(char));
                match = true;
                lineNum++;
            } else {
                if(size < len && c != argv[2][size]) {
                    match = false;
                }
                line[size] = c;
                size++;
                if(size >= capacity) {
                    capacity = capacity * 2;
                    line = realloc(line, capacity);
                }
            }
        }
        free(line);
        fclose(fp);

        if(numMatches == 0) {
            printf("there were no matches for your task!\n");
            exit(1);
        } else if (numMatches == 1) {
            delete_line(matchLineNum, TODO_FILENAME);
            printf("was removed.\n");
        } else {
            printf("there were multiple matches, be more specific!\n");
            exit(1);
        }
    } else if (strcmp(argv[1], LIST_COMMAND) == 0) {
        if(argc != 2) {
            printf("incorrect number of args");
            exit(1);
        }
        FILE *fp = fopen(TODO_FILENAME, "r");
        int c;
        while ((c = getc(fp)) != EOF) {
            putchar(c);
        }
        fclose(fp);
        return 0; 
    } 
    else if (strcmp(argv[1], REMOVE_ALL_COMMAND) == 0) {
        if(argc != 2) {
            printf("incorrect number of args");
            exit(1);
        }
        fclose(fopen(TODO_FILENAME,"w"));
    } else {
        printf("Command not recognized. Either use ");
        printf(ADD_COMMAND);
        printf(", ");
        printf(REMOVE_COMMAND);
        printf(", ");
        printf(LIST_COMMAND);
        printf(", or ");
        printf(REMOVE_ALL_COMMAND);
        printf(".\n");
        exit(1);
    }
    return 0;
}