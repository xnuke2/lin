#include <stdio.h>
#include<stdlib.h>
#include <errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <string>
main(){
    pid_t pid;
    int rv;
    int mypipe[2];

    
    if(pipe(mypipe)){
        fprintf(stderr,"Pipe failed.\n");
        //return EXIT_FAILURE;
    }
    switch (pid=fork())
    {
    case -1:
        perror("fork");
        exit(1);   
        break;
    case 0:
    {
        char* arr=new char[300];
        for(int i=0;i<300;i++){
            arr[i]=NULL;
        }
        close(mypipe[1]);
        printf(" CHILD: process potomok\n") ; 
        ssize_t count; 
        sleep(2);
        while(count= read(mypipe[0],arr,299)>0){
            arr[count]='\0';
            printf(" CHILD: %s\n",arr);
        }
        //printf(" CHILD: file write\n",system("ps -o pid,cmd,ppid,pgid,sid > ch.txt"));
        printf(" CHILD: Exit%n");
    }
    default:
        {
            //system("ps -o pid,cmd,ppid,pgid,sid");
            //printf("PARENT: exit\n");
        close(mypipe[0]);
        dup2(mypipe[1],STDOUT_FILENO);
        close(mypipe[1]);
        execlp("ps","ps","-l",NULL);
        //write(mypipe[1],&"aaaa",5);
        perror("ps fail");
        
        sleep(3);
        printf("PARENT: exit\n");
        exit(0);

    }
    }
}