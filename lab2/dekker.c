#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

int turn = 1;
int flag[2] = {0};

void InCriticalSection (int i, int j)
{
    flag[i] = 1;
    while (flag[j] != 0)
    {
        if (turn == j)
        {
            flag[i] = 0;
            while (turn == j);
            flag[i] = 1;
        }
    }
}

void OutCriticalSection (int i, int j)
{
    turn = j;
    flag[i] = 0;
}

void PrintingOnScreen (int *i)
{
    int x, y, j;
    if(*i==0) j=1;
    if(*i==1) j=0;

    for(x=1; x<=5; x++)
    {
        InCriticalSection(*i, j);
        for(y=1; y<=5; y++)
        {
            printf("Thread: %d, K.O. number: %d (%d/5)\n", (*i)+1, x, y);
            sleep(1);
        }
        OutCriticalSection(*i, j);

    }
}

int main()
{
    int i;
    int arg[2] = {0, 1};
    pthread_t ID[2];

    for(i=0; i<2; i++)
    {
        if((pthread_create(&ID[i], NULL, (void *) PrintingOnScreen, &arg[i])) != 0)
        {
            printf("Error : Can not create thread!");
            exit(1);
        }
    }

    pthread_join(ID[0], NULL);
    pthread_join(ID[1], NULL);

    return 0;

}
