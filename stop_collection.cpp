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
    wiringPiSetup();

    // pin number
    int solenoid = 16;

    // turn off solenoid
    pinMode(solenoid, OUTPUT);
    digitalWrite(solenoid, LOW);

    // write to event log
    std::ofstream collectionLog;
    collectionLog.open("/home/pi/mosquito_log/collectionlog.txt");
    time_t rawtime;
    time(&rawtime);
    struct tm* timeDate = localtime(&rawtime);
    char* timestamp = asctime(timeDate);
    timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string
    collectionLog << timestamp << ":|| Stopping Collection\n";
    collectionLog.close();

    return 0;
}

