/* 
 * This is a driver program to sense the fan RPM of a 3-wire style comuter
 * fan attached to the mosquito trap. It is configured to read the RPM from
 * two fans at once and write the results into shared memory for other programs 
 * in the mosquito trap family to use.
 *
 * Author: Steven Walker
 * Date:   12/9/2018
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <iostream>
#include "mosq_shm.h"

static volatile long int globalCounter [2];

void fan0_interrupt (void) { ++globalCounter[0]; }
void fan1_interrupt (void) { ++globalCounter[1]; }

int main (int argc, char* argv[])
{
    int count[2] = {0};
    long int lastCount[2] = {0};

    bool verbose = false;
    if(!argc)
        verbose = true;
    
    MosqSharedMem myShm(MosqSharedMem::A_WRITE);
    myShm.RegisterPID();

    globalCounter[0] = globalCounter[1] = 0;

    wiringPiSetup();

    wiringPiISR(6, INT_EDGE_FALLING, &fan0_interrupt);
    wiringPiISR(25, INT_EDGE_FALLING, &fan1_interrupt);

    while (1) // repeat until master kills program via pid
    {
        delay(1000);
        count[0] = (globalCounter[0] - lastCount[0])/2;
	count[1] = (globalCounter[1] - lastCount[1])/2;
        myShm.SetFan1RPM((count[0] * 60));
	myShm.SetFan2RPM((count[1] * 60));
        myShm.SetTotalFanRev(globalCounter[0]/2);
        lastCount[0] = globalCounter[0];
	lastCount[1] = globalCounter[1];

        if(verbose) 
        {
            std::cout << "\nFan 1 RPM = " << (count[0] * 60) << " || Total Rev = " << (globalCounter[0]/2);
            std::cout << "\nFan 2 RPM = " << (count[1] * 60) << " || Total Rev = " << (globalCounter[1]/2);
        }
    }
}
