/*
This file contains the implementation for Log.h

Author: S. Brandon Walker
Date: 3/15/2020
*/

#include <log.h>
#include <fstream>
#include <time.h>
#include <cstring>

Log::Log(const std::string & filepath) : _filepath(filepath)
{
    _filestream.open(_filepath.c_str(), std::ios::out | std::ios::app);
    if(!_filestream)
    {
        std::cout << "Error: Could not Open Filepath in constructor";
    }
    _filestream.close();
}

void Log::WriteLog(std::string & message)
{
	_filestream.open(_filepath.c_str(), std::ios::out | std::ios::app);

	time_t rawtime;
	time(&rawtime);
	struct tm* timeDate = localtime(&rawtime);

	char* timestamp = asctime(timeDate);
	timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string

	_filestream << timestamp << " || " << message << '\n';
	_filestream.close();
}

void Log::WriteLog(const std::string & message)
{
	_filestream.open(_filepath.c_str(), std::ios::out | std::ios::app);

	time_t rawtime;
	time(&rawtime);
	struct tm* timeDate = localtime(&rawtime);

	char* timestamp = asctime(timeDate);
	timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string

	_filestream << timestamp << " || " << message << '\n';
	_filestream.close();
}
