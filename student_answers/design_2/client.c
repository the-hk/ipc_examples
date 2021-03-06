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

  int fd = shm_open(BackingFile,      /* name from smem.h */
		    O_RDWR | O_CREAT, /* read/write, create if needed */
		    AccessPerms);     /* access permissions (0644) */
  if (fd < 0) report_and_exit("Can't open shared mem segment...");

  ftruncate(fd, ByteSize); /* get the bytes */

  caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
			ByteSize,   /* how many bytes */
			PROT_READ | PROT_WRITE, /* access protections */
			MAP_SHARED, /* mapping visible to other processes */
			fd,         /* file descriptor */
			0);         /* offset: start at 1st byte */
  if ((caddr_t) -1  == memptr) report_and_exit("Can't get segment...");
  
  fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
  fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile );


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
    strcat(chr_final,"\n");
    strcpy(memptr, chr_final);
    printf("\n");
    if (sem_post(semptr) < 0) report_and_exit("sem_post");
  }
  munmap(memptr, ByteSize); /* unmap the storage */
  close(fd);
  sem_close(semptr);
  shm_unlink(BackingFile); /* unlink from the backing file */
  return 0;
}