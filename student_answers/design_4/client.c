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

#define MaxLoops         12000   /* outer loop */
#define ChunkSize           16   /* how many written at a time */
#define IntsPerChunk         4   /* four 4-byte ints per chunk */
#define MaxZs              250   /* max microseconds to sleep */


void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

int main() {
  char buffer[100];
  char name [20];
  char chr_final[120];
  const char* pipeName = "./fifoChannel";
  mkfifo(pipeName, 0666);                      /* read/write for user/group/others */
  int fd = open(pipeName, O_CREAT | O_WRONLY); /* open as write-only */
  if (fd < 0) return -1;                      
  /* semaphore code to lock the shared mem */
  sem_t* semptr = sem_open(SemaphoreName, /* name */
			   O_CREAT,       /* create the semaphore */
			   AccessPerms,   /* protection perms */
			   0);            /* initial value */
  if (semptr == (void*) -1) report_and_exit("sem_open");
    printf("what is my name? = \n");
    scanf("%s",name);
  while(1){
    printf("-->");
    memset(chr_final, 0, 120);
    scanf("%s",buffer);
    strcat(chr_final,name);
    strcat(chr_final,":");
    strcat(chr_final,buffer);
    int k;
    char chunk[strlen(chr_final)];
    for (k = 0; k < strlen(chr_final); k++) 
	    chunk[k] =  chr_final[k];        
    write(fd, chunk, sizeof(chunk)); 
    usleep((rand() % MaxZs) + 1);           /* pause a bit for realism */
  
    printf("\n");
    if (sem_post(semptr) < 0) report_and_exit("sem_post");
  }
  return 0;
}