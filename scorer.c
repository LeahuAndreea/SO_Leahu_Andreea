#include "biblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <bits/fcntl-linux.h>

#define NUME_SIZE 100
#define PATH 100



typedef struct ScorInspector
{
    char nume[NUME_SIZE];
    int score;
}ScorInspector;

int main (int argc, char *argv[])
{
    if (argc<2)
    {
        printf("eroare la argumente, lipseste districtul\n");
        return 1;
    }

    char path [PATH];
    snprintf(path,O_RDONLY);
    if ( f==-1)
    {
        printf("Scorer [%s] nu a putut fi gasit",argv[1]);
        return 1;
    }

    ScorInspector scoruri[100];
    int n_inspectori=0;
    REPORT r;

    int exista,i;
    while (read(f,&r,sizeof(REPORT))>0) {
        exista=0;
        for (i=0;i<n_inspectori;i++)
        {
            if (strcmp(scoruri[i].nume,r.inspectorName)==0)
            {
                scoruri[i].score=scoruri[i].score+r.severity;
                exista=1;
                break;
            }
        }

        if (exista==0 && n_inspectori<100)
        {
            strcpy(scoruri[n_inspectori].nume,r.inspectorName);
            scoruri[n_inspectori].score=r.severity;
            n_inspectori=n_inspectori+1;
        }
    }

    close(f);

    printf("Raport workload district: %s \n",argv[1]);
    for (i=0;i<n_inspectori;i++)
    {
        printf("inspector: %s \t scor total: %d\n", scoruri[i].nume,scoruri[i].score);
    }
    return 0;
}