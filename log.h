/* 
log.h contains the header file for Log class, for use with the FSU mosquito trap program.

Author: S. Brandon Walker
Date: 3/15/2020
*/

#pragma once
#ifndef _LOG_H
#define _LOG_H

#include <iostream>
#include <fstream>
#include <string>

class Log
{
public: 
    explicit Log(const std::string &);
    void WriteLog (std::string &);
    void WriteLog (const std::string &);

private:
    std::string _filepath;
    std::ofstream _filestream;
};


#endif