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
    int cmpLen = strlen(lineCmp);
    int col = 0;
    while ((c = getc(fp)) != EOF) {
        if(c == '\n') {
            if (col != -1) {
                return true;
            }
            col = 0;
        } else {
            if (col != -1 && col < cmpLen && lineCmp[col] == c) {
                col++;
            } else {
                col = -1;
            }
        }
    }
    return false;
}

int num_lines_in_file(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    int c = 0;
    int numLines = 0;
    while ((c = getc(fp)) != EOF) {
        if(c == '\n') {
            numLines++;
        }
    }
    fclose(fp);
    return numLines;
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
        char **matchLines = malloc(num_lines_in_file(TODO_FILENAME) * sizeof(char*));
        while ((c = getc(fp)) != EOF) {
            if(c == '\n') {
                if(match && size >= len) {
                    matchLineNum = lineNum;
                    if(size == len) { // exact match
                        for(int i = 0; i < numMatches; i++) {
                            free(matchLines[i]);
                        }
                        matchLines[0] = line;
                        numMatches = 1;
                        break;
                    } else {   
                        matchLines[numMatches] = line;                     
                        numMatches++;
                    }
                }
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
        fclose(fp);

        for(int i = 0; i < numMatches; i++) {
            printf("%s\n", matchLines[i]);
            free(matchLines[i]);
        }
        free(matchLines);

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
        int lineNum = 1;
        printf("1. ");
        int c = getc(fp);
        while (c != EOF) {
            putchar(c);
            int tmp = getc(fp);
            if(c == '\n' && tmp != EOF) {                
                lineNum++;       
                printf("%d. ", lineNum);
            }
            c = tmp;
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