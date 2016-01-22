#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int PID = 0;

void InterruptRoutine (int sig)
{
    kill (PID, SIGKILL);
    printf("\nI recived SIGINT so I send SIGKILL to proces %d\n", PID);
    exit(0);
}

int main (int argc, char *argv[])
{
    int numberOfSec = 0;
    int currentSignal = 0;
    int sig[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGQUIT};
    const char *sigName[] = {"SIGUSR1", "SIGUSR2", "SIGTERM", "SIGQUIT"};
    PID = atoi (argv[1]);
    sigset (SIGINT, InterruptRoutine);
    
    srand(time(NULL));
    
    while(1)
    {
        numberOfSec = rand()%3+3;
        currentSignal = rand()%4;
        
        sleep(numberOfSec);
        kill (PID, sig[currentSignal]); 
        printf("Sending %s to proces %d\n", sigName[currentSignal], PID);
    }
}
