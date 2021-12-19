#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>     
#include <sys/msg.h>
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"
#include "shmem.h"


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

    key_t key= ftok(PathName, ProjectId); /* key to identify the queue */
    if (key < 0) report_and_exit("key not gotten...");
    
    int qid = msgget(key, 0666 | IPC_CREAT); /* access if created already */
    if (qid < 0) report_and_exit("no access to queue...");

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
          queuedMessage msg;
          if (msgrcv(qid, &msg, sizeof(msg), 1,MSG_NOERROR | IPC_NOWAIT) < 0)
            puts("msgrcv trouble...");
          printf("%s\n", msg.payload);
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

if (msgctl(qid, IPC_RMID, NULL) < 0)  /* NULL = 'no flags' */
    report_and_exit("trouble removing queue...");
}