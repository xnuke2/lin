#include <stdio.h>
#include<stdlib.h>
#include <errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <ctime>
time_t ent;
time_t st ;
static void catchSignal(int signo);

main()
{
    
    signal(SIGALRM,catchSignal);                                                                                                         

    alarm( 3);
    time(&st);
    sleep(5);

    printf("a\n");
    
}
static void catchSignal(int signo){
    time(&ent);
    double tm =difftime(ent,st);
    printf("time %f\n",tm);
    int a;
    exit(a);
}