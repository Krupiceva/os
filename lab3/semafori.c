#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

typedef struct
{
    char M[5];
} structure;
structure *message;
int *INPUT;
int *OUTPUT;
int ID;
int SemId;


int WRITE=1;
int FULL=2;
int EMPTY=3;



void SemGet(int n)
{
    SemId = semget(IPC_PRIVATE, n, 0600);
    if(SemId == -1)
    {
        printf("Cannot get semafors\n");
        exit(1);
    }
}

int SemSetVal(int SemNum, int SemVal)
{
    return semctl(SemId, SemNum, SETVAL, &SemVal);
}

int SemOp(int SemNum, int SemOp)
{
    struct sembuf SemBuf;
    SemBuf.sem_num = SemNum;
    SemBuf.sem_op = SemOp;
    SemBuf.sem_flg = 0;
    return semop(SemId, &SemBuf, 1);
}

void SemRemove(void)
{
    (void) semctl(SemId, 0, IPC_RMID, 0);
}

void DeleteMem (void)
{
    (void) shmdt((char *) INPUT);
    (void) shmdt((char *) OUTPUT);
    (void) shmdt((char *) message);
    (void) shmctl(ID, IPC_RMID, NULL);
    exit(0);
}

void Producer(int number, char *string)
{
    int i;
    for(i=0; string[i]!='\0'; i++)
    {
        SemOp(FULL,-1);
        SemOp(WRITE,-1);
        message->M[*INPUT]=string[i];
        printf("Producer%d->%c\n", number, string[i]);
        *INPUT=(*INPUT+1)%5;
        SemOp(WRITE,1);
        SemOp(EMPTY,1);
        sleep(1);
    }
    SemOp(FULL,-1);
    SemOp(WRITE,-1);
    message->M[*INPUT]=string[i];
    *INPUT=(*INPUT+1)%5;
    SemOp(WRITE,1);
    SemOp(EMPTY,1);
    sleep(1);
}
void Consumer(int numberOfProducer)
{
    char string[100];
    int i=0;
    int j=0;
   // sleep(1);
    while(j<numberOfProducer)
    {
        SemOp(EMPTY,-1);
        if(message->M[*OUTPUT]=='\0') j++;
        else
        {
            string[i]=message->M[*OUTPUT];
            printf("Consumer <- %c\n", string[i]);
            *OUTPUT=(*OUTPUT+1)%5;
            i++;

        }
        SemOp(FULL,1);
        sleep(1);
    }
    printf("Word: %s\n", string);
}

int main(int argc, char *argv[])
{
    int i;
    SemGet(3);
    SemOp(FULL,1);
    SemOp(WRITE,1);

    ID=shmget(IPC_PRIVATE,sizeof(int)*120,0600);
    if(ID==-1)
        exit (1);
    INPUT=(int*)shmat(ID,NULL,0);
    *INPUT=0;
    OUTPUT=(int*)shmat(ID,NULL,0)+1;
    *OUTPUT=0;
    message=(structure*)shmat(ID,NULL,0)+2;

    sigset(SIGINT,DeleteMem);
    sigset(SIGINT,SemRemove);

    for(i=1; i<=argc; i++)
    {
        if(fork()==0)
        {
            Producer(i, argv[i]);
            exit(0);
        }
    }
    if(fork()==0)
    {
        Consumer(argc);
        exit(0);
    }

    for(i=0; i<=argc; i++)
    {
        wait(NULL);
    }
    SemRemove();
    DeleteMem();

    return 0;
}



