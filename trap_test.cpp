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
#include "mosq_shm.h"
#include <wiringPi.h>
#include <string>
#include <iomanip>

int fan1 = 15;
//int fan2 = 13;
int solenoid = 16;
                
// variables for status light pins
int ledBlue = 27;
int ledGreen = 28;
int ledRed = 29;

void TestStartup();
void TestVerbose();
void TestBluetooth();
 
int main (int argc, char* argv[])
{

//Write Timestamp to log
/*
std::ofstream testLog;
testLog.open("/home/pi/mosquito/logs/testlog.txt");
time_t rawtime;
time(&rawtime);
struct tm* timeDate = localtime(&rawtime);
char* timestamp = asctime(timeDate);
timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string
testLog << timestamp << "|| Starting Trap test on boot\n";
*/
    //setting up GPIO pins
    pinMode(fan1, OUTPUT);
    //pinMode(fan2, OUTPUT);
    pinMode(solenoid, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledBlue,OUTPUT);
    digitalWrite(fan1, HIGH);
    //digitalWrite(fan2, HIGH);
    digitalWrite(solenoid, LOW);
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledBlue,LOW);

    if(argc == 0)
        TestStartup();
    else 
        TestVerbose();
    // else bluetooth test (not implemented)

    return 0;
}

void TestStartup() 
{
    //Write Timestamp to log
    std::ofstream testLog;
    testLog.open("/home/pi/mosquito/logs/testlog.txt");
    time_t rawtime;
    time(&rawtime);
    struct tm* timeDate = localtime(&rawtime);
    char* timestamp = asctime(timeDate);
    timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string
    testLog << timestamp << "|| Starting Trap test on boot\n";
  
    //Fan 1 for 4 seconds
    digitalWrite(fan1, LOW);
    delay(4000);
    digitalWrite(fan1, HIGH);
    delay(2000);
/*
    //Fan 2 for 4 seconds
    digitalWrite(fan2, LOW);
    delay(4000);
    digitalWrite(fan2, HIGH);
    delay(2000);
*/
    //click the solenoid on and off 3 times
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
    delay(1000);
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
    delay(1000);
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
    delay(1000);

    //write completed message to log
    timestamp = asctime(timeDate);
    timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string
    testLog << timestamp << "|| Startup test completed\n";
    testLog.close();

    //display reassuring green led for 30 seconds, smile and wave
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen,HIGH);
    delay(30000);
    digitalWrite(ledGreen, LOW);

    return;
}

void TestVerbose() 
{
    MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
    myMem.Dump();
    
    digitalWrite(fan1, LOW);


    for(int i = 0; i < 30; ++i)
    {
        std::cout << "\nFan 1 RPM:" << std::setw(5) << myMem.GetFan1RPM();
        std::cout << "\nFan 2 RPM:" << std::setw(5) << myMem.GetFan2RPM();
        std::cout << "\nTotal Rev:" << std::setw(5) << myMem.GetTotalFanRev();
        std::cout << "\nBatt Volt:" << std::setw(5) << myMem.GetBattVoltage();
        std::cout << "\nAir Temp:" << std::setw(5) << myMem.GetTemperature();
        std::cout << "\nPressure:" << std::setw(5) << myMem.GetCO2Pressure();
        std::cout << std::string(100,'\n'); // clears the screen
        
        if(i == 15)
            digitalWrite(solenoid, HIGH);

        delay(1000);                              
    }
    
    digitalWrite(fan1, HIGH);
    digitalWrite(solenoid, LOW);

    myMem.ReleaseSensors();
          
    return;

}
