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
entries for the entire year, with hour offsets to correspond to proper start and stop times.

Proper linux command line invocation is as follows:

crontab_file_gen.x AnyFileName.txt > AnyOutputFilename.txt

The program will print the output to the screen unless the redirection operator is used to
specify a system file.

*/

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#define SUNRISE 0
#define SUNSET  1

int main(int argc, char* argv[])
{

	if (argc > 2)
	{
		std::cout << "This program requires an input file for processing. See the following CLI example: \n\n"
			<< "crontab_file_gen.x AnyFileName.txt > AnyOutputFilename.txt\n\n"
			<< "Input files can be generated at http://aa.usno.navy.mil/data/docs/RS_OneYear.php";
		exit(EXIT_FAILURE);
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
	unsigned int buffer;

	// read the input file and organize into array
	for (int i = 1; i < 31; ++i)
	{
		if ((infile >> buffer)) // tests each new line for failure, discards the leading 'day' number on each line
		{
			std::cout << " *** Reached unexpected end of file. Aborting Program. *** \n";
			exit(EXIT_FAILURE);
		}
		for (int j = 0; j < 12; ++j)
		{
			// compensate for varying end of month
			if (i > daysInMonth[j])
			{
				++j; // incrememt past month that has ended to following month skipping extra white space 
			}
			infile >> buffer;
			chart[i][j][SUNRISE] = (buffer - 100); // stop at 1 hour before sunrise
			infile >> buffer; 
			chart[i][j][SUNSET] = (buffer + 100); // start at 1 hour plus sunset
		}
	}

	// declare string headers and section dividers
	std::string bootScript = "@reboot python3 /home/pi/mosquito/onBootTrapTest.py &\n";
	std::string startScript = "python3 /home/pi/mosquito/StartCollection.py\n";
	std::string stopScript = "python3 /home/pi/mosquito/StopCollection.py\n";
	std::string divider = "#========================================================#\n";
	std::string welcome = "#=== WELCOME TO FSU MOSQUITO TRAP COLLECTION SCHEDULE ===#\n";
	std::string boot = "#===                     ON BOOT                      ===#\n";
	std::string jan = "#===                     JANUARY                      ===#\n";
	std::string feb = "#===                     FEBRUARY                     ===#\n";
	std::string mar = "#===                      MARCH                       ===#\n";
	std::string apr = "#===                      APRIL                       ===#\n";
	std::string may = "#===                       MAY                        ===#\n";
	std::string jun = "#===                       JUNE                       ===#\n";
	std::string jul = "#===                       JULY                       ===#\n";
	std::string aug = "#===                      AUGUST                      ===#\n";
	std::string sep = "#===                    SEPTEMBER                     ===#\n";
	std::string oct = "#===                     OCTOBER                      ===#\n";
	std::string nov = "#===                    NOVEMBER                      ===#\n";
	std::string dec = "#===                    DECEMBER                      ===#\n";
	std::string startTimes = "# Start Collection Times\n";
	std::string stopTimes = "# Stop Collection Times\n";
	std::string monthDividers[12] = { jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec };
	unsigned int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// output the crontab file in usable format: minute hour dayOfMonth Month DayOfWeek(*) command
	std::cout << divider << welcome << divider << '\n' << divider << boot << divider << '\n' << bootScript << '\n';

	// write month by month
	for (int i = 0; i < 12; ++i)
	{
		std::cout << '\n' << divider << monthDividers[i] << divider << '\n' << startTimes;
		for (int j = 0; j < daysInMonth[i]; ++j)
		{
			std::cout << (chart[i][j][SUNSET] % 100) << ' ' << (chart[i][j][SUNSET] / 100) << ' '
				<< std::setfill('0') << std::setw(2) << j << ' ' << std::setw(2) << i << " * " << startScript;
		}

		std::cout << '\n' << stopTimes;
		for (int j = 0; j < daysInMonth[i]; ++j)
		{
			std::cout << (chart[i][j][SUNRISE] % 100) << ' ' << (chart[i][j][SUNRISE] / 100) << ' '
				<< std::setfill('0') << std::setw(2) << j << ' ' << std::setw(2) << i << " * " << startScript;
		}
	}
}


