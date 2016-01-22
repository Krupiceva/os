#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>

int IDFind, IDNumber, IDTable; //ID of shared variables

int *find;
int *number;
int *table;

int maxNumber;

int numberOfProcess, numberOfTable;

void InCriticalSection(int process)
{
    int index;
    find[process] = 1;
    maxNumber = number[0]; //Put first number for max value

    for(index=0; index<numberOfProcess; index++) //find max value
    {
        if(maxNumber<number[index])
        {
            maxNumber = number[index];
        }
    }

    number[process] = maxNumber + 1;
    find[process] = 0;

    for(index=0; index<numberOfProcess; index++)
    {
        while(find[index] != 0);
        while(number[index]!=0 && (number[index]<number[process] || (number[index]==number[process] && index<process)));
    }
}

void OutCriticalSection(int process)
{
    number[process] = 0;
}

void PrintingOnScreen (int process, int numberOfProcessl, int numberOfTablel)
{
    int potentiallyFreeTable[numberOfTablel];
    int numberOfFreeTable=0;
    int numberOfTakenTable=0;
    int randomTable;
    int index;

    while(numberOfFreeTable>=0) //repeat for every process while there is free table
    {
        numberOfFreeTable=0;

        for(index=0; index<numberOfTablel; index++) //find all free table
        {
            if(table[index]==0)
            {
                potentiallyFreeTable[numberOfFreeTable]=index;
                numberOfFreeTable++;
            }
        }


        srand((unsigned)time(NULL));
        sleep(1);
        randomTable = rand() % numberOfFreeTable;

        printf("Process %d: I take table number %d\n", process, potentiallyFreeTable[randomTable]+1);

        sleep(4);
        InCriticalSection(process);

        if(table[potentiallyFreeTable[randomTable]]==0)  //check again if picked table is really free
        {
            table[potentiallyFreeTable[randomTable]] = process;
            printf("Process %d: I reserve table %d\n", process, potentiallyFreeTable[randomTable]+1);
            numberOfTakenTable++;

        }
        else
        {
            printf("Process %d: I can't reserve table %d\n", process, potentiallyFreeTable[randomTable]+1);
        }

        for(index=0; index<numberOfTablel; index++)
        {
            if(table[index]==0)
            {
                printf("- ");
            }
            else
            {
                printf("%d ", table[index]);
            }
        }
        printf("\n");
        OutCriticalSection(process);

        numberOfFreeTable--;
    }
}

void DeleteMemory()
{
    (void) shmdt((char *) number);
    (void) shmdt((char *) find);
    (void) shmdt((char *) table);

    (void) shmctl(IDNumber, IPC_RMID, NULL);
    (void) shmctl(IDFind, IPC_RMID, NULL);
    (void) shmctl(IDTable, IPC_RMID, NULL);

    exit(0);
}

int main (int argc, char *argv[])
{
    int index;
    numberOfProcess=atoi(argv[1]);
    numberOfTable=atoi(argv[2]);

    sigset(SIGINT, DeleteMemory);

    if((IDNumber = shmget(IPC_PRIVATE, sizeof(int)*100, 0600)) == -1)  //allocate memory
    {
        printf("Error: Can't create shared Memory");
        exit(1);
    }
    number = (int *) shmat(IDNumber, NULL,0);

    if((IDFind = shmget(IPC_PRIVATE, sizeof(int)*100, 0600)) == -1)
    {
        printf("Error: Can't create shared Memory");
        exit(1);
    }
    find = (int *) shmat(IDFind, NULL,0);

    if((IDTable = shmget(IPC_PRIVATE, sizeof(int)*100, 0600)) == -1)
    {
        printf("Error: Can't create shared Memory");
        exit(1);
    }
    table = (int *) shmat(IDTable, NULL,0);

    for(index=0; index<numberOfTable; index++) //initialize all tables
    {
        table[index]=0;
    }

    for(index=0; index<numberOfProcess; index++)
    {
        switch(fork())
        {
            case 0:
                PrintingOnScreen(index+1, numberOfProcess, numberOfTable);
                exit(0);
            case -1:
                printf("Can't create process");
                break;
            default:
                break;
        }
        sleep(1);
    }

    while(index--)
    {
        wait(NULL);
    }

    DeleteMemory();

    return 0;

}
