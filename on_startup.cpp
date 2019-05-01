#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstring>
#include <wiringPi.h>

int main(void)
{
    //variables for pins to make code more readable
    int fan1 = 15;

    wiringPiSetup();
    //setting up GPIO pins

    pinMode(fan1, OUTPUT);

    digitalWrite(fan1, HIGH);;

    return 0;
}

