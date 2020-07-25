/*

crontab_file_generator.cpp

Author: Steven B. Walker
Date:   9/17/18

This program is developed in support of the FSU mosquito trap project which operates
on the Raspberry Pi platform running  an embedded Linux operating system Raspbian
Stretch. The trap components are switched on at an hour after sunset, and collection
ends at an hour before sunrise. An RTC keeps the time in between power cycles, and
crontab is used as a task scheduler with start and stop times entered for every day
of the year.

This program accepts as an input file a table of sunrise and sunset times provided by
http://aa.usno.navy.mil/data/docs/RS_OneYear.php
Follow the instructions for copying ONLY the numbers and not the month text into a simple
text file. The file will be read and stored, then used to generate crontab formatted
entries for the entire year, with user entered offsets to correspond to proper start and stop times.

The second and third arguments are the offsets before and after sunrise and sunset. These must be
integer arguments between -12 and 5 for sunset and -4 and 12 for sunrise.

Proper linux command line invocation is as follows:

crontab_file_gen.x AnyFileName.txt SR_Offset SS_Offset > AnyOutputFilename.txt

Alternatively the output from the program can be piped directly into Crontab

crontab_file_gen.x AnyFileName.txt SR_Offset SS_Offset | crontab

The program will print the output to the screen unless the redirection operator is used to
specify a system file.

*/

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <errno.h>

#define SUNRISE 0
#define SUNSET  1

void PrintInitErrorMssg();

int main(int argc, char* argv[])
{
	// verify valid number of arguments supplied
	if(argc < 4)
	{
		PrintInitErrorMssg();
	}

	char *p;
	char *p2;
	errno = 0;
	long SR_offset = strtol(argv[2], &p, 10);
	long SS_offset = strtol(argv[3], &p2, 10);

	// check for validity of input arguments
	if (argc < 4 || errno != 0 || *p != '\0' || *p2 != '\0' || SR_offset < -4
	    || SR_offset > 12 || SS_offset < -12 || SS_offset > 5) 
	{
		PrintInitErrorMssg();
	}

	std::ifstream infile;
	infile.open(argv[1]);
	if (infile.fail())
	{
		std::cout << " *** Error: cannot open file " << argv[1] << " or file not found. Exiting program.\n";
		exit(EXIT_FAILURE);
	}

	// declare local variables if file open successful
	unsigned int chart[12][31][2] = { 0 }; // 12 months by 31 days by 2 events
	unsigned int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int buffer;

	// read the input file and organize into array
	for (unsigned int d = 0; d < 31; ++d)
	{
		if (!(infile >> buffer)) // tests each new line for failure, discards the leading 'day' number on each line
		{
			std::cout << " *** Reached unexpected end of file. Aborting Program. *** \n";
			exit(EXIT_FAILURE);
		}

		for (unsigned int m = 0; m < 12; ++m)
		{
			// compensate for varying end of month
			if ((d+1) > daysInMonth[m]) // (d+1) prevents off-by-one error in comparison
			{
				++m; // incrememt past month that has ended to following month skipping extra white space 
			}
			infile >> buffer;
			chart[m][d][SUNRISE] = (buffer + (int)SR_offset * 100); // stop at 1 hour before sunrise
			infile >> buffer; 
			chart[m][d][SUNSET] = (buffer + (int)SS_offset * 100); // start at 1 hour plus sunset
		}
	}

	// declare string headers and section dividers
	std::string bootScript = "@reboot /home/mosqtrap/mosquito/runnables/boot-test.sh &\n";
	std::string startScript = "/home/mosqtrap/mosquito/runnables/startcollection.sh\n";
	std::string stopScript = "/home/mosqtrap/mosquito/runnables/stopcollection.sh\n";
	std::string divider = "#========================================================#\n";
	std::string welcome = "#    WELCOME TO FSU MOSQUITO TRAP COLLECTION SCHEDULE    #\n";
	std::string boot = "#                        ON BOOT                         #\n";
	std::string jan = "#                        JANUARY                         #\n";
	std::string feb = "#                        FEBRUARY                        #\n";
	std::string mar = "#                         MARCH                          #\n";
	std::string apr = "#                         APRIL                          #\n";
	std::string may = "#                          MAY                           #\n";
	std::string jun = "#                          JUNE                          #\n";
	std::string jul = "#                          JULY                          #\n";
	std::string aug = "#                         AUGUST                         #\n";
	std::string sep = "#                       SEPTEMBER                        #\n";
	std::string oct = "#                        OCTOBER                         #\n";
	std::string nov = "#                       NOVEMBER                         #\n";
	std::string dec = "#                       DECEMBER                         #\n";
	std::string startTimes = "# Start Collection Times\n";
	std::string stopTimes = "# Stop Collection Times\n";
	std::string monthDividers[12] = { jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec };

	// output the crontab file in usable format: minute hour dayOfMonth Month DayOfWeek(*) command
	std::cout << divider << welcome << divider << '\n' << divider << boot << divider << '\n' << bootScript;

	// write month by month
	for (unsigned int i = 0; i < 12; ++i)
	{
		std::cout << '\n' << divider << monthDividers[i] << divider << '\n' << startTimes;
		for (unsigned int j = 0; j < daysInMonth[i]; ++j)
		{
			std::cout << std::setfill('0') << std::setw(2) << (chart[i][j][SUNSET] % 100) << ' '
				  << std::setfill('0') << std::setw(2)<< (chart[i][j][SUNSET] / 100) << ' '
				  << std::setfill('0') << std::setw(2) << (j+1) << ' ' << std::setw(2) << (i+1)
				  << " * " << startScript;
		}

		std::cout << '\n' << stopTimes;
		for (unsigned int j = 0; j < daysInMonth[i]; ++j)
		{
			std::cout << std::setfill('0') << std::setw(2) << (chart[i][j][SUNRISE] % 100) << ' '
				  << std::setfill('0') << std::setw(2) << (chart[i][j][SUNRISE] / 100) << ' '
				  << std::setfill('0') << std::setw(2) << (j+1) << ' ' << std::setw(2) << (i+1)
				  << " * " << stopScript;
		}
	}
}


void PrintInitErrorMssg()
{
	std::cout << "This program requires an input file for processing. See the following CLI example: \n\n"
		  << "crontab_file_gen.x AnyFileName.txt SR_offset SS_offset | crontab\n\n"
		  << "SR_offset = sunrise offset and must be an integer between -4 and 12\n"
		  << "SS_offset = sunset offset and must be an integer between -12 and 5\n"
		  << "Input files can be generated at http://aa.usno.navy.mil/data/docs/RS_OneYear.php\n\n";
	exit(EXIT_FAILURE);
}
