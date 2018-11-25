/*
MosqSharedMem.cpp contains implementations for a shared memory wrapper with exception class extension to be used with the
FSU Mosquito Trap Project with software implemented on the Raspberry Pi platform. This library will allow all the device drivers
and other programs to communicate via shared memory, allowing them to have separate program implemenations. This program is loosely
based upon an example found at:

https://cppcodetips.wordpress.com/2015/02/28/c-wrapper-class-for-shared-memory/

Author: Steven B Walker
Date:   11/20/18
*/

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>
#include "mosq_shm.h"

const std::string MosqSharedMem::lockSemaphoreName = "/mosemaphore";

MosqSharedMem::MosqSharedMem(int at_mode) :m_sName("mosMemory"), m_iD(-1),
m_SemID(NULL), m_nSize(0), realTimeData_ptr(NULL)
{
	// Semaphore open
	m_SemID = sem_open(lockSemaphoreName.c_str(), O_CREAT, S_IRUSR | S_IWUSR, 1);

	// create and attach data																	//avoiding subsequent calls to shm_open
	Create(sizeof(SensorData));
	Attach(at_mode);
}

MosqSharedMem::~MosqSharedMem()
{
        Clear();
}

////////////////////////////////////////////////////////////////////////////////////////////
//				SHARED MEMORY FUNCTIONS					  //
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
	realTimeData_ptr = (SensorData*)mmap(NULL, m_nSize, mode, MAP_SHARED, m_iD, 0);
	if (realTimeData_ptr == NULL)
	{
		throw MSMexception("Exception in attaching the shared memory region");
	}

	return true;
}

void MosqSharedMem::Detach()
{
	munmap(realTimeData_ptr, m_nSize);
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
	/**
	* Semaphore unlink: Remove a named semaphore  from the system.
	*/
	if (m_SemID != NULL)
	{
		/**
		* Semaphore Close: Close a named semaphore
		*/
		if (sem_close(m_SemID) < 0)
		{
//			perror("sem_close");
		}
		/**
		* Semaphore unlink: Remove a named semaphore  from the system.
		*/
		if (sem_unlink(lockSemaphoreName.c_str()) < 0)
		{
//			perror("sem_unlink");
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
//				DATA ACCESS FUNCTIONS	                   		  //
////////////////////////////////////////////////////////////////////////////////////////////

void MosqSharedMem::SetFanRPM(const int& rpm)
{
	Lock();
	realTimeData_ptr->fanRPM = rpm;
	UnLock();
}

void MosqSharedMem::SetTotalFanRev(const int& tr)
{
	Lock();
	realTimeData_ptr->totalFanRotations = tr;
	UnLock();
}

void MosqSharedMem::SetBattVoltage(const int& bv)
{
	Lock();
	realTimeData_ptr->battVoltage = bv;
	UnLock();
}

void MosqSharedMem::SetTemperature(const int& tp)
{
	Lock();
	realTimeData_ptr->temperature = tp;
	UnLock();
}

void MosqSharedMem::SetCO2Pressure(const int& pr)
{
	Lock();
	realTimeData_ptr->CO2pressure = pr;
	UnLock();
}

int MosqSharedMem::GetFanRPM(void) const
{
	Lock();
	int temp = realTimeData_ptr->fanRPM;
	UnLock();
	return temp;
}

int MosqSharedMem::GetTotalFanRev(void) const
{
	Lock();
	int temp = realTimeData_ptr->totalFanRotations;
	UnLock();
	return temp;
}

int MosqSharedMem::GetBattVoltage(void) const
{
	Lock();
	int temp = realTimeData_ptr->battVoltage;
	UnLock();
        return temp;
}

int MosqSharedMem::GetTemperature(void) const
{
	Lock();
	int temp = realTimeData_ptr->temperature;
        UnLock();
        return temp;
}

int MosqSharedMem::GetCO2Pressure(void) const
{
	Lock();
	int temp = realTimeData_ptr->CO2pressure;
        UnLock();
        return temp;
}

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
		  << "dataptr = " << realTimeData_ptr << "  (pointer address)\n"
		  << "semValu = " << *sval << "\n\n";
}
