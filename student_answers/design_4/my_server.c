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
#include "shmem.h"


#define shared_filename "shared_file.txt"

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

void my_write(char buff []){
    FILE *fp1 = fopen(shared_filename,"w");
    fwrite(buff, strlen(buff), 3, fp1);
    fclose(fp1);
}

void my_read(int *counter){
    char temp;
    int local_counter = 0;
    // read data and print it to screen
    FILE *fp1 = fopen(shared_filename,"r");
    if(fscanf(fp1,"%c",&temp)==EOF){
        printf("file is empty\n");
    }else{
        if(counter==0){
            printf("%c",temp);
        }
        local_counter++;
        while(fscanf(fp1,"%c",&temp)!=EOF){
            if(local_counter>= *counter){
                local_counter++;
                if(temp=="\n"){
                    printf("\n");
                }else{
                    printf("%c",temp);
                }
            }else{
                local_counter++;
            }
    }
    fclose(fp1);
    }
    *counter=local_counter;
    
}



int main(){
    printf("\n-------------->WELCOME 2 CHAT ROOM<-------------\n");
    const char* file = "./fifoChannel";
    int fd = open(file, O_RDONLY); 
    char next[100];
    if (fd < 0) return -1; /* no point in continuing */
    unsigned count = 0, total = 0, primes_count = 0;
    int return_sem_post;
    int sem_waiting;   
    int counter = 0;   
    int value,valueold;
    /* create a semaphore for mutual exclusion */
    sem_t* semptr = sem_open(SemaphoreName, /* name */
    			   O_CREAT,       /* create the semaphore */
    			   AccessPerms,   /* protection perms */
    			   0);            /* initial value */
    if (semptr == (void*) -1) report_and_exit("sem_open");
    while(1){
        int ctr = 0;
        sem_getvalue(semptr, &value);
        // start mutex to prevent any file opening
        if (!sem_wait(semptr)) { /* wait until semaphore != 0 */
          //lock the semaphore
            memset(next, 0, 255);
            int i;
            ssize_t count = read(fd, next, sizeof(next));   
            printf("%s\n",next);
            if (0 == count) break;                  /* end of stream */
            else if (count == sizeof(int)) {        /* read a 4-byte int value */
              total++;
                }   
          // end mutex to let others open the  text        
          sem_post(semptr);
          sem_getvalue(semptr, &value);
        } 
        sem_getvalue(semptr, &value);
        valueold = value;
        while(valueold == value){
            ctr ++;
            //printf("hello : %d \n",ctr);
            sem_getvalue(semptr, &value);
            sleep(0.1);
        }
    }

close(fd);       /* close pipe from read end */
unlink(file);    /* unlink from the underlying file */
}