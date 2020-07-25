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
#include <iomanip>
#include <fstream>
#include <time.h>
#include <cstring>
#include <wiringPi.h>
#include "log.h"
#include "mosq_shm.h"

#define FAN1_ERR 0
#define FAN2_ERR 1
#define LO_PRESSURE 2
#define HI_PRESSURE 3
#define SOLENOID_ERR 4
#define BATT_ERR 5
#define Hi_TEMP 6

std::string GenerateFileName();
void LogSensorData(MosqSharedMem &, Log &);
void TestSensors(MosqSharedMem &);

int main(void)
{
    std::cout << "Start\n";
    MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
    std::cout << "SharedMem init";
    myMem.RegisterPID();
    std::cout << "RegisterPID\n";
    Log collectionLog("/home/mosqtrap/mosquito/logs/event-log.txt");
    collectionLog.WriteLog("Starting Collection");
    Log data(GenerateFileName());

    wiringPiSetup();

    std::cout << "passed variable setup\n";

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
        for(int i = 0; i < 300; ++i) // (300) delay 5 minutes between collection entries
        {
            TestSensors(myMem); // test every second
            delay(1000);
        }
    } while(1);

    return 0;
}

// generates a unique filename for the collection activity
// the file is used to store sensor data for the collection period
std::string GenerateFileName()
{
    std::string path = "/home/mosqtrap/mosquito/collection-data/";
    std::string filename;
    // current date/time based on current system
    time_t now = time(0);
    tm *ltm = localtime(&now);

    filename = std::to_string(1 + ltm->tm_mon) + '.' + std::to_string(ltm->tm_mday) + '.' + std::to_string(1900 + ltm->tm_year)
                            + '.' + std::to_string(1+ltm->tm_hour) + ':' + std::to_string(1+ltm->tm_min) + ".txt";

    std::cout << filename << '\n';
    filename = path + filename;
    std::cout << filename << '\n';

    return filename;                        
    
    /*// print various components of tm structure.
    cout << "Year:" << 1900 + ltm->tm_year << endl;
    cout << "Month: "<< 1 + ltm->tm_mon<< endl;
    cout << "Day: "<<  ltm->tm_mday << endl;
    cout << "Time: "<< 1 + ltm->tm_hour << ":";
    cout << 1 + ltm->tm_min << ":";
    cout << 1 + ltm->tm_sec << endl;*/
}


// saves sensor data to a collection log file
void LogSensorData(MosqSharedMem & myMem, Log & data)
{
    std::stringstream ss;

    ss << "Fan1:" << std::setw(6) << myMem.GetFan1RPM() << " rpm";
    ss << "  |  Fan2:" << std::setw(6) << myMem.GetFan2RPM() << " rpm";
    ss << "  |  Revolutions:" << std::setw(8) << myMem.GetTotalFanRev();
    ss << "  |  Battery:" << std::setw(8) << std::setprecision(4) << myMem.GetBattVoltage() << "V";;
    ss << "  |  Temperature:" << std::setw(8) << std::setprecision(4) << myMem.GetTemperature() << " F";
    ss << "  |  Pressure: " << std::setw(8) << std::setprecision(3) << myMem.GetCO2Pressure() << " psi";
  
    data.WriteLog(ss.str()); //https://www.tutorialspoint.com/stringstream-in-cplusplus
}


// check for error conditions  based on sensor readings and log in error log
// each error is reported only once for now
void TestSensors(MosqSharedMem & myMem)
{
    static bool errorFlags[7] = {1,1,1,1,1,1,1};
    Log errorLog("/home/mosqtrap/mosquito/logs/error-log.txt");
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

    if(myMem.GetTemperature() > 99 && errorFlags[Hi_TEMP])
    {
        std::string msg = "Warning: High Ambient Temperature Detected - " + std::to_string(myMem.GetTemperature()) + " F";
        errorLog.WriteLog(msg);
        errorFlags[Hi_TEMP] = false;
    }
    
}
