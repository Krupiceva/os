#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

int pauseFlag = 0;
unsigned long number = 1000000001;
unsigned long lastNumber = 1000000001;

void PeriodicPrint ()
{
    printf("\nI'm a prime number: %lu\n", lastNumber);
}

void SetPause()
{
    pauseFlag = 1 - pauseFlag;
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

int main ()
{
    int isPrime;
    struct itimerval t;

    sigset(SIGALRM, PeriodicPrint);
    sigset(SIGINT, SetPause );
    sigset(SIGTERM, StopProgram);

    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = 500000;

    t.it_interval.tv_sec = 5;
    t.it_interval.tv_usec = 500000;

    setitimer (ITIMER_REAL, &t, NULL);

    while (1)
    {
        if ( (isPrime = IsItPrime(number)) == 1)
            lastNumber = number;
        number++;

        while (pauseFlag == 1);

    }
}
