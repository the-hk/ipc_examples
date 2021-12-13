#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//int shmget(key_t key, size_t size, int shmflg);
//void *shmget(int shmid, const void *shmaddr, int shmflg);

//shmget() is used to create shared memory segment
//shmat() is used to attach the shared segmetn with the address space of the processes

int main(){
    void *shared_memory;
    char buff[100];
    int shmid;

    shmid = shmget((key_t)1234,1024,0666|IPC_CREAT);
    printf("Key of share memory is %d\n",shmid);
    shared_memory = shmat (shmid,NULL,0);
    printf("Process attached to %d\n",shared_memory);
    printf("Enter some data to write to shared memory \n",shmid);
    read(0,buff,100);
    strcpy(shared_memory,buff);
    printf("You wrote : %s\n",(char *),shared_memory);
}