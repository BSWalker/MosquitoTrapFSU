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

int main(void)
{
	MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
	myMem.Dump();

    for(int i = 0; i < 60; ++i)
    {
	std::cout << "\nRPM:" << std::setw(5) << myMem.GetFanRPM();
        std::cout << "    Total Rev:" << std::setw(5) << myMem.GetTotalFanRev();
        std::cout << "    Temp:" << std::setw(3) << myMem.GetTemperature() << '\n';

        delay(1000);
    }

    myMem.ReleaseSensors();

    return 0;
}
