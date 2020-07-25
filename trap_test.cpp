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
#include "log.h"

//const int fan1 = 15;
//const int solenoid = 16;

// variables for status light pins
//const int ledBlue = 27;
//const int ledGreen = 28;
//const int ledRed = 29;

#define FAN1_ERR 0
#define FAN2_ERR 1
#define LO_PRESSURE 2
#define HI_PRESSURE 3
#define SOLENOID_ERR 4
#define BATT_ERR 5

#define FAN 15
#define SOLENOID 16

#define LED_GRN 28
#define LED_RED 29
#define LED_BLU 27

void TestStartup();
void TestVerbose();
//void TestBluetooth();
//void LEDnotify(unisgned int color, unsigned int numFlashes);
void FlashRed(unsigned int t);
void LEDshowResults(bool isPassed, bool *testFlags);

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
    pinMode(FAN, OUTPUT);
    pinMode(SOLENOID, OUTPUT);
    pinMode(LED_GRN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLU,OUTPUT);
    digitalWrite(FAN, HIGH);
    digitalWrite(SOLENOID, LOW);
    digitalWrite(LED_GRN, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLU,LOW);

    if(argc == 1)
        TestStartup();
    else 
        TestVerbose();
    // else bluetooth test (not implemented)

    return 0;
}

void TestStartup() 
{
    MosqSharedMem myMem(MosqSharedMem::A_READ);
    bool testPassed = true;
    bool testFlags[6] = {1,1,1,1,1,1};
    Log testLog("/home/mosqtrap/mosquito/logs/boot-test-log.txt");

    testLog.WriteLog("Starting Trap test on boot");

    std::cout << "\nStarting Fan Test";

    //Fans for 5 seconds
    digitalWrite(FAN, LOW);
    delay(5000);
    if(myMem.GetFan1RPM() < 1000)
    {
        std::string msg = "Error: Low Fan 1 RPM Detected: " + std::to_string(myMem.GetFan1RPM()) + " RPM";
        testLog.WriteLog(msg);
        testFlags[FAN1_ERR] = testPassed = false;
    }
    if(myMem.GetFan2RPM() < 1000)
    {
        std::string msg = "Error: Low Fan 2 RPM Detected: " + std::to_string(myMem.GetFan2RPM()) + " RPM";
        testLog.WriteLog(msg);
        testFlags[FAN2_ERR] = testPassed = false;
    }
    delay(4000);
    digitalWrite(FAN, HIGH);
    delay(2000);

    std::cout << "\nFinished Fan Test, testing ambient pressure";

    // test ambient line pressure
    if(myMem.GetCO2Pressure() < 10)
    {
        std::cout << "\nLow Pressure Detected";
        std::string msg = "Error: Low Line Pressure Detected: " + std::to_string(myMem.GetCO2Pressure()) + " PSI";
        testLog.WriteLog(msg);
        testFlags[LO_PRESSURE] = testPassed = false;
    }
    if(myMem.GetCO2Pressure() > 20)
    {
        std::string msg = "Error: High Line Pressure Detected: " + std::to_string(myMem.GetCO2Pressure()) + " PSI";
        testLog.WriteLog(msg);
        testFlags[HI_PRESSURE] = testPassed = false;
    }

    std::cout << "\nTesting Solenoid Activation";

    //check solenoid activation
    float pressure1 = myMem.GetCO2Pressure();
    float pressure2 = 0;
    digitalWrite(SOLENOID, HIGH);
    delay(3000);
    pressure2 = myMem.GetCO2Pressure();
    if(pressure2 > pressure1 - 0.5 && testFlags[LO_PRESSURE]) // low pressure error creates false flag
    {
        testLog.WriteLog("Error: Solenoid Error Detected - Insufficient Pressure Change When Active");
        testFlags[SOLENOID_ERR] = testPassed = false;
    }
    digitalWrite(SOLENOID, LOW);
    delay(2000);
    pressure1 = myMem.GetCO2Pressure();
    if(pressure1 < pressure2 + 0.5 && testFlags[LO_PRESSURE]) // low pressure error creates false flag
    {
        testLog.WriteLog("Error: Solenoid Error Detected - Failed to Close or Bad Seal");
        testFlags[SOLENOID_ERR] = testPassed = false;
    }
    digitalWrite(SOLENOID, HIGH);

    std::cout << "\nTesting Battery Voltage";

    if(myMem.GetBattVoltage() < 10)
    {
        std::string msg = "Warning: Low Startup Battery Voltage Detected: " + std::to_string(myMem.GetBattVoltage()) + " V";
        testLog.WriteLog(msg);
        testFlags[BATT_ERR] = testPassed = false;
    }

    std::string msg = "Startup Test Complete with";
    if(testPassed)
       msg += " no ";
    else
       msg += " ";
    msg += "errors";
    std::cout << '\n' << msg << std::endl;
    testLog.WriteLog(msg);
    LEDshowResults(testPassed, testFlags);

    myMem.ReleaseSensors();

    return;
}

void TestVerbose() 
{
    MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
    myMem.Dump();
    
    digitalWrite(FAN, LOW);

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
            digitalWrite(SOLENOID, HIGH);

        delay(1000);                              
    }
    
    std::cout << "\n\n";
    digitalWrite(FAN, HIGH);
    digitalWrite(SOLENOID, LOW);

    myMem.ReleaseSensors();
          
    return;

}

void LEDshowResults(bool testPassed, bool *testFlags)
{
    std::cout << "\nLED Show Results";
    if(testPassed)
    {
        std::cout << " - Holding LED Green" << std::endl;
        digitalWrite(LED_GRN, HIGH);
        delay(15000);
        digitalWrite(LED_GRN, LOW);
        return;
    }

    for(unsigned int i = 0; i < 6; ++i)
    {
        if(!testFlags[i])
        {
            FlashRed(i+1);
            delay(3000);
        }
    }

    digitalWrite(LED_BLU, HIGH);
    std::cout << "\nHolding Blue LED" << std::endl;
    delay(10000);
    digitalWrite(LED_BLU, LOW);
}

void FlashRed(unsigned int t)
{
    std::cout << "\nFlashing Led: ";
    for(unsigned int i = 0; i < t; ++i)
    {
        digitalWrite(LED_RED, HIGH);
        std::cout << "R " << std::endl;
        delay(1000);
        digitalWrite(LED_RED, LOW);
    }
}
