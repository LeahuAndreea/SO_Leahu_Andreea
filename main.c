#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define inspSize 100
#define catSize 50
#define desSize 100

typedef struct
{
    float latitudine;
    float longitudine;
}GPS;

typedef struct
{
    int id;
    char inspectorName[inspSize];
    GPS gps;
    char category [catSize];
    int severity;
    time_t timestamp;
    char description[desSize];
}REPORT;



int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        char text[]="Too few arguments";
        write(2,text,sizeof(text));
        return 0;
    }
    if (strcmp(argv[2],"--role")!=0)
    {
        char text[]="Wrong syntax";
        write(2,text,sizeof(text));
        return 0;
    }
    return 0;
}