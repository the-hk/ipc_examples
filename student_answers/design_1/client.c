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
    FILE *fp1 = fopen(shared_filename,"a");
    sprintf(buff,"%s\n",buff);
    fwrite(buff, strlen(buff), 1,fp1);
    fclose(fp1);
}

int main() {
  char buffer[100];
  char name [20];
  char chr_final[120];
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
    my_write(chr_final);
    printf("\n");
    if (sem_post(semptr) < 0) report_and_exit("sem_post");
  }
  return 0;
}