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
#include "log.h"


int main(void)
{
    wiringPiSetup();

    // pin number
    int solenoid = 16;

    // turn off solenoid
    pinMode(solenoid, OUTPUT);
    digitalWrite(solenoid, LOW);

    // write to event log
    Log collectionLog("/home/mosqtrap/mosquito/logs/event_log.txt");
    collectionLog.WriteLog("Stopping Collection");

    return 0;
}

