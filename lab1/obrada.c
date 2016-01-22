#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define N 6

int waitingMark[N];
int priorities[N];
int currentPriority = 0;
int allSignals[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGQUIT, SIGINT};

void DisableInterrupt()
{
    int index;
    for(index=0; index<5; index++)
        sighold(allSignals[index]);
}

void EnableInterrupt()
{
    int index;
    for(index=0; index<5; index++)
        sigrelse(allSignals[index]);
}

void InterruptRoutine(int currentSignal);

void ProcessingInterrupt(int currentSignal)
{
    sigset(SIGUSR1, InterruptRoutine);
    sigset(SIGUSR2, InterruptRoutine);
    sigset(SIGTERM, InterruptRoutine);
    sigset(SIGQUIT, InterruptRoutine);
    sigset(SIGINT, InterruptRoutine);

    int index;

    switch(currentSignal)
    {
        case (1):
                printf("|-  S  -  -  -  -|\n");
                for(index=1; index<=5; index++)
                {
                    printf("|-  %d  -  -  -  -|\n", index);
                    sleep(1);
                }
                printf("|-  E  -  -  -  -|\n");
                break;

        case (2):
                printf("|-  -  S  -  -  -|\n");
                for(index=1; index<=5; index++)
                {
                    printf("|-  -  %d  -  -  -|\n", index);
                    sleep(1);
                }
                printf("|-  -  E  -  -  -|\n");
                break;

        case (3):
                printf("|-  -  -  S  -  -|\n");
                for(index=1; index<=5; index++)
                {
                    printf("|-  -  -  %d  -  -|\n", index);
                    sleep(1);
                }
                printf("|-  -  -  E  -  -|\n");
                break;

        case(4):
                printf("|-  -  -  -  S  -|\n");
                for(index=1; index<=5; index++)
                {
                    printf("|-  -  -  -  %d  -|\n", index);
                    sleep(1);
                }
                printf("|-  -  -  -  E  -|\n");
                break;

        case (5):
                printf("|-  -  -  -  -  S|\n");
                for(index=1; index<=5; index++)
                {
                    printf("|-  -  -  -  -  %d|\n", index);
                    sleep(1);
                }
                printf("|-  -  -  -  -  E|\n");
                break;
    }
}

void InterruptRoutine(int currentSignal)
{
    int index=1;
    int j = 0;
    int thisSignal = 0;

    DisableInterrupt();

    switch(currentSignal)
    {
        case SIGUSR1:
                    index=1;
                    printf("|-  X  -  -  -  -|\n");
                    break;

        case SIGUSR2:
                    index=2;
                    printf("|-  -  X  -  -  -|\n");
                    break;

        case SIGTERM:
                    index=3;
                    printf("|-  -  -  X  -  -|\n");
                    break;

        case SIGQUIT:
                    index=4;
                    printf("|-  -  -  -  X  -|\n");
                    break;

        case SIGINT:
                    index=5;
                    printf("|-  -  -  -  -  X|\n");
                    break;
    }

    waitingMark[index]++;

    if(currentPriority<index)
    {
        do
        {
            thisSignal = 0;

            for(j=(currentPriority + 1); j<N; j++)
            {
                if (waitingMark[j] != 0)
                {
                    thisSignal = j;
                }
	    }

	    if(thisSignal > 0)
            {
                waitingMark[thisSignal]--;
                priorities[thisSignal] = currentPriority;
                currentPriority = thisSignal;
                EnableInterrupt();
                ProcessingInterrupt(thisSignal);
                DisableInterrupt();
                currentPriority = priorities[thisSignal];
            }
        } while(thisSignal > 0);
    }

    EnableInterrupt();
}

int main()
{
    int index;
    sigset(SIGUSR1, InterruptRoutine);
    sigset(SIGUSR2, InterruptRoutine);
    sigset(SIGTERM, InterruptRoutine);
    sigset(SIGQUIT, InterruptRoutine);
    sigset(SIGINT, InterruptRoutine);

    printf("Processing interrupts, PID=%d\n", getpid());
    printf("|G  1  2  3  4  5|\n");
    printf("|----------------|\n");

    for(index=0; index<5; index++)
    {
	sleep(1);
    }

    for(index=1; index<=50; index++)
    {
        printf("|%d  -  -  -  -  -|\n", index%10);
        sleep(1);
    }

    printf("Basic program is over.\n");

    return 0;
}
