#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    }+
    fflush(stderr);
}

//ta paidia ti tha kanoun re bro
int search_child(int offset, int length, char target){ //arage einai sosta auta p exo valei san orismata stin parenthesi?tha to mathoume alli stigmi 
    printf("Hello World, i am the child with PID %d and my parent has PID %d .\n", getpid(), getppid()); //apo tin alli ask
    char c2c = 'a';
    int count = 0;

    int fd=open(argv[1], O_RDONLY);
    if(fd<0){
        perror("the child could not open input.txt ");
        exit(1); //η return 1; ποια ειναι η διαφορα καν?
    }
    off_t start_of_each= lseek(fd, offset, SEEK_SET);
    printf("Start offset: %ld\n", (long)start_of_each);
    printf("the length of the bytes i should read is: %ld\n", length);
    if(fd==-1){
        perror("lseek of child didnt do a good job");
        exit(1);
    }
    char buff[length];
    int b_read;
    
  
    /* character to search for (third parameter in command line) */
    c2c=target;//prpei apo kapou na to paro to target eeeeeeee
    
    /*count the occurences of the given character*/
    while((b_read=read(fd,buff, start_of_each+length))>0){
        if(b_read==-1) {/*error*/
            perror("read");
            exit(1);
            }
    

        for(ssize_t i=0;i<b_read;i++){
            if(buff[i]==c2c){
                count++;
                }
        }
    }
    printf("Count: %d\n", count);
    return count; //pipe
    close(fd);
    exit(0);
}

int main(int argc,char **argv){  //den ksero an etsi prpei na paroume ta orismata

//orizo pragmatakia
int status;
pid_t p, mypid;
off_t filesize;
int children=3;

//πρωτα ανοιγω το αρχειο και βλεπω το μεγεθος του 
int fd=open(argv[1], O_RDONLY);
if(fd<0){
    perror("open");
    return 1;
    }
filesize= lseek(fd, 0, SEEK_END);
if(filesize==-1){
    perror("lseek didnt do a good job");
    return 1;
}
printf("Το μεγεθος του αρχειου ειναι %ld bytes\n", filesize); 
close(fd);

//υπολογιζω ποσο μεγαλο κομματι θα παρει το καθε παιδακι να ψαξει
int part_each_child=filesize/children;
printf("part_each_child: %d\n", part_each_child);

//kano fork ta paidakia mou
for(int i=0; i<children; i++){
    printf("i=%d\n", i); //AYTO DEN TO TΥΠΩΝΕΙ ΚΑΙ ΔΕΝ ΚΑΤΑΛΑΒΑΙΝΩ ΓΙΑΤΙ ΡΕ ΦΙΛΕ
    p=fork();
    if(p<0) {
        perror("fork");
        exit(1);
        }
    else if(p==0){
        mypid=getpid();
        printf("I am worker #%d with PID %d\n", i+1, getpid());
        
        //pame ligo na doume gia kathe paidi ti analambanei
        int offset=i*part_each_child;
        printf("offset before getting inside child %d\n", offset);
        int result=search_child(offset,part_each_child, argv[2][0]); //tsekare ligo ta orismata 
        sleep(3);
        printf("Result: %d\n", result);
        sleep(3); //ebala auto na doume ti fasoula
        exit(0); //πολυ σημαντικο λεει παρολα αυτα στην προηγουμενη ασκ το ειχαμε σκιπαρει και δεν ξερω γιατι 
        }
    else{
        mypid=getpid();
        printf("katafera na ftaso kai edo");//isos leei na balo kati edo alla tespa
        
        p=wait(&status);
        explain_wait_status(p, status);
        }



}
return 0;
}
//!!!!!!!!!!!!!!!
//sigoura pragmata pou den exoun ginei : 1) an den einai akeraia i diairesi den exo balei auto pou perisseuei se kapoion worker ara kati xanoume 2) pipe gia na stelnei to kathe child ston paent 3) ctrl+c 4) kapoio check re file posa paidia yparxoyn ti diabazoun ti metrane ti trexoun 
