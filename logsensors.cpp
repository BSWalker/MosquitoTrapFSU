/*
Test harness for MosSharedMem class objects developed for 
mosquito trap project.

Author: Steven B Walker
Date:   11/23/18
*/

#include "mosq_shm.h"
#include <iostream>
//#include <string>
//#include <unistd.h>
#include <cstring>
#include <cstdlib>
//#include <signal.h>
#include <wiringPi.h>
#include <iomanip>
#include <fstream>

int main(void)
{
	MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
	myMem.Dump();

	int fan1Rpm;
	int fan2Rpm;
	long int totalFanRev;
	float battVoltage;
	float temperature;
	float co2pressure;

	std::ofstream data;


    for(int i = 0; i < 60; ++i)
    {

	fan1Rpm = myMem.GetFan1RPM();
	fan2Rpm = myMem.GetFan2RPM();
        totalFanRev = myMem.GetTotalFanRev();
        battVoltage = myMem.GetBattVoltage();
	temperature = myMem.GetTemperature();
	co2pressure = myMem.GetCO2Pressure();

	data.open ("/home/pi/mosquito_log/sensor_data.txt", std::ios::trunc);
	data << fan1Rpm << '\n' << fan2Rpm << '\n' << totalFanRev << '\n' 
	     << battVoltage << '\n' << temperature << '\n' << co2pressure << '\n';
	data.close();

        delay(1000);
    }

    myMem.ReleaseSensors();

    return 0;
}
