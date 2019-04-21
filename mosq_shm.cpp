/*
MosqSharedMem.cpp contains implementations for a shared memory wrapper with exception class 
extension to be used with the FSU Mosquito Trap Project with software implemented on the 
Raspberry Pi platform. This library will allow all the device drivers and other programs to communicate via shared memory, allowing them to have separate program implemenations. This 
program is loosely based upon an example found at:

https://cppcodetips.wordpress.com/2015/02/28/c-wrapper-class-for-shared-memory/

Author: Steven B Walker
Date:   11/20/18
*/

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstring>
#include "mosq_shm.h"


const std::string MosqSharedMem::lockSemaphoreName = "/mosemaphore";

MosqSharedMem::MosqSharedMem(int at_mode) :m_sName("mosMemory"), m_iD(-1),
m_SemID(NULL), m_nSize(0), m_realTimeData_ptr(NULL), m_errFile("/home/pi/Mosquito_Log/error_log.txt")
{
	try
	{
		// Semaphore open
		m_SemID = sem_open(lockSemaphoreName.c_str(), O_CREAT, S_IRUSR | S_IWUSR, 1);

        // create and attach data
		Create(sizeof(SensorData));
		Attach(at_mode);
	}
	catch (std::exception& ex)
    {
        PrintErrMsg(ex.what());
		exit(1);
    }
}

MosqSharedMem::~MosqSharedMem()
{
        Clear();
}

////////////////////////////////////////////////////////////////////////////////////////////
//			                	SHARED MEMORY FUNCTIONS	                				  //
////////////////////////////////////////////////////////////////////////////////////////////

bool MosqSharedMem::Create(size_t nSize, int mode /*= READ_WRITE*/)
{
	m_nSize = nSize;
	m_iD = shm_open(m_sName.c_str(), O_CREAT | mode, S_IRWXU | S_IRWXG);

	if (m_iD < 0)
	{
		switch (errno)
		{
		case EACCES:
			throw MSMexception("Permission Exception ");
			break;
		case EEXIST:
			throw MSMexception("Shared memory object specified by name already exists.");
			break;
		case EINVAL:
			throw MSMexception("Invalid shared memory name passed.");
			break;
		case EMFILE:
			throw MSMexception("The process already has the maximum number of files open.");
			break;
		case ENAMETOOLONG:
			throw MSMexception("The length of name exceeds PATH_MAX.");
			break;
		case ENFILE:
			throw MSMexception("The limit on the total number of files open on the system has been reached");
			break;
		default:
			throw MSMexception("Invalid exception occurred in shared memory creation");
			break;
		}
	}

	// adjust newly created memory segment to correct size and initialize data on first operation
	ftruncate(m_iD, m_nSize);

	return true;
}

bool MosqSharedMem::Attach(int mode /*= A_READ | A_WRITE*/)
{
	/* requesting the shared segment    --  mmap() */
	m_realTimeData_ptr = (SensorData*)mmap(NULL, m_nSize, mode, MAP_SHARED, m_iD, 0);
	if (m_realTimeData_ptr == NULL)
	{
		throw MSMexception("Exception in attaching the shared memory region");
	}

	return true;
}

void MosqSharedMem::Detach()
{
	munmap(m_realTimeData_ptr, m_nSize);
}

void MosqSharedMem::Lock() const
{
	sem_wait(m_SemID);
}

void MosqSharedMem::UnLock() const
{
	sem_post(m_SemID);
}

void MosqSharedMem::Clear()
{
	if (m_iD != -1)
	{
		if (shm_unlink(m_sName.c_str()) < 0)
		{
//			perror("shm_unlink");
		}
	}

	// Semaphore unlink: Remove a named semaphore  from the system.

	if (m_SemID != NULL)
	{
		// Semaphore Close: Close a named semaphore

		if (sem_close(m_SemID) < 0)
		{
//			perror("sem_close");
		}

		// Semaphore unlink: Remove a named semaphore  from the system.

		if (sem_unlink(lockSemaphoreName.c_str()) < 0)
		{
//			perror("sem_unlink");
		}
	}
}


