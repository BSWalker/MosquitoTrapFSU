/*
MosqSharedMem.h contains headers for a shared memory wrapper with exception class extension to be used with the 
FSU Mosquito Trap Project with software implemented on the Raspberry Pi platform. This library will allow all the device drivers
and other programs to communicate via shared memory, allowing them to have separate program implemenations. This program is loosely
based upon an example found at:

https://cppcodetips.wordpress.com/2015/02/28/c-wrapper-class-for-shared-memory/

Author: Steven B Walker
Date:   11/20/18
*/

#pragma once

#ifndef _MOSQSHM_H
#define _MOSQSHM_H

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

// exception class signals a problem with the execution of a shared memory call
// for MosqSharedMem. Inherits from std::exception

class MSMexception : public std::exception
{
public:
	// Construct a SharedMemoryException with a explanatory message.
	// @param message explanatory message
	// @param bSysMsg true if system message(from strerror(errno))
	//should be postfixed to the user provided message

	MSMexception(const std::string &message) : m_sMsg(message) {}
	~MSMexception() throw() {}

	// Returns a pointer to the (constant) error description.
	// @return A pointer to a \c const \c char*. The underlying memory
	// is in posession of the \c Exception object. Callers \a must
	// not attempt to free the memory.
	const char* what() const throw() { return m_sMsg.c_str(); }

protected:
	std::string m_sMsg; // error message
};

// shared memory class
class MosqSharedMem
{
public:
	enum { C_READ_ONLY = O_RDONLY, C_READ_WRITE = O_RDWR } CREATE_MODE;
	enum { A_READ = PROT_READ, A_WRITE = PROT_WRITE } ATTACH_MODE;

	static const std::string lockSemaphoreName;
public:
	// constructor - destructor
	// constructor requires attach mode: A_READ, A_WRITE, or (A_READ | A_WRITE)
	explicit MosqSharedMem (int);
	~MosqSharedMem ();

	// shared data update and retreive functions - outside API
	void SetFanRPM      (const int&);
	void SetTotalFanRev (const int&);
	void SetBattVoltage (const int&);
	void SetTemperature (const int&);
	void SetCO2Pressure (const int&);

	int GetFanRPM      (void) const;
	int GetTotalFanRev (void) const;
	int GetBattVoltage (void) const;
	int GetTemperature (void) const;
	int GetCO2Pressure (void) const;

	void Dump (); // for development
private:
	// shared memory operations - implemented privately for simplified public API
	bool  Create  (size_t nSize, int mode = C_READ_WRITE);
	bool  Attach  (int mode = A_READ | A_WRITE);
	void  Detach  ();
	void  Lock    () const;
	void  UnLock  () const;
	int   GetID   () { return m_iD; }
	void  Clear();

private:
	struct SensorData // embedded struct declaration to encapsulate data in shared memory
	{
		int fanRPM;
		int totalFanRotations;
		int battVoltage;
		int temperature;
		int CO2pressure;
	};

private:
	std::string m_sName;
	int m_iD;
	sem_t* m_SemID;
	size_t m_nSize;
        SensorData* realTimeData_ptr;
};


#endif
