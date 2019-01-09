/*
Test harness for MosSharedMem class objects developed for 
mosquito trap project.

Author: Steven B Walker
Date:   11/23/18
*/

#include "mosq_shm.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

int main()
{
    int temp;

    MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);

    myMem.Dump();

    std::cout << "\npassed constructor";
    std::cout << "\nThe fan RPM is: " << myMem.GetFanRPM();
    std::cout << "\nThe total revolutions are: " << myMem.GetTotalFanRev();
    std::cout << "\nThe battery voltage is: " << myMem.GetBattVoltage();
    std::cout << "\nThe temperature is: " << myMem.GetTemperature();
    std::cout << "\nThe line pressure is: " << myMem.GetCO2Pressure() << '\n';

    while (temp != -1)
    {
        std::cout << "Input fan RPM: ";
        std::cin >> temp;
        myMem.SetFanRPM(temp);
        std::cout << "The new fan RPM is: " << myMem.GetFanRPM() << '\n';
    }

    myMem.Dump();
    
    std::cout << "Press any key to test kill function...\n";
    std::cin.ignore(5);
    std::cin.get();

    myMem.ReleaseSensors();

    std::cout << "\nProgram 2 should be dead now. Press any key to exit.\n";
    std::cin.get();

    return 0;
}
