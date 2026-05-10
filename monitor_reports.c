#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>

#define PID_FILE ".monitor_pid"
#define text_size 100

void signal_handler(int sig)
{
    if (sig == SIGUSR1)
    {
        char text[text_size]="semnal SIGSUR. A fost adaugat un report\n";
        write(1,text,strlen(text));
    }
    else
    {
        if (sig=SIGINT)
        {
            char text[text_size]="semnal SIGINT. Se inchide monitorul\n";
            write(1,text,strlen(text));

            unlink(PID_FILE);
            exit(0);
        }
    }
}

int main(void)
{
    int f;
    f=open(PID_FILE,O_WRONLY|O_CREAT|O_TRUNC,0666);
    if (f==-1)
    {
        perror("eroare la crearea .monitor_pid");
        exit(1);
    }
    char pid_str[10];
    int len;
    len=snprintf(pid_str,sizeof(pid_str),"%d",getpid());
    write(f,pid_str,len);
    close(f);

    struct sigaction s;
    s.sa_handler=signal_handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags=SA_RESTART;

    if (sigaction(SIGUSR1,&s,NULL)==-1)
        perror("eroare la sigaction SIGUSR1\n");
    if (sigaction(SIGINT,&s,NULL)==-1)
        perror("eroare la sigaction SIGINT\n");

    while (1)
        pause();

    return 0;
}