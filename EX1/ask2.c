#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int char_count(int filed, char target){
 char c2c = 'a';
    int count = 0;
    char buff[BUFFER_SIZE];
    ssize_t b_read;
    char answer[256];




        /* character to search for (third parameter in command line) */
      c2c=target;

       /*count the occurences of the given character*/
     while((b_read=read(filed,buff,sizeof(buff)-1))>0){

           if(b_read==-1) {/*error*/
          perror("read");
          return 1;
         }

        for(ssize_t i=0;i<b_read;i++){
            if(buff[i]==c2c){
                count++;
                            }

                                      }
                                                        }

        return count;

                        }
                        int main(int argc,char **argv){

pid_t p, mypid;
mypid=-1;
int status;
int x=2;
int fd;

fd=open(argv[1],O_RDONLY);
        if (fd==-1){
            perror("open");
            exit(1);
        }
void explain_wait_status(pid_t pid, int status) {
    if (WIFEXITED(status)) {
        fprintf(stderr, "Child with PID = %ld terminated normally, exit status = %d\n", (long)pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stderr, "Child with PID = %ld was terminated by a signal, signo = %d\n", (long)pid, WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
        fprintf(stderr, "Child with PID = %ld has been stopped by a signal, signo = %d\n", (long)pid, WSTOPSIG(status));
    } else {
        fprintf(stderr, "%s: Internal error: Unhandled case, PID = %ld, status = %d\n", __func__, (long)pid, status);
        exit(0);//to ekana 0 sta slides leei 1
    }
    fflush(stderr);
}

void child(){
        x=6;
        printf("Hello World, i am the child with PID %d and my parent has PID %d .\n", getpid(), getppid());
        printf("CHILD x= %d\n", x);
        int count=char_count(fd,argv[2][0]);
        printf("Child counted: %d occurences\n", count);

        }

void father(){
        x=4;
        p=wait(&status);
        explain_wait_status(p, status);
        printf("Im the parent and my child has PID %d\n",p);
        printf("PARENT x= %d\n", x);


}
p=fork();
if(p<0) {
        perror("fork");
        exit(1);
        }
else if(p==0){
        mypid=getpid();
        child();
        }
else{
        mypid=getpid();
        father();
        }
return 0;
}
