#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>

int main( int argc, char * argv[]){
  int val;
  int semid, shmid, fd;
  int key = ftok( "file", 22 );
  struct sembuf sb;
  //printf("Flag 1 \n");
  semid = semget( key, 1, 0);
  //printf(" Semaphore ID: [%d]\n", semid );
  sb.sem_op = -1;
  sb.sem_num = 1;
  sb.sem_flg = SEM_UNDO;
  
  semop( semid, &sb, 1);
  printf(" INSIDE SEMAPHORE! [%d]\n", semid);

  shmid = shmget( key, sizeof(int), 0 );
  //printf(" INSIDE SHM! [%d]\n", shmid);
  int * address = shmat( shmid, 0, 0 );
  
  fd = open( "file", O_RDWR, 0666 );

  //printf("Length of prev times -1: [%d]\n", -1 * (*address) );//this give me -6.
  lseek( fd, -1 * ( *address ), SEEK_END );
  char string[10000];
  val = read( fd, &string, *address - 1 );
  printf("Last Line: [%s]\n", string );
  
  char string2[10000];
  printf("Type Next Line:");
  fgets(string2, sizeof(string2), stdin);

  //printf("String2: [%s]\n", string2);
  
  lseek( fd, 0, SEEK_END);
  val = write( fd, string2, strlen(string2));
  if( val < 0 ){
    printf("Errno[%s]\n", strerror(errno));
  }
  close( fd );
  
  *address = strlen(string2);

  val = shmdt( address );
  //printf("Detach Success[%d]\n", val);

  sb.sem_op = 1;
  semop( semid, &sb, 1);
  printf("Exited Semaphore!\n");

  
}
