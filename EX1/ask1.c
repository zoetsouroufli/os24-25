#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define BUFFER_SIZE 1024

#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {


     char c2c = 'a';
     int count = 0;
     char buff[BUFFER_SIZE];
     ssize_t b_read;
     char answer[256];
     ssize_t written;

    /*open file for reading*/
     int fd;
    fd=open(argv[1],O_RDONLY);
    if (fd==-1){
        perror("open read");
        exit(1);
    }

    /*open file for writing*/
    int fw;
    fw=open(argv[2], O_WRONLY);
    if(fw<0){
        perror("open write");
        exit(1);
    }

    /* character to search for (third parameter in command line) */
    c2c=argv[3][0];

   /*count the occurences of the given character*/
     while((b_read=read(fd,buff,sizeof(buff)-1))>0){

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

            snprintf(answer, sizeof(answer), "The character '%c' appears %d times in file %s.\n", c2c, count, argv[1]);
int idx = 0;
int len = strlen(answer);

do {
    written = write(fw, answer + idx, len - idx);
    if (written == -1) {
        perror("write");
        return 1;
    }
    idx += written;
} while (idx < len);

 if (written == -1) {
        perror("write");
        return 1;
    }
        close(fw);
        close(fd);
    return 0;
}
