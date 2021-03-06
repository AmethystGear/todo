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

// append the provided task with the provided date at the provided index of the provided file.
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

// searches for provided task in file. returns true if it exists.
// returns false otherwise.
bool contains_task(char *task, char *fileName) {
    FILE *fp = fopen(fileName, "r");
    int c;
    int cmpLen = strlen(task);
    int col = 0;
    bool pastTab = false;
    while ((c = getc(fp)) != EOF) {
        if(c == '\t') {
            if (col != -1 && col == cmpLen) {
                return true;
            }
            pastTab = true;
        } else if (!pastTab) {
            if (col != -1 && col < cmpLen && task[col] == c) {
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

// returns number of lines in provided file
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

struct Date {
    int month;
    int day;
    int year;
};

// returns true if the provided year is a leap year.
// returns false otherwise.
bool is_leap_year(int year) {
    if(year < 0) {
        return false;
    }
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

// returns number of days in provided month on provided year
// returns -1 if the month is not a valid month, or the year is not a valid year.
int days_in_month(int month, int year) {
    if(year < 0) {
        return -1;
    }
    if(month == 2 && is_leap_year(year)) {
        return 29;
    } else if(month == 2) {
        return 28;
    } else if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        return 31;
    } else if(month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else {
        return -1;
    }
}

// returns true if the date can represent a real calendar date
// return false otherwise.
bool is_valid_date(struct Date *date) {
    // month in valid range
    if(date->month > 12 || date->month < 1) {
        return false;
    }
    // day in valid range (also checks year since days_in_month returns -1 if an invalid year is given)
    int days = days_in_month(date->month, date->year);
    if(days == -1 || date->day > days || date->day < 1) {
        return false;
    }
    return true;
}

// return 0 if the dates are exactly the same.
// return -1 if the a comes earlier than b.
// return 1 if a comes after b.
int compare_dates(struct Date *a, struct Date *b) {
    if(a->day == b->day && a->month == b->month && a->year == b->year) {
        return 0;
    } else if(
             (a->year < b->year) || 
             (a->year == b->year && a->month < b->month) || 
             (a->year == b->year && a->month == b->month && a->day < b->day)) 
    {
        return -1;
    } else {
        return 1;
    }
}

// parses date from provided string.
// if the provided string is a valid date
// (that is, in the format mm/dd or mm/dd/yyyy and represents a possible calendar date)
bool parse_date(char *str, struct Date *date) {
    int len = strlen(str);
    int firstSlash = -1;
    int secondSlash = -1;
    for(int i = 0; i < len; i++) {
        if(str[i] == '/' && firstSlash == -1) {
            firstSlash = i;
        } else if (str[i] == '/') {
            secondSlash = i;
            break;
        }
    }
    
    if(firstSlash == -1) {
        return false;
    } else {
        char monthBuff[3];
        char dayBuff[3];
        char yearBuff[5];

        if(firstSlash > 2) {
            return false;
        }
        memcpy(monthBuff, &str[0], firstSlash);
        monthBuff[2] = '\0';

        if(secondSlash != -1) {
            if(secondSlash - firstSlash - 1 > 2) {
                return false;
            }
            memcpy(dayBuff, &str[firstSlash + 1], secondSlash - firstSlash - 1);            
            if(len - secondSlash - 1 > 4) {
                return false;
            }
            memcpy(yearBuff, &str[secondSlash + 1], len - secondSlash - 1);
            yearBuff[4] = '\0';
        } else {
            if(len - firstSlash - 1 > 2) {
                return false;
            }
            memcpy(dayBuff, &str[firstSlash + 1], len - firstSlash - 1);
        }
        dayBuff[2] = '\0';

        int possibleMonth = -1;
        int possibleDay = -1;
        int possibleYear = -1;

        possibleMonth = atoi(monthBuff);
        possibleDay = atoi(dayBuff);
        if(secondSlash != -1) {
            possibleYear = atoi(yearBuff);
        } else {
            time_t s = time(NULL);
            struct tm *currentTime;
            currentTime = localtime(&s);
            struct Date currDate = {
                .day = currentTime->tm_mday, 
                .month = currentTime->tm_mon + 1, 
                .year = currentTime->tm_year + 1900
            };
            struct Date possibleDate = {
                .day = possibleDay, 
                .month = possibleMonth, 
                .year = currentTime->tm_year + 1900
            };
            if(compare_dates(&possibleDate, &currDate) == -1) {
                possibleYear = possibleDate.year + 1;
            } else {
                possibleYear = possibleDate.year;
            }
        }

        date->day = possibleDay;
        date->month = possibleMonth;
        date->year = possibleYear;

        if(!is_valid_date(date)) {
            return false;
        }
        return true;
    }
}

int days_between_dates(struct Date *start, struct Date *finish) {
    if(compare_dates(start, finish) == 1) {
        return -days_between_dates(finish, start);
    }
    if(finish->year > start->year) {
        int totalDays = days_in_month(start->month, start->year) - start->day;
        for(int i = start->month + 1; i <= 12; i++) {
            totalDays += days_in_month(i, start->year);
        }
        for(int i = start->year + 1; i < finish->year; i++) {
            totalDays += is_leap_year(i) ? 366 : 365;
        }
        for(int i = 1; i < finish->month; i++) {
            totalDays += days_in_month(i, finish->year);
        }
        totalDays += finish->day;
        return totalDays;
    } else if (finish->month > start->month) {
        int totalDays = days_in_month(start->month, start->year) - start->day;
        for(int i = start->month + 1; i < finish->month; i++) {
            totalDays += days_in_month(i, start->year);
        }
        totalDays += finish->day;
        return totalDays;
    } else {
        return finish->day - start->day;
    }
}

// calculate the days from now until the provided date.
// if the date is earlier than the current date, returns negative days.
int days_till_date(struct Date *date) {
    time_t s = time(NULL);
    struct tm *currentTime;
    currentTime = localtime(&s);

    struct Date currDate = {
        .day = currentTime->tm_mday, 
        .month = currentTime->tm_mon + 1, 
        .year = currentTime->tm_year + 1900
    };

    return days_between_dates(&currDate, date);
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
            printf(" {task to add}, or {path to todo file} ");
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
            struct Date date;
            if(!parse_date(argv[4], &date)) {
                printf("you didn't enter the date properly! You should enter it as mm/dd or mm/dd/yyyy\n");
                exit(1);
            }
            char date_formatted[11];
            sprintf(date_formatted, "%d/%d/%d", date.month, date.day, date.year);

            if (num_lines_in_file(TODO_FILENAME) == 0) {
                FILE *fp = fopen(TODO_FILENAME, "a");
                fprintf(fp, "%s\t", argv[3]);  
                fprintf(fp, "%s\n", date_formatted);                        
                fclose(fp);
            } else {
                FILE *fp = fopen(TODO_FILENAME, "r");
                int c = 0;
                int numLines = 0;
                bool pastTab = false;
                bool inserted = false;
                int index = 0;
                char str_date[11];
                while ((c = getc(fp)) != EOF) {
                    if(c == '\t') {
                        pastTab = true;
                    }
                    else if(c == '\n') {
                        str_date[index] = '\0';
                        struct Date lineDate;
                        if(strlen(str_date) != 0 && !parse_date(str_date, &lineDate)){
                            printf("the provided file is in the wrong format!\n");
                            exit(1);
                        } else if (strlen(str_date) == 0 || days_till_date(&lineDate) > days_till_date(&date)) {
                            insert_task(numLines, argv[3], date_formatted, TODO_FILENAME);
                            inserted = true;
                            break;
                        }
                        pastTab = false;
                        numLines++;
                        index = 0;
                    }
                    else if(pastTab) {
                        str_date[index] = c;
                        index++;
                    }
                }
                fclose(fp);
                if(!inserted) {
                    FILE *fp = fopen(TODO_FILENAME, "a");
                    fprintf(fp, "%s\t", argv[3]);
                    fprintf(fp, "%s\n", date_formatted);                        
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
            exit(1);
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
                line[size] = '\0';
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
            int tmp = getc(fp);
            if(c == '\n' && tmp != EOF) {                
                lineNum++;       
                printf("\n%d. ", lineNum);
            } else if(c == '\t' && tmp != '\n') {
                printf(": ");
            } else {
                putchar(c);
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
