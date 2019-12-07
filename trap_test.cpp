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
    wiringPiSetup();
    //setting up GPIO pins
    pinMode(fan1, OUTPUT);
    pinMode(solenoid, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledBlue,OUTPUT);
    digitalWrite(fan1, HIGH);
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
    digitalWrite(fan1, HIGH);
    delay(2000);
    digitalWrite(fan1, LOW);
    delay(4000);
    digitalWrite(fan1, HIGH);
    delay(2000);
/*
    //Fan 2 for 4 seconds
    digitalWrite(fan2, LOW);
    delay(4000);
    digitalWrite(fan2, HIGH);
    delay(2000)
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

float batt = 0;
float temp = 0;
float press = 0;

    for(int i = 0; i < 5; ++i)
	delay(1000);

    for(int i = 0; i < 60; ++i)
    {
batt = myMem.GetBattVoltage();
temp = myMem.GetTemperature();
press = myMem.GetCO2Pressure();

        std::cout << "\nFan1:" << std::setw(6) << myMem.GetFan1RPM() << " rpm";
        std::cout << "  |  Fan2:" << std::setw(6) << myMem.GetFan2RPM() << " rpm";
        std::cout << "  |  Revolutions:" << std::setw(8) << myMem.GetTotalFanRev();
        std::cout << "  |  Battery:" << std::setw(8) << std::setprecision(4) << batt << "V";;
        std::cout << "  |  Temperature:" << std::setw(8) << std::setprecision(4) << temp << " F";
        std::cout << "  |  Pressure: " << std::setw(8) << std::setprecision(3) << press << " psi";
        
        if(i == 15)
            digitalWrite(solenoid, HIGH);

        delay(1000);                              
    }
    
    std::cout << "\n\n";
    digitalWrite(fan1, HIGH);
    digitalWrite(solenoid, LOW);

    myMem.ReleaseSensors();
          
    return;

}
