#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

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

void insert_task(int index, char *task, char* date, char *fileName) {
    FILE *fp = fopen(fileName, "r");
    FILE *tmp = fopen("temp", "w");

    int c;
    int lineNum = 0;
    if(lineNum == index) {
        fprintf(tmp, "%s\t", task);
        fprintf(tmp, "%s\n", date);
    }
    while((c = getc(fp)) != EOF) {
        fputc(c, tmp);
        if(c == '\n') {
            lineNum++;
            if(lineNum == index) {
                fprintf(tmp, "%s\t", task);
                fprintf(tmp, "%s\n", date);
            }
        }
    }
    fclose(fp);
    fclose(tmp);

    remove(fileName);
    rename("temp", fileName);   
}

bool contains_task(char *lineCmp, char *fileName) {
    FILE *fp = fopen(fileName, "r");
    int c;
    int cmpLen = strlen(lineCmp);
    int col = 0;
    bool pastTab = false;
    while ((c = getc(fp)) != EOF) {
        if(c == '\t') {
            if (col != -1 && col == cmpLen) {
                return true;
            }
            pastTab = true;
        } else if (!pastTab) {
            if (col != -1 && col < cmpLen && lineCmp[col] == c) {
                col++;
            } else {
                col = -1;
            }
        } else if (c == '\n') {
            pastTab = false;
            col = 0;
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

bool parse_date(char *date, int *month, int *day) {
    int len = strlen(date);
    int slash = -1;
    for(int i = 0; i < len; i++) {
        if(date[i] == '/') {
            slash = i;
            break;
        }
    }
    if(slash == -1) {
        return false;
    } else {
        char monthBuff[2];
        char dayBuff[2];
        memcpy(monthBuff, &date[0], slash);
        memcpy(dayBuff, &date[slash + 1], len - slash);

        int possibleMonth = atoi(monthBuff);
        int possibleDay = atoi(dayBuff);
        if(possibleDay > 31 || possibleDay < 1) {
            return false;
        }
        if(possibleMonth > 12 || possibleMonth < 1) {
            return false;
        }
        *month = possibleMonth;
        *day = possibleDay;
        return true;
    }
}

bool is_leap_year(int year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

int days_in_month(int month, int year) {
    if(month == 2 && is_leap_year(year)) {
        return 29;
    } else if(month == 2) {
        return 28;
    } else if(month == 1 || month == 3 || month == 7 || month == 8 || month == 10 || month == 12) {
        return 31;
    } else {
        return 30;
    }
}

int days_till_date(int month, int day) {
    time_t s = time(NULL);
    struct tm *currentTime;
    currentTime = localtime(&s);
    int currDay = currentTime->tm_mday;
    int currMonth = currentTime->tm_mon + 1;
    int currYear = currentTime->tm_year;

    if(currMonth == month && currDay == day) {
        return 0;
    }
    
    if(month > currMonth) {
        int daysLeftInMonth = days_in_month(currMonth, currYear) - currDay;
        int totalDays = daysLeftInMonth;
        for(int i = currMonth + 1; i < month; i++) {
            totalDays += days_in_month(i, currYear);
        }
        totalDays += day;
        return totalDays;
    } else if (month == currMonth && day > currDay) {
        return day - currDay;
    } else {
        int daysLeftInMonth = days_in_month(currMonth, currYear) - currDay;
        int totalDays = daysLeftInMonth;
        for(int i = currMonth + 1; i <= 12; i++) {
            totalDays += days_in_month(i, currYear);
        }
        for(int i = 1; i < month; i++) {
            totalDays += days_in_month(i, currYear + 1);
        }
        totalDays += day;
        return totalDays;
    }
}

int main(int argc, char *argv[]) {
    char *TODO_FILENAME = argv[1];
    if(access(TODO_FILENAME, F_OK) == -1) {
        fclose(fopen(TODO_FILENAME,"w"));
    }

    if(argc < 3 || argc > 5) {
        printf("incorrect number of args\n");
        printf("remember, the format is {path to todo file} {command} {command arguments (if any)}\n");
        exit(1);
    }

    if(strcmp(argv[2], ADD_COMMAND) == 0) {
        if(argc != 4 && argc != 5) {
            printf("incorrect number of args\n");
            printf("remember, the format is {path to todo file} ");
            printf(ADD_COMMAND);
            printf(" {task to add}, or {path to todo file}");
            printf(ADD_COMMAND);
            printf(" {task to add} {date}\n");
            exit(1);
        }
        if(contains_task(argv[3], TODO_FILENAME)) {
            printf("that task is already listed!\n");
            exit(1);
        }
        
        if(argc == 4) {
            FILE *fp = fopen(TODO_FILENAME, "a");
            fprintf(fp, "%s\t\n", argv[3]);                    
            fclose(fp);
        } else {
            if (num_lines_in_file(TODO_FILENAME) == 0) {
                FILE *fp = fopen(TODO_FILENAME, "a");
                fprintf(fp, "%s\t", argv[3]);  
                fprintf(fp, "%s\n", argv[4]);                        
                fclose(fp);
            } else {
                int month = -1;
                int day = -1;
                if(!parse_date(argv[4], &month, &day)) {
                    printf("you didn't enter the date properly! You should enter it as {month}/{day}.\n");
                    exit(1);
                }
                FILE *fp = fopen(TODO_FILENAME, "r");
                int c = 0;
                int numLines = 0;
                bool pastTab = false;
                bool inserted = false;
                int index = 0;
                char date[6];
                while ((c = getc(fp)) != EOF) {
                    if(c == '\t') {
                        pastTab = true;
                    }
                    else if(c == '\n') {
                        date[index] = '\0';
                        int lineMonth;
                        int lineDay;
                        if(strlen(date) != 0 && !parse_date(date, &lineMonth, &lineDay)){
                            printf("the provided file is in the wrong format!");
                            exit(1);
                        } else if (strlen(date) == 0 || days_till_date(lineMonth, lineDay) > days_till_date(month, day)) {
                            insert_task(numLines, argv[3], argv[4], TODO_FILENAME);
                            inserted = true;
                            break;
                        }
                        pastTab = false;
                        numLines++;
                        index = 0;
                    }
                    else if(pastTab) {
                        date[index] = c;
                        index++;
                    }
                }
                fclose(fp);
                if(!inserted) {
                    FILE *fp = fopen(TODO_FILENAME, "a");
                    fprintf(fp, "%s\t", argv[3]);  
                    fprintf(fp, "%s\n", argv[4]);                        
                    fclose(fp);
                }
            }
        }
    } else if (strcmp(argv[2], REMOVE_COMMAND) == 0) {
        if(argc != 4) {
            printf("incorrect number of args\n");
            printf("remember, the format is {path to todo file} ");
            printf(REMOVE_COMMAND);
            printf(" {task to remove}\n");
        }

        int len = strlen(argv[3]);

        FILE *fp = fopen(TODO_FILENAME, "r");
        int c;
        int size = 0;
        int capacity = 10;
        char *line = malloc(capacity * sizeof(char));

        int lineNum = 0;
        int matchLineNum = 0;
        int numMatches = 0;
        bool match = true;
        bool pastTab = false;
        char **matchLines = malloc(num_lines_in_file(TODO_FILENAME) * sizeof(char*));
        while ((c = getc(fp)) != EOF) {
            if(c == '\t') {
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
                pastTab = true;
            } else if (c == '\n') {
                size = 0;
                capacity = 10;
                line = malloc(capacity * sizeof(char));
                match = true;
                pastTab = false;
                lineNum++;            
            } else if (!pastTab) {
                if(size < len && c != argv[3][size]) {
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
    } else if (strcmp(argv[2], LIST_COMMAND) == 0) {
        if(argc != 3) {
            printf("incorrect number of args\n");
            printf("remember, the format is {path to todo file} ");
            printf(LIST_COMMAND);
            printf("\n");
            exit(1);
        }
        FILE *fp = fopen(TODO_FILENAME, "r");
        int lineNum = 1;
        if(num_lines_in_file(TODO_FILENAME) > 0) {
            printf("1. ");
        }
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
    else if (strcmp(argv[2], REMOVE_ALL_COMMAND) == 0) {
        if(argc != 3) {
            printf("incorrect number of args\n");
            printf("remember, the format is {path to todo file} ");
            printf(REMOVE_ALL_COMMAND);
            printf("\n");
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