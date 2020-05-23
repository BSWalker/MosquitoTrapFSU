/*
Test harness for MosSharedMem class objects developed for 
mosquito trap project.

Author: Steven B Walker
Date:   11/23/18
*/

#include "mosq_shm.h"
#include <iostream>
//#include <unistd.h>
#include <cstring>
#include <cstdlib>
//#include <signal.h>
#include <wiringPi.h>
#include <iomanip>
#include <fstream>
#include <string>
#include "log.h"

#define FAN1_ERR 0
#define FAN2_ERR 1
#define CO2P_ERR 2
#define BATT_ERR 3
#define TEMP_ERR 4

struct SensorData
{
	int fan1Rpm;
	int fan2Rpm;
	long int totalFanRev;
	float battVoltage;
	float temperature;
	float co2pressure;
}

void GenerateLogFileName(std::string &name);
void UpdateReadings(SensorData &s, const MosqSharedMem &m);
void CheckForErrors(const SensorData &s);
std::string FormatSensorLogOutput(const SensorData &s);


int main(void)
{
	MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
	myMem.RegisterPID();
	std::string file = "/home/mosqtrap/mosquito/logs/";
	Log sensorLog(GenerateLogFileName(file));
	SensorData readings;
	


	

    
    return 0;
}


void GenerateLogFileName(std::string &name)
{
	time_t rawtime;
	time(&rawtime);
	struct tm* td = localtime(&rawtime);
	name = name + td->tm_month + '-' + td->tm_day + '-' + td->tm_year + "-sensorlog";

	//char* timestamp = asctime(Date);
	//timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string
}


void UpdateReadings(SensorData  &s, const MosqSharedMem &m)
{
	s.fan1Rpm = m.GetFan1RPM();
	s.fan2Rpm = m.GetFan2RPM();
	s.totalFanRev = m.GetTotalFanRev();
	s.battVoltage = m.GetBattVoltage();
	s.temperature = m.GetTemperature();
	s.co2pressure = m.GetCO2Pressure();
}


void CheckForErrors(const SensorData &s)
{
	static bool errFlags[5] = {0};
	Log errorLog("/home/mosqtrap/mosquito/logs/errorlog.txt");

	if(s.fan1Rpm < 1000 && !errFlags[FAN1_ERR])
    {
       	std::string msg = "Error: Low Fan 1 RPM Detected - " + std::to_string(s.fan1Rpm) + " RPM";
       	errLog.WriteLog(msg);
		errFlags[FAN1_ERR] = true;
    }
    if(s.fan2Rpm < 1000 && !errFlags[FAN2_ERR])
    {
        std::string msg = "Error: Low Fan 2 RPM Detected - " + std::to_string(s.fan2Rpm) + " RPM";
        errLog.WriteLog(msg);
		errFlags[FAN2_ERR] = true;
    }
    if(s.co2pressure > 2 && !errFlags[CO2P_ERR])
    {
        std::string msg = "Error: Abnormal Line Pressure Detected - " + std::to_string(s.co2pressure) + " PSI";
        errLog.WriteLog(msg);
		errFlags[CO2P_ERR] = true;
    }
    if(s.battVoltage < 6.5 && !errFlags[BATT_ERR])
    {
		std::string msg = "Warning: Battery Voltage Critically Low -" + std::to_string(s.battVoltage) + " V";
        errLog.WriteLog(msg);
		errFlags[BATT_ERR] = true;
    }
	if(s.temperature > 100 && !errFlags[TEMP_ERR])
	{
		std::string msg = "Warning: High Ambient Temperature Detected - " + std::to_string(s.temperature) + " F";
		errLog.WriteLog(msg);
		errFlags[TEMP_ERR] = true;
	}

	// need to enable a way to reset error flags periodically. But how? What if they are set at different
	// times and frequencies. Maybe this wasn't such a good idea... Could I save the timestamp and then
	// check the value against 0 instead of bool? dumb.

}


std::string FormatSensorLogOutput(const SensorData &s)
{
	std::ostringstream ss;
	ss << "\nFan1:" << std::setw(6) << s.fan1Rpm << " rpm";
    ss << "  |  Fan2:" << std::setw(6) << s.fan2Rpm << " rpm";
    ss << "  |  Revolutions:" << std::setw(8) << s.totalFanRev;
    ss << "  |  Battery:" << std::setw(8) << std::setprecision(4) << s.battVoltage << "V";;
    ss << "  |  Temperature:" << std::setw(8) << std::setprecision(4) << s.temperature << " F";
    ss << "  |  Pressure: " << std::setw(8) << std::setprecision(3) << s.co2pressure << " psi";

	return ss.str();
}