////////////////////////////////////////////////////////////////////////////
//		               		DATA ACCESS FUNCTIONS                		  //
////////////////////////////////////////////////////////////////////////////

void MosqSharedMem::SetFan1RPM(const int& rpm)
{
	Lock();
	m_realTimeData_ptr->fan1RPM = rpm;
	UnLock();
}

void MosqSharedMem::SetFan2RPM(const int& rpm)
{
        Lock();
        m_realTimeData_ptr->fan2RPM = rpm;
        UnLock();
}

void MosqSharedMem::SetTotalFanRev(const long int& tr)
{
	Lock();
	m_realTimeData_ptr->totalFanRotations = tr;
	UnLock();
}

void MosqSharedMem::SetBattVoltage(const float& bv)
{
	Lock();
	m_realTimeData_ptr->battVoltage = bv;
	UnLock();
}

void MosqSharedMem::SetTemperature(const float& tp)
{
	Lock();
	m_realTimeData_ptr->temperature = tp;
	UnLock();
}

void MosqSharedMem::SetCO2Pressure(const float& pr)
{
	Lock();
	m_realTimeData_ptr->CO2pressure = pr;
	UnLock();
}

int MosqSharedMem::GetFan1RPM(void) const
{
	Lock();
	int temp = m_realTimeData_ptr->fan1RPM;
	UnLock();
	return temp;
}

int MosqSharedMem::GetFan2RPM(void) const
{
        Lock();
        int temp = m_realTimeData_ptr->fan2RPM;
        UnLock();
        return temp;
}


long int MosqSharedMem::GetTotalFanRev(void) const
{
	Lock();
	int temp = m_realTimeData_ptr->totalFanRotations;
	UnLock();
	return temp;
}

float MosqSharedMem::GetBattVoltage(void) const
{
	Lock();
	int temp = m_realTimeData_ptr->battVoltage;
	UnLock();
        return temp;
}

float MosqSharedMem::GetTemperature(void) const
{
	Lock();
	int temp = m_realTimeData_ptr->temperature;
        UnLock();
        return temp;
}

float MosqSharedMem::GetCO2Pressure(void) const
{
	Lock();
	int temp = m_realTimeData_ptr->CO2pressure;
        UnLock();
        return temp;
}

// utility for debug purposes
void MosqSharedMem::Dump ()
{
        int s = 0;
        int* sval = &s;
        sem_getvalue(m_SemID,sval);

	std::cout << "Current shared memory status: \n\n";
	std::cout << "m_sName = " << m_sName << '\n'
		  << "m_iD    = " << m_iD << '\n'
		  << "m_SemID = " << m_SemID << "  (pointer address)\n"
		  << "m_nSize = " << m_nSize << '\n'
		  << "dataptr = " << m_realTimeData_ptr << "  (pointer address)\n"
		  << "semValu = " << *sval << "\n";
    std::cout << "Registered PIDs:\n";
    for (size_t i = 0; i < m_realTimeData_ptr->numProcesses; ++i)
        std::cout << m_realTimeData_ptr->procPIDs[i] << '\n';
}

// allows sensor processes to register process IDs to shared memory
// this allows for central program to initiate shutown of these
// dependent processes when ReleaseSensors() is called
void MosqSharedMem::RegisterPID ()
{
    m_realTimeData_ptr->procPIDs[m_realTimeData_ptr->numProcesses] = getpid();
    ++m_realTimeData_ptr->numProcesses;
}

void MosqSharedMem::ReleaseSensors()
{
    for (size_t i = 0; i < m_realTimeData_ptr->numProcesses; ++i)
        kill(m_realTimeData_ptr->procPIDs[i], SIGQUIT);
}

void MosqSharedMem::PrintErrMsg (const char* msg)
{
	std::ofstream errfile;
	errfile.open(m_errFile.c_str(), std::ios::out | std::ios::app);

	time_t rawtime;
	time(&rawtime);
	struct tm* timeDate = localtime(&rawtime);

	char* timestamp = asctime(timeDate);
	timestamp[strlen(timestamp) - 1] = 0; // remove newline character from string

	errfile << timestamp << ": SHM INIT ERROR -- " << msg << '\n';
	errfile.close();
}


