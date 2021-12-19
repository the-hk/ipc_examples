#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h> /* wait */
#include "shmem.h"

#define ReadEnd  0
#define WriteEnd 1

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}


int main(){
    printf("\n-------------->WELCOME 2 CHAT ROOM<-------------\n");
    char name[50],buffer[100];
    char chr_final[100];
    int pipeFDs[2];
    char buf;       /* 1-byte buffer */
    const char* msg = "Nature's first green is gold\n"; /* bytes to write */
    int return_sem_post;
    int sem_waiting;   
    int counter = 0;   
    int value,valueold;
    int i=0;


    sem_t* semptr2 = sem_open(SemaphoreName, /* name */
			   O_CREAT,       /* create the semaphore */
			   AccessPerms,   /* protection perms */
			   0);            /* initial value */
    /* create a semaphore for mutual exclusion */
    sem_t* semptr = sem_open(SemaphoreName, /* name */
    			   O_CREAT,       /* create the semaphore */
    			   AccessPerms,   /* protection perms */
    			   0);            /* initial value */
    if (semptr == (void*) -1) report_and_exit("sem_open");

     pid_t cpid = fork();                                /* fork a child process */
    if (cpid < 0) report_and_exit("fork");              /* check for failure */

    if (0 == cpid) {    /*** child ***/                 /* child process */
      close(pipeFDs[WriteEnd]);                         /* child reads, doesn't write */
      printf("what is my name? = \n");
      scanf("%s",name);
      while(1){
        printf("-->");
        memset(chr_final, 0, 120);
        scanf("%s",buffer);
        strcat(chr_final,name);
        strcat(chr_final,":");
        strcat(chr_final,buffer);
        strcat(chr_final,"\n");
        write(STDOUT_FILENO, &chr_final, sizeof(chr_final)); 
        printf("\n");
    if (sem_post(semptr2) < 0) report_and_exit("sem_post");
  }
      close(pipeFDs[ReadEnd]);                          /* close the ReadEnd: all done */
      _exit(0);                                         /* exit and notify parent at once  */
    }
    else {   
      pid_t cpid2 = fork();            /*** parent ***/
      if(cpid2==0){
        close(pipeFDs[WriteEnd]);                         /* child reads, doesn't write */
        printf("what is my name? = \n");
        scanf("%s",name);
        while(1){
          printf("-->");
          memset(chr_final, 0, 120);
          scanf("%s",buffer);
          strcat(chr_final,name);
          strcat(chr_final,":");
          strcat(chr_final,buffer);
          strcat(chr_final,"\n");
          write(STDOUT_FILENO, &chr_final, sizeof(chr_final)); 
          printf("\n");
      if (sem_post(semptr2) < 0) report_and_exit("sem_post");
  }
      close(pipeFDs[ReadEnd]);                          /* close the ReadEnd: all done */
      _exit(0);

      }else{
        while(1){
        sem_getvalue(semptr, &value);
        // start mutex to prevent any file opening
        if (!sem_wait(semptr)) { /* wait until semaphore != 0 */
          //lock the semaphore
          close(pipeFDs[ReadEnd]);                          /* parent writes, doesn't read */
          write(pipeFDs[WriteEnd], msg, strlen(msg));       /* write the bytes to the pipe */
          close(pipeFDs[WriteEnd]); 
          // end mutex to let others open the  text        
          sem_post(semptr);
          sem_getvalue(semptr, &value);
        } 
        sem_getvalue(semptr, &value);
        valueold = value;
        while(valueold == value){
            sem_getvalue(semptr, &value);
            sleep(0.1);
        }
      }
      }
      while(1){
        sem_getvalue(semptr, &value);
        // start mutex to prevent any file opening
        if (!sem_wait(semptr)) { /* wait until semaphore != 0 */
          //lock the semaphore
          close(pipeFDs[ReadEnd]);                          /* parent writes, doesn't read */
          write(pipeFDs[WriteEnd], msg, strlen(msg));       /* write the bytes to the pipe */
          close(pipeFDs[WriteEnd]); 
          // end mutex to let others open the  text        
          sem_post(semptr);
          sem_getvalue(semptr, &value);
        } 
        sem_getvalue(semptr, &value);
        valueold = value;
        while(valueold == value){
            sem_getvalue(semptr, &value);
            sleep(0.1);
        }
      }

      wait(NULL);                                       /* wait for child to exit */
      exit(0);                                          /* exit normally */
    }
    sem_close(semptr);
    unlink(BackingFile);


}