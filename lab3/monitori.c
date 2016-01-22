#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

pthread_mutex_t monitor=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t FULL=PTHREAD_COND_INITIALIZER;
pthread_cond_t EMPTY=PTHREAD_COND_INITIALIZER;

int INPUT=0;
int OUTPUT=0;
char Buffer[5];
int numberOfFreeSpaceInBuffer=5;
int producerWait=0;
int consumerWait=0;
char *messageArray[50];
int lengthOfString=0;


void ProduceMessage(int j)
{
    char string[100];
    int i;
    strcpy(string,messageArray[j]);
    lengthOfString=lengthOfString+strlen(string);

    for(i=0; string[i]!='\0'; i++)
    {
        pthread_mutex_lock(&monitor);

        while(numberOfFreeSpaceInBuffer==0)
        {
            producerWait=1;
            pthread_cond_wait(&FULL,&monitor);
        }

        numberOfFreeSpaceInBuffer--;
        Buffer[INPUT]=string[i];
        INPUT=(INPUT+1)%5;
        printf("Producer%d -> %c\n", j+1, string[i]);

        if(consumerWait==1)
        {

            consumerWait=0;
            pthread_cond_broadcast(&EMPTY);
            pthread_mutex_unlock(&monitor);
        }

        else
        {
            pthread_mutex_unlock(&monitor);
        }


        sleep(1);
    }
}


void AcceptMessage(int numberOfProducer)
{
    char string[100];
    int i=0;
    int j=0;

pthread_mutex_lock(&monitor);

    while(i<lengthOfString)
    {

        while(numberOfFreeSpaceInBuffer==5)
        {

            consumerWait=1;
            pthread_cond_wait(&EMPTY,&monitor);

        }
        string[i]=Buffer[OUTPUT];
        printf("Consumer <- %c\n", string[i]);
        OUTPUT=(OUTPUT+1)%5;
        i++;
        numberOfFreeSpaceInBuffer++;

        if(producerWait==1)
        {

            producerWait=0;
            pthread_cond_broadcast(&FULL);
            pthread_mutex_unlock(&monitor);
        }

        else
        {
            pthread_mutex_unlock(&monitor);
        }

        sleep(1);
    }
    string[i]=='\0';
    printf("Word: %s\n", string);
}


void *Producer(void *i)
{
    int index=*((int*)i);
    ProduceMessage(index);

}

void *Consumer (void *i)
{
    int numberOfProducer=*((int*)i);
    AcceptMessage(numberOfProducer);
}


int main(int argc, char **argv)
{
    pthread_mutex_t monitor=PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t FULL=PTHREAD_COND_INITIALIZER;
    pthread_cond_t EMPTY=PTHREAD_COND_INITIALIZER;
    int i=0;
    pthread_t threadP[argc];

    for(i=0; i<argc-1; i++)
    {
        messageArray[i]=argv[i+1];

        if(pthread_create(&threadP[i],NULL,(void* )Producer,(void*)&i)!=0)
        {
            printf("Cannot make new thread. \n");
            exit(1);
        }
    }


    if(pthread_create(&threadP[i],NULL,(void* )Consumer,(void*)&argc)!=0)
    {
        printf("Cannot make new thread. \n");
        exit(1);
    }

    for(i=0; i<argc; i++)
    {
        pthread_join(threadP[i], NULL);

    }

    return 0;
}
