#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#define numberOfThreads 4
int pauseFlag = 0;
unsigned long number = 1000000007;
unsigned long lastNumber = 1000000007;
int ID[numberOfThreads] = {0,1,2,3};
pthread_t IDOfThreads[numberOfThreads];
int lastNumberLamport;
int getIn[numberOfThreads]={0};
int numbers[numberOfThreads]={0};

void PeriodicPrint ()
{
    printf("\nI'm a prime number: %lu\n", lastNumber);
}

void SetPause()
{
    int index;
    pauseFlag = 1 - pauseFlag;
    if(pauseFlag == 0)
    {
        for(index=0; index<numberOfThreads; index++)
        {
            pthread_kill(IDOfThreads[index], SIGUSR1);
        }
    }
}

void StopProgram ()
{
    printf("\nI'm a last prime number: %lu\n", lastNumber);
    exit(1);
}

int IsItPrime (unsigned long number)
{
    unsigned long div, maxSqrt;

    if((number & 1) == 0)
        return 0;
    maxSqrt = sqrt (number);
    for (div = 3; div <= maxSqrt; div += 2)
    {
        if((number % div) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void InCriticalSection(int index)
{
    int j;

    getIn[index] = 1;
    numbers[index] = lastNumberLamport +1;
    lastNumberLamport=numbers[index];
    getIn[index] = 0;

    for(j==0; j<numberOfThreads; j++)
    {
        while(getIn[j] !=0);
        while ((numbers[j] != 0) && (numbers[j] < numbers[index] || (numbers[j] == numbers[index] && j < index)));

    }
}

void OutCriticalSection(int index)
{
    numbers[index]=0;
}

void nothing()
{
    printf("Contine to work\n");
}

void* Counting (void *p)
{
    sigset(SIGUSR1, nothing);
    int index, isPrime;
    unsigned long myNumber;
    index=*((int*)p);

    while (index<numberOfThreads)
    {
        while(pauseFlag == 1)
        pause();
        InCriticalSection (index);

        myNumber = number;
        number++;

        OutCriticalSection(index);

        isPrime = IsItPrime(myNumber);

        InCriticalSection(index);

        if(((isPrime==1) && (myNumber>lastNumber)))
        {
            lastNumber=myNumber;
        }
        OutCriticalSection(index);
    }

}





int main ()
{
    int index;
    struct itimerval t;

    sigset(SIGALRM, PeriodicPrint);
    sigset(SIGINT, SetPause );
    sigset(SIGTERM, StopProgram);

    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = 500000;

    t.it_interval.tv_sec = 5;
    t.it_interval.tv_usec = 500000;

    setitimer (ITIMER_REAL, &t, NULL);

    for(index=0; index<numberOfThreads; index++)
    {
        if(pthread_create(&IDOfThreads[index], NULL, (void*) Counting, (void *)&ID[index]))
        {
             printf("Cannot create thread");
             exit(1);
        }
    }

    for(index=0; index<numberOfThreads; index++)
        pthread_join(IDOfThreads[index], NULL);
}
