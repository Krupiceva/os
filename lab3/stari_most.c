#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>
#include<time.h>

pthread_cond_t condition[2];
pthread_mutex_t monitor;
int cars=0;
int directionOnBridge=0;

void GetUpOnTheBridge(int direction)
{
    pthread_mutex_lock(&monitor);
    while(cars==3 || (directionOnBridge!=direction && cars>0))
    {
        pthread_cond_wait(&condition[direction],&monitor);
    }
    cars++;
    directionOnBridge=direction;
    pthread_mutex_unlock(&monitor);
}

void GetDownOffTheBridge(int direction)
{
    pthread_mutex_lock(&monitor);
    cars--;
    if(cars==0)
        pthread_cond_broadcast(&condition[1-direction]);
    else
        pthread_cond_signal(&condition[direction]);
    pthread_mutex_unlock(&monitor);
}

void *Car(void *i)
{
    int direction;
    srand((unsigned)time(NULL));
    sleep(1);
    direction=rand()%2;
    printf("Car %d wait for bridge in direction %d\n", (int)i, direction);
    sleep(1);
    GetUpOnTheBridge(direction);
    sleep(2);
    printf("Car %d passes the bridge\n", (int)i);
    sleep(1);
    GetDownOffTheBridge(direction);
    sleep(1);
    printf("Car %d is passed the bridge\n", (int)i);
    sleep(1);
}

int main()
{
    int n, i;
    printf("How many cars do you want? ");
    scanf("%d", &n);
    pthread_t threadCars[n];
    pthread_mutex_init (&monitor,NULL);
    pthread_cond_init (condition,NULL);

    for(i=0; i<n; i++)
    {
        if(pthread_create(&threadCars[i],NULL,(void* )Car,(void*)(i+1)))
        {
            printf("Cannot make new thread. \n");
            exit(1);
        }
    }

    for(i=0; i<n; i++)
        pthread_join(threadCars[i],NULL);

    return 0;
}
