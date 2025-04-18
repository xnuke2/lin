#include <stdio.h>
#include<stdlib.h>
#include <errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
main(){
    pid_t pid;
    int rv;
    setsid();
    switch (pid=fork())
    {
    case -1:
        perror("fork");
        exit(1);   
    case 0:
        
        printf(" CHILD: process potomok\n") ;
        printf(" CHILD: PID - %d\n", getpid());
        printf(" CHILD: PPID -- %d\n",getppid());
        printf(" CHILD: file write\n",system("ps -o pid,cmd,ppid,pgid,sid > ch.txt"));
        printf(" CHILD: exit\n");
        scanf(" %d");
        exit(rv);
    default:
    
        printf(" PARENT: PID - %d\n", getpid());
        printf(" PARENT: PID child-- %d\n",pid);
        printf(" PARENT: PPID - %d\n", getppid());
        //int* stat_loc;
        int* stat;
        printf(" PARENT: file write\n",system("ps -o pid,cmd,ppid,pgid,sid > out.txt"));
        wait(stat);
        printf(" PARENT: kod vosvrata -- %d\n",WEXITSTATUS(rv));
        printf(" PARENT: st - %d \n",stat);
        printf(" PARENT: exit\n");
    }
}
//bool printToFile()