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


int main(){
    printf("\n-------------->WELCOME 2 CHAT ROOM<-------------\n");
    int return_sem_post;
    int sem_waiting;   
    int counter = 0;   
    int value,valueold;
    int i=0;

    int fd = shm_open(BackingFile, O_RDWR, AccessPerms);  /* empty to begin */
    if (fd < 0) report_and_exit("Can't get file descriptor...");

    /* get a pointer to memory */
    caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
    			ByteSize,   /* how many bytes */
    			PROT_READ | PROT_WRITE, /* access protections */
    			MAP_SHARED, /* mapping visible to other processes */
    			fd,         /* file descriptor */
    			0);         /* offset: start at 1st byte */
    if ((caddr_t) -1 == memptr) report_and_exit("Can't access segment...");


    /* create a semaphore for mutual exclusion */
    sem_t* semptr = sem_open(SemaphoreName, /* name */
    			   O_CREAT,       /* create the semaphore */
    			   AccessPerms,   /* protection perms */
    			   0);            /* initial value */
    if (semptr == (void*) -1) report_and_exit("sem_open");
    while(1){
        sem_getvalue(semptr, &value);
        // start mutex to prevent any file opening
        if (!sem_wait(semptr)) { /* wait until semaphore != 0 */
          //lock the semaphore
          for (i = 0; i < strlen(memptr); i++)
            write(STDOUT_FILENO, memptr + i, 1); /* one byte at a time */
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
    munmap(memptr, ByteSize);
    close(fd);
    sem_close(semptr);
    unlink(BackingFile);


}