#include <stdio.h>
#include <sys/stat.h>
#include<unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#define NAME_SIZE 50
#define CATEGORY_SIZE 30
#define DESCRIPTION_SIZE 100
#define PATH_SIZE 256
#define LINK_SIZE 200
#define LOG_SIZE 250

typedef struct {
    float latitude;
    float longitude;
}GPS;

typedef struct {
    int id;
    char inspectorName[NAME_SIZE];
    GPS gps;
    char category[CATEGORY_SIZE];
    int severity;
    time_t timestamp;
    char description[DESCRIPTION_SIZE];
}REPORT;