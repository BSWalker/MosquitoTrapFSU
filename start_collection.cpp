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


//Write to event log

std::ofstream collectionLog;
collectionLog.open("/home/pi/mosquito_log/collectionlog.txt");
time_t rawtime;
time(&rawtime);
struct tm* timeDate = localtime(&rawtime);
char* timestamp = asctime(timeDate);
timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string
collectionLog << timestamp << "|| Starting Collection\n";
collectionLog.close();

//variables for pins to make code more readable
int fan1 = 11;
int fan2 =13;
int solenoid = 15;

//setting up GPIO pins

pinMode(fan1, OUTPUT);
pinMode(fan2, OUTPUT);
pinMode(solenoid, OUTPUT);
digitalWrite(fan1, LOW);
digitalWrite(fan2, LOW);
digitalWrite(solenoid, HIGH);

return 0;
}

