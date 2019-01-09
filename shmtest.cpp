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
#include <signal.h>

void SIGQUIT_handler(int);

int main()
{
	int temp;
    
    if( signal(SIGQUIT, SIGQUIT_handler) == SIG_ERR) // register handler
    {
        std::cout << "Error registering siquit handler\n";
    }
		MosqSharedMem myMem(MosqSharedMem::A_READ | MosqSharedMem::A_WRITE);
        myMem.RegisterPID();
		myMem.Dump();

		std::cout << "Input fan RPM: ";
		std::cin >> temp;
		myMem.SetFanRPM(temp);
		std::cout << "The fan RPM is: " << myMem.GetFanRPM() << '\n';

                std::cout << "Input total revolutions: ";
                std::cin >> temp;
                myMem.SetTotalFanRev(temp);
                std::cout << "The total revolutions are: " 
			  << myMem.GetTotalFanRev() << '\n';

                std::cout << "Input Battery Voltage: ";
                std::cin >> temp;
                myMem.SetBattVoltage(temp);
                std::cout << "The battery voltage is: " << myMem.GetBattVoltage() << '\n';

                std::cout << "Input temperature: ";
                std::cin >> temp;
                myMem.SetTemperature(temp);
                std::cout << "The temperature is: " << myMem.GetTemperature() << '\n';

                std::cout << "Input line pressure: ";
                std::cin >> temp;
                myMem.SetCO2Pressure(temp);
                std::cout << "The line pressure is: " << myMem.GetCO2Pressure() << '\n';

		std::cout << "\n\nWaiting for second program input...\n";

		while (myMem.GetFanRPM() != -1)
			sleep(1);

		std::cout << "Change Detected in fan RPM\n\n";

                std::cout << "Input fan RPM: ";
                std::cin >> temp;
                myMem.SetFanRPM(temp);
                std::cout << "The fan RPM is: " << myMem.GetFanRPM() << '\n';

                std::cout << "Input total revolutions: ";
                std::cin >> temp;
                myMem.SetTotalFanRev(temp);
                std::cout << "The total revolutions are: " 
                          << myMem.GetTotalFanRev() << '\n';

	myMem.PrintErrMsg("Test Message");

    std::cin.ignore(5);
    std::cin.get(); // delay until other program shuts down
    
	return 0;
}

void SIGQUIT_handler(int sig)
{
    signal(sig, SIG_IGN);
    std::cout << "\nReceived kill signal from host program!\n";
    exit(0);
}
