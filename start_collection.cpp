#include <string>
#include <sstream>
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

#define FAN1_ERR 0
#define FAN2_ERR 1
#define LO_PRESSURE 2
#define HI_PRESSURE 3
#define SOLENOID_ERR 4
#define BATT_ERR 5

std::string GenerateFileName();
void LogSensorData(MosqSharedMem);
void TestSensors(MosqSharedMem);

int main(void)
{
    MosqSharedMem myMem(MosqSharedMem::A_READ||MosqSharedMem::A_WRITE);
    myMem.RegisterPID();
    Log collectionLog("/home/mosqtrap/mosquito/logs/event_log.txt");
    collectionLog.WriteLog("Starting Collection");
    Log data(GenerateFileName());

    wiringPiSetup();

    //variables for pins to make code more readable
    int fan = 15;
    int solenoid = 16;

    //setting up GPIO pins

    pinMode(fan, OUTPUT);
    pinMode(solenoid, OUTPUT);
    digitalWrite(fan, LOW);
    digitalWrite(solenoid, HIGH);

    delay(5000); // wait 5 seconds for trap to settle
    
    do
    {
        LogSensorData(myMem, data);
        for(int i = 0; i < 300; ++i) // delay 5 minutes between collection entries
        {
            TestSensors(myMem); // test every seconds
            delay(1000);
        }
    } while(1);

    return 0;
}

// generates a unique filename for the collection activity
// the file is used to store sensor data for the collection period
std::string GenerateFileName()
{
"/home/moqtrap/mosquito/collection_data/<filename>"
}


// saves sensor data to a collection log file
void LogSensorData(MosqSharedMem myMem, Log data)
{
    std::stringstream ss;

    ss << "Fan1:" << std::setw(6) << myMem.GetFan1RPM() << " rpm";
    ss << "  |  Fan2:" << std::setw(6) << myMem.GetFan2RPM() << " rpm";
    ss << "  |  Revolutions:" << std::setw(8) << myMem.GetTotalFanRev();
    ss << "  |  Battery:" << std::setw(8) << std::setprecision(4) << batt << "V";;
    ss << "  |  Temperature:" << std::setw(8) << std::setprecision(4) << temp << " F";
    ss << "  |  Pressure: " << std::setw(8) << std::setprecision(3) << press << " psi\n";
  
    data.WriteLog(ss.str()); //https://www.tutorialspoint.com/stringstream-in-cplusplus
}


// check for error conditions  based on sensor readings and log in error log
// each error is reported only once for now
void TestSensors(MosqSharedMem myMem)
{
    static bool errorFlags[5] = {1};
    Log errorLog("/home/mosqtrap/mosquito/logs/error_log.txt");
    if(myMem.GetFan1RPM() < 1000 && errorFlags[FAN1_ERR])
    {
        std::string msg = "Error: Low Fan 1 RPM Detected - " + std::to_string(myMem.GetFan1RPM()) + " RPM";
        errorLog.WriteLog(msg);
        errorFlags[FAN1_ERR] = false;
    }
    if(myMem.GetFan2RPM() < 1000 && errorFlags[FAN2_ERR])
    {
        std::string msg = "Error: Low Fan 2 RPM Detected - " + std::to_string(myMem.GetFan2RPM()) + " RPM";
        errorLog.WriteLog(msg);
        errorFlags[FAN2_ERR] = false;
    }

    // test ambient line pressure
    if(myMem.GetCO2Pressure() > 2 && errorFlags[HI_PRESSURE])
    {
        std::string msg = "Error: Abnormal Line Pressure Detected - " + std::to_string(myMem.GetCO2Pressure()) + " PSI";
        errorLog.WriteLog(msg);
        errorFlags[HI_PRESSURE] = false;
    }

    /*
    //click the solenoid on and off 3 times
    float pressure1 = myMem.GetCO2Pressure();
    float pressure2 = 0;
    digitalWrite(SOLENOID, HIGH);
    delay(3000);
    pressure2 = myMem.GetCO2Pressure();
    if(pressure2 > pressure1 - 0.5)
    {
        testLog.WriteLog("Error: Solenoid Error Detected - Insufficient Pressure Change When Active");
        testFlags[SOLENOID_ERR] = testPassed = false;
    }
    digitalWrite(SOLENOID, LOW);
    delay(2000);
    pressure1 = myMem.GetCO2Pressure();
    if(pressure1 < pressure2 + 0.5)
    {
        testLog.WriteLog("Error: Solenoid Error Detected - Failed to Close or Bad Seal");
        testFlags[SOLENOID_ERR] = testPassed = false;
    }
    digitalWrite(SOLENOID, HIGH);
    */

    if(myMem.GetBattVoltage() < 6 && errorFlags[BATT_ERR])
    {
        std::string msg = "Warning: Low Battery Voltage Detected - " + std::to_string(myMem.GetBattVoltage()) + " V";
        errorLog.WriteLog(msg);
        errorFlags[BATT_ERR] = false;
    }
    
}