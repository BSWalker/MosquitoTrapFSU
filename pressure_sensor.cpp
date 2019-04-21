/*

pressureSensor.cpp is a driver program that reads gauge pressure from HoneyWell ABPMANN030PG2A3 sensor to be used with the FSU Mosquito Trap Project with software implemented on the 

Raspberry Pi platform. This program is configured specifically for this sensor.

HoneyWell I2C Communications documentation: https://sensing.honeywell.com/i2c-comms-digital-output-pressure-sensors-tn-008201-en-final-30may12.pdf

HoneyWell ABPDANT015PGAA5 datasheet: https://sensing.honeywell.com/honeywell-sensing-basic-board-mount-pressure-abp-series-datasheet-2305128-e-en.pdf


Author: Logan M Wallace
Date:   02/25/2019

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <wiringPi.h>
#include <pcf8591.h>
#include "mosq_shm.h"
#include <wiringPiI2C.h>
#include <bitset>
#include <unistd.h>

int main(void)
	{
	float outputMax = 14745;  //output at max pressure [counts], calibrated to 90%;
	float outputMin = 1638;  //output at min pressure [counts], calibrated to 10%; see honeywell i2c comms documentation
	float pressureMax = 30; //maximum pressure
	float pressureMin = 0; //minimum pressure
	int   digiValue;
	float pressure;
	int   fd;
	char  response[4];

	wiringPiSetup();
	fd = wiringPiI2CSetup(0x28);   //declare I2C address, default for this sensor is 0x28;
	MosqSharedMem myMem(MosqSharedMem::A_WRITE); //allow writing to shared memory repository
	myMem.RegisterPID(); //register PID

	while (1)
	{
    	read(fd,response,4);
		digiValue = response[0];
		digiValue = digiValue << 8;
		digiValue |= response[1];

		pressure = ((((float)digiValue - outputMin)*(pressureMax-pressureMin))/(outputMax-outputMin)) + pressureMin; //pressure output function, see honeywell i2c comms documentation
		//std::cout << "\nPSI: " << std::setw(5) << pressure; //output to screen
   		myMem.SetCO2Pressure(pressure);  //writes to shared memory
   		delay(1000);     
	}

 	return 0;

}  
