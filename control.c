#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


union semun{
  int val;
  struct semid_ds * buf;
  unsigned int array;
  struct seminfo *__buf;
};

int main( int argc, char * argv[] ){
  if ( argc < 2 ){
    printf(" Failed to include enough args \n" );
    return 0;
  }else{
    union semun su;
    int semid, shmid, fd;
    int val;
    int key = ftok( "file", 22 );
    
    if( strcmp( argv[1], "-c" ) == 0 ){
      umask( 0000 );
      semid = semget( key, 1, IPC_CREAT | IPC_EXCL | 0664 );
      printf("Semaphore Created [%d]\n", semid);
      
      shmid = shmget( key, sizeof( int), IPC_CREAT | IPC_EXCL | 0664 );
      printf("Shared Mem Created [%d]\n", shmid);

      /*
      su.val = shmid;
      val = semctl( semid, 0, SETVAL, su);
      printf("Setting Value [%d]\n", val);      
      */      

      fd = open( "file", O_TRUNC | O_CREAT, 0664 );
      printf("File Made [%d]\n", fd);

      int * address;
      address = shmat( shmid, 0, 0 );
      printf(" Pointer : [%p]\n", address );
      *address = 0;
      val = shmdt( address );

      printf("Change of Shared Mem : [%d]\n", val);
      
     
    }else if( strcmp( argv[1], "-r" ) == 0 ){

      semid = semget( key, 1, 0 );
      val = semctl( semid, 0, IPC_RMID );
      printf(" Removing the Semaphore: [%d]\n", val);

      shmid = shmget( key, 1, 0 );
      val = shmctl( shmid, IPC_RMID, 0 );
      printf(" Removin Shared Mem: [%d]\n", val);

      fd = open("file", O_RDONLY, 0664 );
      char string[100000];
      val = read( fd, string, sizeof( string ) );
      printf(" Reading File : [%d]\n", val);
      printf(" File Output:\n %s\n", string );
      
      
    }else{
      printf("You done goofed\n");
    }
  }
}
