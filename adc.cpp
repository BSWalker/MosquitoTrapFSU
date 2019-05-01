/*

ADC3.cpp is a driver program which reads a battery voltage value using the PCF8591 Analog to Digital convertor to be used with the FSU Mosquito Trap Project with software implemented on the 

Raspberry Pi platform. This program is configured specifically for this sensor. 


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


int main(void)
{
float value;
float battVoltage;
MosqSharedMem myMem(MosqSharedMem::A_WRITE); //enable write to shared memory
myMem.RegisterPID(); //register PID

  if (wiringPiSetup() == -1)
    {
     exit(1);
    }
  
  pcf8591Setup(120, 0x48); //random number over 64 for the first number(imaginary pin assigned) and the hex number is the default I2C address

  pinMode(120, INPUT); //sets imaginary pin as input
 
float maxV = 0;
  while (1)
{
  for (int i = 0; i < 20; ++i)
   {
    value = analogRead(120);  //uses analogRead function to retrive value
    battVoltage = (value * 5.0 / 255.0)*(1200.0/470.0); //multiplies the read value by the reference voltage and divides by the number of 2 to the 8th, ie the PCF8591 is an 8 bit ADC;
    if (battVoltage > maxV)
       maxV = battVoltage;
    //std::cout << "\nRawValue: " << std::setw(5) << value;
    //std::cout << "\nBattVoltage: " << std::setw(5) << battVoltage; //outputs battery voltage on screen
    //std::cout << ".";
    delay(100); 
   }

   myMem.SetBattVoltage((float)maxV); //writes battery voltage to shared memory

   //std::cout << "\nVoltage: " << std::setw(5) << maxV;

}

 return 0;

}  


